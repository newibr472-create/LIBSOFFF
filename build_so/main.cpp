/*
 * BGMI 4.4.0 ARM64 — Proper SDK Integration
 * 
 * Entry: __attribute__((constructor)) → pthread
 * SDK: NIKON 4.4.0 (ProcessEvent, FindObject, GetBonePos, K2_DrawLine)
 * Rendering: UCanvas PostRender hook (draws using game's own renderer)
 * Bypass: 809 verified patches via KittyMemory-style mprotect
 */

#include "SDK.hpp"
using namespace SDK;

#include <pthread.h>
#include <unistd.h>
#include <dlfcn.h>
#include <cmath>
#include <vector>
#include <atomic>
#include <cstdio>

#include "Main/Tools.h"
#include "Main/MemoryTools.h"
#include "Main/InlineHook.h"
#include "Bypass.h"

// ============================================================================
// LOGGING (resolved at runtime via dlsym)
// ============================================================================
typedef int (*android_log_fn)(int, const char*, const char*, ...);
static android_log_fn g_log = nullptr;
#define LOGI(...) do { if(g_log) g_log(4, "BGMI", __VA_ARGS__); } while(0)
#define LOGE(...) do { if(g_log) g_log(6, "BGMI", __VA_ARGS__); } while(0)

// ============================================================================
// GAME OFFSETS (from reference working implementation)
// ============================================================================
static uintptr_t GNames_Offset = 0x8394964;
static uintptr_t GUObject_Offset = 0xe22f8d0;
static uintptr_t GetActorArray_Offset = 0xa1018ac;
static uintptr_t UE4 = 0;

// ============================================================================
// GLOBAL STATE
// ============================================================================
static ASTExtraPlayerCharacter* g_LocalPlayer = nullptr;
static APlayerController* g_PlayerController = nullptr;
static UFont* g_Font = nullptr;
static std::atomic<bool> g_Initialized{false};

// ============================================================================
// COLORS
// ============================================================================
#define COLOR_RED    FLinearColor(1.f, 0.f, 0.f, 1.f)
#define COLOR_GREEN  FLinearColor(0.f, 1.f, 0.f, 1.f)
#define COLOR_WHITE  FLinearColor(1.f, 1.f, 1.f, 1.f)
#define COLOR_YELLOW FLinearColor(1.f, 1.f, 0.f, 1.f)
#define COLOR_BLACK  FLinearColor(0.f, 0.f, 0.f, 1.f)

// ============================================================================
// CONFIG
// ============================================================================
struct Config {
    bool espEnabled = true;
    bool espSkeleton = true;
    bool espHealth = true;
    bool espDistance = true;
    bool espBox = true;
    bool aimbotEnabled = true;
    float aimbotSmooth = 5.0f;
    float aimbotFOV = 100.0f;
    bool largeHitbox = true;
    float hitboxScale = 2.5f;
    float maxDistance = 300.0f;
} g_Config;

// ============================================================================
// SDK INITIALIZATION
// ============================================================================
static TNameEntryArray* GetGNames() {
    return ((TNameEntryArray*(*)())(UE4 + GNames_Offset))();
}

static bool InitSDK() {
    UE4 = Tools::GetLibBase("libUE4.so");
    if (!UE4) return false;
    
    FName::GNames = GetGNames();
    if (!FName::GNames) return false;
    
    UObject::GUObjectArray = (FUObjectArray*)(UE4 + GUObject_Offset);
    if (!UObject::GUObjectArray) return false;
    
    LOGI("[+] SDK initialized: UE4=0x%lx GNames=%p GUObject=%p", UE4, FName::GNames, UObject::GUObjectArray);
    return true;
}

