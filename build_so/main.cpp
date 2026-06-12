/*
 * BGMI 4.4.0 ARM64 — ESP + Aimbot + Large Hitbox + 809 Bypass Patches
 * Architecture: Reference-matching (VTable PostRender hook, SDK ProcessEvent)
 * Entry: __attribute__((constructor)) → pthread
 * Rendering: UCanvas via PostRender VTable hook (index 134)
 * SDK: NIKON 4.4.0 compiled (GNames, GUObjectArray, ProcessEvent)
 */

#include "SDK.hpp"
using namespace SDK;
using namespace std;

#include <pthread.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <android/log.h>
// android/native_window_jni.h removed — not needed for core
#include <cmath>
#include <vector>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <atomic>
#include <chrono>
#include <signal.h>

#include "Main/Tools.h"
#include "Main/MemoryTools.h"
#include "Bypass.h"

// ============================================================================
// LOGGING
// ============================================================================
#define TAG "BGMI_MOD"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

// ============================================================================
// GAME OFFSETS (verified from reference — BGMI 4.4.0 ARM64)
// ============================================================================
uintptr_t GNames_Offset = 0x8394964;
uintptr_t GUObject_Offset = 0xe22f8d0;
uintptr_t GetActorArray_Offset = 0xa1018ac;
uintptr_t GNativeAndroidApp_Offset = 0xdf74398;
uintptr_t Actors_Offset = 0xA0;

// ============================================================================
// GLOBAL STATE
// ============================================================================
uintptr_t UE4 = 0;
struct android_app; // forward declare
android_app *g_App = nullptr;
ASTExtraPlayerCharacter *g_LocalPlayer = nullptr;
ASTExtraPlayerController *g_PlayerController = nullptr;
int screenWidth = 0, screenHeight = 0;

// ============================================================================
// COLORS
// ============================================================================
#define COLOR_RED     FLinearColor(1.f, 0.f, 0.f, 1.f)
#define COLOR_GREEN   FLinearColor(0.f, 1.f, 0.f, 1.f)
#define COLOR_WHITE   FLinearColor(1.f, 1.f, 1.f, 1.f)
#define COLOR_YELLOW  FLinearColor(1.f, 1.f, 0.f, 1.f)
#define COLOR_BLACK   FLinearColor(0.f, 0.f, 0.f, 1.f)
#define COLOR_CYAN    FLinearColor(0.f, 1.f, 1.f, 1.f)
#define COLOR_ORANGE  FLinearColor(1.f, 0.4f, 0.f, 1.f)

// ============================================================================
// CONFIG
// ============================================================================
struct sConfig {
    bool Enable = true;
    struct {
        bool Skeleton = true;
        bool Box = true;
        bool Health = true;
        bool Distance = true;
        bool Line = false;
    } ESP;
    struct {
        bool Enable = true;
        float Smooth = 5.0f;
        float FOV = 120.0f;
        bool Prediction = true;
        bool VisCheck = false;
    } AimBot;
    struct {
        bool Enable = true;
        float Scale = 2.5f;
    } Hitbox;
    float MaxDistance = 300.0f;
};
static sConfig Config;

// ============================================================================
// MATH HELPERS
// ============================================================================
static float VectorDistance(FVector a, FVector b) {
    float dx = a.X - b.X, dy = a.Y - b.Y, dz = a.Z - b.Z;
    return sqrtf(dx*dx + dy*dy + dz*dz);
}