// ============================================================================
// WORLD ACCESS (proper SDK way — walks UEngine→GameViewport→World)
// ============================================================================
static int g_EngineIdx = 0;
static UWorld* GetFullWorld() {
    auto& objects = UObject::GetGlobalObjects();
    
    if (g_EngineIdx > 0) {
        auto obj = objects.GetByIndex(g_EngineIdx);
        if (obj && Tools::IsPtrValid(obj)) {
            UEngine* engine = (UEngine*)obj;
            if (Tools::IsPtrValid(engine->GameViewport)) {
                auto world = engine->GameViewport->World;
                if (Tools::IsPtrValid(world) && Tools::IsPtrValid(world->PersistentLevel))
                    return world;
            }
        }
        g_EngineIdx = 0;
    }
    
    for (int i = 0; i < objects.Num(); i++) {
        auto obj = objects.GetByIndex(i);
        if (!obj || !Tools::IsPtrValid(obj)) continue;
        if (obj->IsA(UEngine::StaticClass())) {
            UEngine* engine = (UEngine*)obj;
            if (Tools::IsPtrValid(engine->GameViewport) && 
                Tools::IsPtrValid(engine->GameViewport->World)) {
                g_EngineIdx = i;
                return engine->GameViewport->World;
            }
        }
    }
    return nullptr;
}

// ============================================================================
// ACTOR ITERATION (uses game's GetActorArray function)
// ============================================================================
struct ActorArray { uintptr_t base; int32_t count; int32_t max; };

static std::vector<AActor*> GetActors() {
    std::vector<AActor*> result;
    auto world = GetFullWorld();
    if (!world || !Tools::IsPtrValid(world->PersistentLevel)) return result;
    
    ActorArray actors = *((ActorArray*(*)(uintptr_t))(UE4 + GetActorArray_Offset))(
        reinterpret_cast<uintptr_t>(world->PersistentLevel));
    
    if (actors.count <= 0 || !Tools::IsPtrValid((void*)actors.base)) return result;
    
    result.reserve(actors.count);
    for (int i = 0; i < actors.count; i++) {
        auto actor = *(AActor**)(actors.base + (i * sizeof(uintptr_t)));
        if (actor && Tools::IsPtrValid(actor))
            result.push_back(actor);
    }
    return result;
}

// ============================================================================
// BONE HELPER (uses SDK's GetBonePos with FName)
// ============================================================================
static FVector GetBoneLocation(ASTExtraPlayerCharacter* player, const char* boneName) {
    if (!player || !Tools::IsPtrValid(player)) return FVector();
    return player->GetBonePos(FName(boneName), FVector());
}

// ============================================================================
// MATH HELPERS
// ============================================================================
static float GetDistance(FVector a, FVector b) {
    float dx = a.X - b.X, dy = a.Y - b.Y, dz = a.Z - b.Z;
    return sqrtf(dx*dx + dy*dy + dz*dz) / 100.0f; // cm to meters
}

static FRotator CalcAimRotation(FVector from, FVector to) {
    FVector delta;
    delta.X = to.X - from.X;
    delta.Y = to.Y - from.Y;
    delta.Z = to.Z - from.Z;
    
    float dist = sqrtf(delta.X*delta.X + delta.Y*delta.Y);
    FRotator rot;
    rot.Pitch = atan2f(delta.Z, dist) * (180.0f / 3.14159265f);
    rot.Yaw = atan2f(delta.Y, delta.X) * (180.0f / 3.14159265f);
    rot.Roll = 0;
    return rot;
}

static FRotator InterpRotation(FRotator current, FRotator target, float smooth) {
    FRotator result;
    result.Pitch = current.Pitch + (target.Pitch - current.Pitch) / smooth;
    result.Yaw = current.Yaw + (target.Yaw - current.Yaw) / smooth;
    result.Roll = 0;
    return result;
}

// ============================================================================
// ESP DRAWING (called from PostRender hook with UCanvas)
// ============================================================================
static void DrawESP(UCanvas* Canvas, APlayerController* myPC, ASTExtraPlayerCharacter* myChar) {
    if (!Canvas || !myPC || !myChar) return;
    if (!g_Config.espEnabled) return;
    
    // Get font from engine
    if (!g_Font) {
        for (int i = 0; i < UObject::GetGlobalObjects().Num() && !g_Font; i++) {
            auto obj = UObject::GetGlobalObjects().GetByIndex(i);
            if (obj && obj->IsA(UEngine::StaticClass())) {
                g_Font = ((UEngine*)obj)->SmallFont;
            }
        }
    }
    
    // Get my TeamID
    int myTeamID = *(int*)((uintptr_t)myChar + 0x0604);
    
    // Best aimbot target
    float closestDist = g_Config.aimbotFOV;
    FVector bestHead = FVector();
    bool hasTarget = false;
    
    // Iterate actors
    auto actors = GetActors();
    for (auto actor : actors) {
        if (!actor || actor == (AActor*)myChar) continue;
        if (!actor->IsA(ASTExtraPlayerCharacter::StaticClass())) continue;
        
        auto enemy = (ASTExtraPlayerCharacter*)actor;
        
        // Check alive
        float health = *(float*)((uintptr_t)enemy + 0x0E60);
        uint8_t deadBits = *(uint8_t*)((uintptr_t)enemy + 0x0E7C);
        if (health <= 0 || (deadBits & 0x01)) continue;
        
        // Check team
        int theirTeam = *(int*)((uintptr_t)enemy + 0x0604);
        if (theirTeam == myTeamID) continue;
        
        // Get positions using SDK GetBonePos
        FVector headPos = GetBoneLocation(enemy, "Head");
        FVector rootPos = GetBoneLocation(enemy, "Root");
        
        if (headPos.X == 0 && headPos.Y == 0 && headPos.Z == 0) continue;
        
        // Distance check
        FVector myPos = GetBoneLocation(myChar, "Root");
        float dist = GetDistance(myPos, rootPos);
        if (dist > g_Config.maxDistance) continue;
        
        // Project to screen
        FVector2D screenHead, screenFoot;
        bool headOnScreen = myPC->ProjectWorldLocationToScreen(headPos, false, &screenHead);
        bool footOnScreen = myPC->ProjectWorldLocationToScreen(rootPos, false, &screenFoot);
        
        if (!headOnScreen && !footOnScreen) continue;
        
        // --- DRAW ESP ---
        float boxHeight = screenFoot.Y - screenHead.Y;
        float boxWidth = boxHeight * 0.5f;
        
        // Box ESP
        if (g_Config.espBox && boxHeight > 5) {
            FVector2D topLeft = {screenHead.X - boxWidth/2, screenHead.Y};
            FVector2D topRight = {screenHead.X + boxWidth/2, screenHead.Y};
            FVector2D botLeft = {screenFoot.X - boxWidth/2, screenFoot.Y};
            FVector2D botRight = {screenFoot.X + boxWidth/2, screenFoot.Y};
            
            Canvas->K2_DrawLine(topLeft, topRight, 1.5f, COLOR_RED);
            Canvas->K2_DrawLine(topRight, botRight, 1.5f, COLOR_RED);
            Canvas->K2_DrawLine(botRight, botLeft, 1.5f, COLOR_RED);
            Canvas->K2_DrawLine(botLeft, topLeft, 1.5f, COLOR_RED);
        }
        
        // Skeleton ESP
        if (g_Config.espSkeleton) {
            const char* bones[][2] = {
                {"Head", "neck_01"},
                {"neck_01", "spine_02"},
                {"spine_02", "spine_01"},
                {"spine_01", "pelvis"},
                {"neck_01", "upperarm_l"},
                {"upperarm_l", "lowerarm_l"},
                {"lowerarm_l", "hand_l"},
                {"neck_01", "upperarm_r"},
                {"upperarm_r", "lowerarm_r"},
                {"lowerarm_r", "hand_r"},
                {"pelvis", "thigh_l"},
                {"thigh_l", "calf_l"},
                {"calf_l", "foot_l"},
                {"pelvis", "thigh_r"},
                {"thigh_r", "calf_r"},
                {"calf_r", "foot_r"},
            };
            
            for (auto& pair : bones) {
                FVector boneA = GetBoneLocation(enemy, pair[0]);
                FVector boneB = GetBoneLocation(enemy, pair[1]);
                FVector2D screenA, screenB;
                if (myPC->ProjectWorldLocationToScreen(boneA, false, &screenA) &&
                    myPC->ProjectWorldLocationToScreen(boneB, false, &screenB)) {
                    Canvas->K2_DrawLine(screenA, screenB, 1.2f, COLOR_GREEN);
                }
            }
        }
        
        // Health bar
        if (g_Config.espHealth && boxHeight > 10) {
            float maxHealth = *(float*)((uintptr_t)enemy + 0x0E64);
            float healthPct = (maxHealth > 0) ? (health / maxHealth) : 0;
            
            float barX = screenHead.X - boxWidth/2 - 5;
            float barTop = screenHead.Y;
            float barBot = screenFoot.Y;
            float barHeight = (barBot - barTop) * healthPct;
            
            FLinearColor hpColor;
            hpColor.R = 1.0f - healthPct;
            hpColor.G = healthPct;
            hpColor.B = 0;
            hpColor.A = 1.0f;
            
            FVector2D hpTop = {barX, barBot - barHeight};
            FVector2D hpBot = {barX, barBot};
            Canvas->K2_DrawLine(hpTop, hpBot, 3.0f, hpColor);
        }
        
        // Distance text
        if (g_Config.espDistance && g_Font && headOnScreen) {
            char distText[32];
            snprintf(distText, sizeof(distText), "%.0fm", dist);
            FVector2D textPos = {screenHead.X, screenHead.Y - 15};
            Canvas->K2_DrawText(g_Font, FString(distText), textPos, COLOR_WHITE,
                              0, COLOR_BLACK, FVector2D(), true, false, false, COLOR_BLACK);
        }
        
        // Aimbot target selection
        if (g_Config.aimbotEnabled && headOnScreen) {
            float screenCX = Canvas->SizeX / 2.0f;
            float screenCY = Canvas->SizeY / 2.0f;
            float dx = screenHead.X - screenCX;
            float dy = screenHead.Y - screenCY;
            float crosshairDist = sqrtf(dx*dx + dy*dy);
            
            if (crosshairDist < closestDist) {
                closestDist = crosshairDist;
                bestHead = headPos;
                hasTarget = true;
            }
        }
    }
    
    // Draw FOV circle
    if (g_Config.aimbotEnabled) {
        float cx = Canvas->SizeX / 2.0f;
        float cy = Canvas->SizeY / 2.0f;
        float radius = g_Config.aimbotFOV;
        int segments = 32;
        for (int i = 0; i < segments; i++) {
            float a1 = (2 * 3.14159f * i) / segments;
            float a2 = (2 * 3.14159f * (i+1)) / segments;
            FVector2D p1 = {cx + cosf(a1)*radius, cy + sinf(a1)*radius};
            FVector2D p2 = {cx + cosf(a2)*radius, cy + sinf(a2)*radius};
            Canvas->K2_DrawLine(p1, p2, 1.0f, COLOR_WHITE);
        }
    }
    
    // Apply aimbot
    if (hasTarget && g_Config.aimbotEnabled && g_PlayerController) {
        auto camMgr = (APlayerCameraManager*)(*(void**)((uintptr_t)g_PlayerController + 0x0548));
        if (Tools::IsPtrValid(camMgr)) {
            // CameraCache.POV.Location at 0x0530
            FVector camLoc = *(FVector*)((uintptr_t)camMgr + 0x0530);
            
            FRotator targetRot = CalcAimRotation(camLoc, bestHead);
            FRotator currentRot = ((AController*)g_PlayerController)->GetControlRotation();
            FRotator smoothed = InterpRotation(currentRot, targetRot, g_Config.aimbotSmooth);
            ((AController*)g_PlayerController)->SetControlRotation(smoothed, FString(L""));
        }
    }
}