static FRotator ToRotator(FVector from, FVector to) {
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

static void clampAngles(FRotator &rot) {
    while (rot.Pitch > 89.f) rot.Pitch -= 360.f;
    while (rot.Pitch < -89.f) rot.Pitch += 360.f;
    while (rot.Yaw > 180.f) rot.Yaw -= 360.f;
    while (rot.Yaw < -180.f) rot.Yaw += 360.f;
}

// ============================================================================
// GNames GETTER (calls function at offset — returns TNameEntryArray*)
// ============================================================================
TNameEntryArray *GetGNames() {
    return ((TNameEntryArray*(*)())(UE4 + GNames_Offset))();
}

// ============================================================================
// GET FULL WORLD (walks UEngine → GameViewport → World — cached)
// ============================================================================
static UWorld *GEWorld = nullptr;
static int GWorldNum = 0;
static TUObjectArray gobjects;

UWorld *GetFullWorld() {
    if (GWorldNum == 0) {
        gobjects = UObject::GUObjectArray->ObjObjects;
        for (int i = 0; i < gobjects.Num(); i++) {
            auto obj = gobjects.GetByIndex(i);
            if (!obj) continue;
            if (obj->IsA(UEngine::StaticClass())) {
                auto GEngine = (UEngine*)obj;
                if (GEngine && Tools::IsPtrValid(GEngine)) {
                    auto ViewPort = GEngine->GameViewport;
                    if (ViewPort && Tools::IsPtrValid(ViewPort)) {
                        GEWorld = ViewPort->World;
                        GWorldNum = i;
                        if (!GEWorld || !Tools::IsPtrValid(GEWorld) || !GEWorld->NetDriver) {
                            g_LocalPlayer = nullptr;
                            g_PlayerController = nullptr;
                            return nullptr;
                        }
                        return ViewPort->World;
                    }
                }
            }
        }
    } else {
        auto GEngine = (UEngine*)(gobjects.GetByIndex(GWorldNum));
        if (GEngine && Tools::IsPtrValid(GEngine)) {
            auto ViewPort = GEngine->GameViewport;
            if (ViewPort && Tools::IsPtrValid(ViewPort)) {
                GEWorld = ViewPort->World;
                if (!GEWorld || !Tools::IsPtrValid(GEWorld) || !GEWorld->NetDriver) {
                    g_LocalPlayer = nullptr;
                    g_PlayerController = nullptr;
                    return nullptr;
                }
                return ViewPort->World;
            }
        }
        GWorldNum = 0; // Reset cache if stale
    }
    return nullptr;
}

// ============================================================================
// GET GAME VIEWPORT (FindObject with retry loop)
// ============================================================================
static UGameViewportClient *GameViewport = nullptr;

UGameViewportClient *GetGameViewport() {
    while (!GameViewport) {
        GameViewport = UObject::FindObject<UGameViewportClient>(
            "GameViewportClient Transient.UAEGameEngine_1.GameViewportClient_1");
        if (!GameViewport) sleep(1);
    }
    return GameViewport;
}

// ============================================================================
// GET ACTORS (uses game's GetActorArray function at known offset)
// ============================================================================
std::vector<AActor*> GetActors() {
    auto World = GetFullWorld();
    if (!World) return {};
    auto PersistentLevel = World->PersistentLevel;
    if (!PersistentLevel || !Tools::IsPtrValid(PersistentLevel)) return {};

    struct GovnoArray { uintptr_t base; int32_t count; int32_t max; };
    static thread_local GovnoArray Actors{};
    Actors = *(((GovnoArray*(*)(uintptr_t))(UE4 + GetActorArray_Offset))
              (reinterpret_cast<uintptr_t>(PersistentLevel)));

    if (Actors.count <= 0 || !Tools::IsPtrValid((void*)Actors.base)) return {};

    std::vector<AActor*> actors;
    actors.reserve(Actors.count);
    for (int i = 0; i < Actors.count; i++) {
        auto Actor = *(uintptr_t*)(Actors.base + (i * sizeof(uintptr_t)));
        if (Actor && Tools::IsPtrValid((void*)Actor))
            actors.push_back(reinterpret_cast<AActor*>(Actor));
    }
    return actors;
}

// ============================================================================
// GET ALL ACTORS OF CLASS (SDK ProcessEvent via UGameplayStatics)
// ============================================================================
template<class T>
void GetAllActors(std::vector<T*> &Actors) {
    auto GWorld = GetFullWorld();
    if (!GWorld) return;
    TArray<AActor*> Actors2;
    UGameplayStatics::GetAllActorsOfClass((UObject*)GWorld, T::StaticClass(), &Actors2);
    for (int i = 0; i < Actors2.Num(); i++) {
        if (Actors2[i] && Tools::IsPtrValid(Actors2[i]))
            Actors.push_back((T*)Actors2[i]);
    }
}

// ============================================================================
// BONE LOCATION (SDK function — ProcessEvent internally)
// ============================================================================
FVector GetBoneLocation(ASTExtraPlayerCharacter *Actor, const char *BoneName) {
    if (!Actor || !Tools::IsPtrValid(Actor)) return FVector();
    return Actor->GetBonePos(FName(BoneName), FVector());
}

// ============================================================================
// AIMBOT TARGET SELECTION
// ============================================================================
ASTExtraPlayerCharacter* GetAimbotTarget() {
    if (!g_LocalPlayer || !g_PlayerController) return nullptr;

    float bestDist = Config.AimBot.FOV;
    ASTExtraPlayerCharacter* bestTarget = nullptr;

    std::vector<ASTExtraPlayerCharacter*> players;
    GetAllActors(players);

    for (auto player : players) {
        if (!player || !Tools::IsPtrValid(player)) continue;
        if (player->PlayerKey == g_LocalPlayer->PlayerKey) continue;
        if (player->TeamID == g_LocalPlayer->TeamID) continue;
        if (player->bDead) continue;
        if (player->Health <= 0) continue;

        FVector headPos = GetBoneLocation(player, "Head");
        if (headPos.X == 0 && headPos.Y == 0 && headPos.Z == 0) continue;

        FVector2D screenPos;
        if (g_PlayerController->ProjectWorldLocationToScreen(headPos, false, &screenPos)) {
            float dx = screenPos.X - (screenWidth / 2.0f);
            float dy = screenPos.Y - (screenHeight / 2.0f);
            float dist = sqrtf(dx*dx + dy*dy);
            if (dist < bestDist) {
                bestDist = dist;
                bestTarget = player;
            }
        }
    }
    return bestTarget;
}

// ============================================================================
// RENDER ESP (called from PostRender hook every frame)
// ============================================================================
void RenderESP(UCanvas *Canvas, int ScreenWidth, int ScreenHeight) {
    if (!Canvas || !Config.Enable) return;

    screenWidth = ScreenWidth;
    screenHeight = ScreenHeight;

    // --- Find local player ---
    ASTExtraPlayerCharacter *localPlayer = nullptr;
    ASTExtraPlayerController *localPlayerController = nullptr;

    auto GWorld = GetFullWorld();
    if (!GWorld) return;

    UNetDriver *NetDriver = GWorld->NetDriver;
    if (!NetDriver || !Tools::IsPtrValid(NetDriver)) return;

    UNetConnection *ServerConnection = NetDriver->ServerConnection;
    if (!ServerConnection || !Tools::IsPtrValid(ServerConnection)) return;

    localPlayerController = (ASTExtraPlayerController*)ServerConnection->PlayerController;
    if (!localPlayerController || !Tools::IsPtrValid(localPlayerController)) return;

    // Find local player character by PlayerKey
    std::vector<ASTExtraPlayerCharacter*> PlayerCharacters;
    GetAllActors(PlayerCharacters);

    for (auto actor : PlayerCharacters) {
        if (actor && actor->PlayerKey == localPlayerController->PlayerKey) {
            localPlayer = actor;
            break;
        }
    }
    if (!localPlayer) return;

    g_LocalPlayer = localPlayer;
    g_PlayerController = (ASTExtraPlayerController*)localPlayerController;

    // --- Large Hitbox ---
    if (Config.Hitbox.Enable && localPlayer->PartHitComponent) {
        auto &angles = localPlayer->PartHitComponent->ConfigCollisionDistSqAngles;
        for (int j = 0; j < angles.Num(); j++) {
            angles[j].Angle = 90.0f;
        }
        localPlayer->PartHitComponent->ConfigCollisionDistSqAngles = angles;
    }

    // --- Aimbot ---
    if (Config.AimBot.Enable && localPlayer->bIsWeaponFiring) {
        ASTExtraPlayerCharacter* target = GetAimbotTarget();
        if (target) {
            FVector targetAimPos = GetBoneLocation(target, "Head");

            // Bullet prediction
            if (Config.AimBot.Prediction) {
                auto WeaponMgr = localPlayer->WeaponManagerComponent;
                if (WeaponMgr && Tools::IsPtrValid(WeaponMgr)) {
                    auto weapon = (ASTExtraShootWeapon*)WeaponMgr->CurrentWeaponReplicated;
                    if (weapon && Tools::IsPtrValid(weapon)) {
                        auto shootComp = weapon->ShootWeaponComponent;
                        if (shootComp && Tools::IsPtrValid(shootComp)) {
                            auto entityComp = shootComp->ShootWeaponEntityComponent;
                            if (entityComp && Tools::IsPtrValid(entityComp)) {
                                FVector velocity = target->GetVelocity();
                                float dist = localPlayer->GetDistanceTo(target);
                                float travelTime = dist / entityComp->BulletFireSpeed;
                                targetAimPos.X += velocity.X * travelTime;
                                targetAimPos.Y += velocity.Y * travelTime;
                                targetAimPos.Z += velocity.Z * travelTime;
                                // Gravity drop compensation
                                float distM = dist / 100.0f;
                                targetAimPos.Z -= distM * 1.0f;
                            }
                        }
                    }
                }
            }

            // Calculate aim rotation
            FVector gunLoc = g_PlayerController->PlayerCameraManager->CameraCache.POV.Location;
            FRotator desired = ToRotator(gunLoc, targetAimPos);
            FRotator current = g_PlayerController->PlayerCameraManager->CameraCache.POV.Rotation;

            float dPitch = desired.Pitch - current.Pitch;
            float dYaw = desired.Yaw - current.Yaw;
            while (dPitch > 180) dPitch -= 360;
            while (dPitch < -180) dPitch += 360;
            while (dYaw > 180) dYaw -= 360;
            while (dYaw < -180) dYaw += 360;

            float s = Config.AimBot.Smooth;
            if (s < 1.0f) s = 1.0f;

            FRotator out;
            out.Pitch = current.Pitch + dPitch / s;
            out.Yaw = current.Yaw + dYaw / s;
            out.Roll = 0;
            clampAngles(out);

            g_PlayerController->ControlRotation = out;
        }
    }

    // --- ESP ---
    for (auto player : PlayerCharacters) {
        if (!player || !Tools::IsPtrValid(player)) continue;
        if (player->PlayerKey == localPlayer->PlayerKey) continue;
        if (player->TeamID == localPlayer->TeamID) continue;
        if (player->bDead) continue;
        if (player->Health <= 0) continue;

        float dist = localPlayer->GetDistanceTo(player) / 100.0f;
        if (dist > Config.MaxDistance) continue;

        FVector headPos = GetBoneLocation(player, "Head");
        FVector rootPos = GetBoneLocation(player, "Root");
        if (headPos.X == 0 && headPos.Y == 0 && headPos.Z == 0) continue;

        FVector2D screenHead, screenFoot;
        bool headVisible = g_PlayerController->ProjectWorldLocationToScreen(headPos, false, &screenHead);
        bool footVisible = g_PlayerController->ProjectWorldLocationToScreen(rootPos, false, &screenFoot);

        if (!headVisible && !footVisible) continue;

        float boxH = screenFoot.Y - screenHead.Y;
        float boxW = boxH * 0.5f;

        // --- Box ESP ---
        if (Config.ESP.Box && boxH > 5) {
            FVector2D tl = {screenHead.X - boxW/2, screenHead.Y};
            FVector2D tr = {screenHead.X + boxW/2, screenHead.Y};
            FVector2D bl = {screenFoot.X - boxW/2, screenFoot.Y};
            FVector2D br = {screenFoot.X + boxW/2, screenFoot.Y};
            Canvas->K2_DrawLine(tl, tr, 1.5f, COLOR_RED);
            Canvas->K2_DrawLine(tr, br, 1.5f, COLOR_RED);
            Canvas->K2_DrawLine(br, bl, 1.5f, COLOR_RED);
            Canvas->K2_DrawLine(bl, tl, 1.5f, COLOR_RED);
        }

        // --- Skeleton ESP ---
        if (Config.ESP.Skeleton) {
            static const char* bonePairs[][2] = {
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

            for (auto &pair : bonePairs) {
                FVector boneA = GetBoneLocation(player, pair[0]);
                FVector boneB = GetBoneLocation(player, pair[1]);
                FVector2D sA, sB;
                if (g_PlayerController->ProjectWorldLocationToScreen(boneA, false, &sA) &&
                    g_PlayerController->ProjectWorldLocationToScreen(boneB, false, &sB)) {
                    Canvas->K2_DrawLine(sA, sB, 1.2f, COLOR_GREEN);
                }
            }
        }

        // --- Health Bar ---
        if (Config.ESP.Health && boxH > 10) {
            float maxHP = player->HealthMax;
            float hp = player->Health;
            float pct = (maxHP > 0) ? (hp / maxHP) : 0;
            if (pct > 1.0f) pct = 1.0f;

            float barX = screenHead.X - boxW/2 - 4;
            float barTop = screenHead.Y;
            float barBot = screenFoot.Y;
            float filledH = (barBot - barTop) * pct;

            FLinearColor hpColor;
            hpColor.R = 1.0f - pct;
            hpColor.G = pct;
            hpColor.B = 0;
            hpColor.A = 1.0f;

            FVector2D hTop = {barX, barBot - filledH};
            FVector2D hBot = {barX, barBot};
            Canvas->K2_DrawLine(hTop, hBot, 3.0f, hpColor);
        }

        // --- Distance ---
        if (Config.ESP.Distance && headVisible) {
            char buf[32];
            snprintf(buf, sizeof(buf), "%.0fm", dist);
            FVector2D textPos = {screenHead.X - 10, screenHead.Y - 14};
            Canvas->K2_DrawText(nullptr, FString(buf), textPos, COLOR_WHITE,
                               0, COLOR_BLACK, FVector2D(), false, false, false, COLOR_BLACK);
        }

        // --- Snap Line ---
        if (Config.ESP.Line && headVisible) {
            FVector2D bottom = {(float)(ScreenWidth / 2), (float)ScreenHeight};
            Canvas->K2_DrawLine(bottom, screenFoot, 1.0f, COLOR_YELLOW);
        }
    }

    // --- FOV Circle ---
    if (Config.AimBot.Enable) {
        float cx = ScreenWidth / 2.0f;
        float cy = ScreenHeight / 2.0f;
        float r = Config.AimBot.FOV;
        int seg = 36;
        for (int i = 0; i < seg; i++) {
            float a1 = (2.0f * 3.14159f * i) / seg;
            float a2 = (2.0f * 3.14159f * (i + 1)) / seg;
            FVector2D p1 = {cx + cosf(a1)*r, cy + sinf(a1)*r};
            FVector2D p2 = {cx + cosf(a2)*r, cy + sinf(a2)*r};
            Canvas->K2_DrawLine(p1, p2, 1.0f, COLOR_WHITE);
        }
    }
}

// ============================================================================
// VTABLE HOOK (simple mprotect + swap — no Dobby needed)
// ============================================================================
template<typename T>
void VTable_Hook(void **vtable, int32_t index, T newFunction, T &oldFunction) {
    if (vtable[index] != (void*)newFunction) {
        oldFunction = reinterpret_cast<T>(vtable[index]);
        uintptr_t page = (uintptr_t)&vtable[index] & ~(sysconf(_SC_PAGESIZE) - 1);
        mprotect((void*)page, sysconf(_SC_PAGESIZE) * 2, PROT_READ | PROT_WRITE);
        vtable[index] = (void*)newFunction;
    }
}

// ============================================================================
// POSTRENDER HOOK
// ============================================================================
void *(*orig_PostRender)(UGameViewportClient *ViewportClient, UCanvas *Canvas) = nullptr;

void *new_PostRender(UGameViewportClient *ViewportClient, UCanvas *Canvas) {
    if (Canvas && Tools::IsPtrValid(Canvas)) {
        RenderESP(Canvas, Canvas->SizeX, Canvas->SizeY);
    }
    return orig_PostRender(ViewportClient, Canvas);
}

int GetPostRenderIndex() {
    return (200 - 66); // = 134
}

void PostrenderDraw() {
    auto GViewport = GetGameViewport();
    if (GViewport) {
        auto VTable = (void**)GViewport->VTable;
        if (VTable && Tools::IsPtrValid(VTable)) {
            int postrender_idx = GetPostRenderIndex();
            using PostRender_t = void*(*)(UGameViewportClient*, UCanvas*);
            VTable_Hook<PostRender_t>(VTable, postrender_idx, new_PostRender, orig_PostRender);
            LOGI("[+] PostRender hooked at vtable[%d]", postrender_idx);
        }
    }
}

// ============================================================================
// BAN FIXER + CRASH FIX
// ============================================================================
void FixGameCrash() {
    system("rm -rf /data/data/com.pubg.imobile/files/il2cpp");
    system("rm -rf /data/data/com.pubg.imobile/files/obblib");
    system("touch /data/data/com.pubg.imobile/files/obblib");
}

void BanFixer() {
    const char* paths[] = {
        "/data/media/0/com.pubg.imobile/hawk_data",
        "/data/media/0/com.pubg.imobile/login-identifier.txt",
        "/storage/emulated/0/com.pubg.imobile/hawk_data",
        "/storage/emulated/0/com.pubg.imobile/login-identifier.txt",
    };
    for (auto &p : paths) {
        remove(p);
    }
}

// ============================================================================
// MAIN THREAD (exact reference pattern)
// ============================================================================
void *Chameli(void *) {
    LOGI("[*] ====================================");
    LOGI("[*] BGMI 4.4.0 ESP + Aimbot + Bypass");
    LOGI("[*] ====================================");

    // Step 1: Clean files
    BanFixer();
    FixGameCrash();

    // Step 2: Wait for libUE4.so
    while (!UE4) {
        UE4 = Tools::GetBaseAddress("libUE4.so");
        sleep(1);
    }
    LOGI("[+] libUE4.so base = 0x%lx", (unsigned long)UE4);

    // Step 3: Apply all bypass patches
    ApplyAllBypasses();
    LOGI("[+] Applied bypass patches");

    // Step 4: Wait for native app
    while (!g_App) {
        g_App = *(android_app**)(UE4 + GNativeAndroidApp_Offset);
        sleep(1);
    }
    LOGI("[+] g_App = %p", g_App);

    // Step 5: Initialize GNames
    FName::GNames = GetGNames();
    while (!FName::GNames) {
        FName::GNames = GetGNames();
        sleep(1);
    }
    LOGI("[+] GNames = %p", FName::GNames);

    // Step 6: Initialize GUObjectArray
    UObject::GUObjectArray = (FUObjectArray*)(UE4 + GUObject_Offset);
    LOGI("[+] GUObjectArray = %p", UObject::GUObjectArray);

    // Step 7: Hook PostRender (this blocks until viewport found)
    PostrenderDraw();

    LOGI("[+] Initialization complete — ESP active");
    return nullptr;
}

// ============================================================================
// ENTRY POINT
// ============================================================================
__attribute__((constructor))
void _init() {
    pthread_t Ptid;
    pthread_create(&Ptid, 0, Chameli, 0);
}