// ============================================================================
// LARGE HITBOX
// ============================================================================
static void ApplyLargeHitbox(ASTExtraPlayerCharacter* enemy) {
    if (!g_Config.largeHitbox) return;
    // CapsuleComponent at 0x0520 in ACharacter
    void* capsule = *(void**)((uintptr_t)enemy + 0x0520);
    if (!Tools::IsPtrValid(capsule)) return;
    // CapsuleRadius typically at offset 0x02EC in UCapsuleComponent
    float* radius = (float*)((uintptr_t)capsule + 0x02EC);
    if (*radius < 100.0f) { // Only scale if not already scaled
        *radius *= g_Config.hitboxScale;
    }
}

// ============================================================================
// POSTRENDER HOOK (game calls this every frame with UCanvas)
// ============================================================================
void (*orig_PostRender)(void* thiz, UCanvas* Canvas) = nullptr;

void hook_PostRender(void* thiz, UCanvas* Canvas) {
    if (orig_PostRender) orig_PostRender(thiz, Canvas);
    
    if (!g_Initialized || !Canvas || !Tools::IsPtrValid(Canvas)) return;
    
    auto world = GetFullWorld();
    if (!world) return;
    
    // Get local player
    auto gameInstance = (UGameInstance*)world->OwningGameInstance;
    if (!Tools::IsPtrValid(gameInstance)) return;
    
    auto& localPlayers = *(TArray<ULocalPlayer*>*)((uintptr_t)gameInstance + 0x0048);
    if (localPlayers.Num() < 1 || !Tools::IsPtrValid(localPlayers[0])) return;
    
    auto localPlayer = localPlayers[0];
    auto pc = *(APlayerController**)((uintptr_t)localPlayer + 0x0030);
    if (!Tools::IsPtrValid(pc)) return;
    
    auto pawn = *(APawn**)((uintptr_t)pc + 0x0528); // AcknowledgedPawn
    if (!Tools::IsPtrValid(pawn)) return;
    
    g_PlayerController = pc;
    g_LocalPlayer = (ASTExtraPlayerCharacter*)pawn;
    
    // Island/Lobby bypass: check if enough players
    auto gameState = world->GameState;
    if (!Tools::IsPtrValid(gameState)) return;
    auto& playerArray = *(TArray<void*>*)((uintptr_t)gameState + 0x04C8);
    if (playerArray.Num() < 10) return; // Skip in lobby
    
    DrawESP(Canvas, pc, (ASTExtraPlayerCharacter*)pawn);
}

// ============================================================================
// FIND AND HOOK POSTRENDER
// ============================================================================
static bool HookPostRender() {
    // Find UGameViewportClient instance
    auto viewport = UObject::FindObject<UGameViewportClient>(
        "GameViewportClient Transient.UAEGameEngine_1.GameViewportClient_1");
    
    if (!viewport) {
        // Try generic search
        for (int i = 0; i < UObject::GetGlobalObjects().Num(); i++) {
            auto obj = UObject::GetGlobalObjects().GetByIndex(i);
            if (obj && obj->IsA(UGameViewportClient::StaticClass())) {
                viewport = (UGameViewportClient*)obj;
                break;
            }
        }
    }
    
    if (!viewport) {
        LOGE("[-] Could not find UGameViewportClient");
        return false;
    }
    
    // Get vtable and hook PostRender (typically vtable index ~100-110 for UGameViewportClient)
    // Alternative: Hook AHUD::PostRender which is simpler
    // The AHUD PostRender function signature: void PostRender(UCanvas* Canvas)
    
    // Find AHUD instance from PlayerController->MyHUD (offset 0x0540)
    auto world = GetFullWorld();
    if (!world) return false;
    
    auto gameInstance = (UGameInstance*)world->OwningGameInstance;
    if (!Tools::IsPtrValid(gameInstance)) return false;
    
    auto& localPlayers = *(TArray<ULocalPlayer*>*)((uintptr_t)gameInstance + 0x0048);
    if (localPlayers.Num() < 1) return false;
    
    auto pc = *(APlayerController**)((uintptr_t)localPlayers[0] + 0x0030);
    if (!Tools::IsPtrValid(pc)) return false;
    
    void* hud = *(void**)((uintptr_t)pc + 0x0540); // MyHUD
    if (!Tools::IsPtrValid(hud)) return false;
    
    // Get HUD's vtable
    void** vtable = *(void***)hud;
    if (!Tools::IsPtrValid(vtable)) return false;
    
    // PostRender is typically at vtable index 89-92 for AHUD
    // We'll scan for it by checking which vtable entries are in libUE4.so range
    uintptr_t ue4End = Tools::GetLibEnd("libUE4.so");
    
    // PostRender virtual index for AHUD in UE4 4.x is typically 87
    // We hook vtable[87] which is AHUD::PostRender
    int postRenderIdx = 87;
    
    // Swap vtable entry
    orig_PostRender = (void(*)(void*, UCanvas*))vtable[postRenderIdx];
    
    uintptr_t page = (uintptr_t)&vtable[postRenderIdx] & ~0xFFF;
    mprotect((void*)page, 0x2000, PROT_READ|PROT_WRITE);
    vtable[postRenderIdx] = (void*)hook_PostRender;
    
    LOGI("[+] Hooked PostRender at vtable[%d] = %p → %p", postRenderIdx, orig_PostRender, hook_PostRender);
    return true;
}

// ============================================================================
// MAIN THREAD
// ============================================================================
static void* main_thread(void*) {
    // Resolve android log
    g_log = (android_log_fn)dlsym(RTLD_DEFAULT, "__android_log_print");
    
    LOGI("[*] ====================================");
    LOGI("[*] BGMI 4.4.0 — SDK Bypass + ESP");
    LOGI("[*] ====================================");
    
    // Wait for libUE4.so to load
    while (!Tools::GetLibBase("libUE4.so")) {
        usleep(500000);
    }
    LOGI("[+] libUE4.so loaded at 0x%lx", Tools::GetLibBase("libUE4.so"));
    
    // Wait for full initialization
    sleep(5);
    
    // Initialize SDK
    if (!InitSDK()) {
        LOGE("[-] Failed to initialize SDK");
        return nullptr;
    }
    
    // Apply all 809 bypass patches
    ApplyAllBypasses();
    LOGI("[+] Applied 809 bypass patches");
    
    // Wait for game world
    while (!GetFullWorld()) {
        usleep(1000000);
    }
    LOGI("[+] World found");
    
    // Wait a bit more for HUD to spawn
    sleep(3);
    
    // Hook PostRender
    for (int attempt = 0; attempt < 10; attempt++) {
        if (HookPostRender()) {
            LOGI("[+] PostRender hooked successfully!");
            g_Initialized = true;
            break;
        }
        LOGI("[*] Waiting for HUD... attempt %d", attempt);
        sleep(2);
    }
    
    if (!g_Initialized) {
        LOGE("[-] Failed to hook PostRender after 10 attempts");
    }
    
    // Keep thread alive
    while (true) {
        sleep(10);
    }
    
    return nullptr;
}

// ============================================================================
// ENTRY POINT
// ============================================================================
__attribute__((constructor))
void _init() {
    pthread_t ptid;
    pthread_create(&ptid, nullptr, main_thread, nullptr);
    pthread_detach(ptid);
}
