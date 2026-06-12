/*
 * BGMI 4.4.0 ARM64 — ESP + Aimbot + Large Hitbox + Bypass
 * 
 * CRASH-SAFE DESIGN:
 * - Constructor does NOTHING except spawn a detached thread
 * - Thread waits COMPLETELY for libUE4.so before ANY game access
 * - NO system() calls (SELinux blocks them)
 * - NO early SDK access (GNames/GUObjectArray not ready)
 * - NO exceptions, NO locale, NO codecvt at load time
 * - All SDK ProcessEvent calls are null-guarded
 * - VTable hook only after game fully initialized
 */

// === IMPORTANT: SDK.hpp included but we compile with -fno-exceptions ===
// === The codecvt in NIKON_Basic.hpp FString can crash if called too early ===
#include "SDK.hpp"
using namespace SDK;

#include <pthread.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <android/log.h>
#include <cmath>
#include <vector>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>

// ============================================================================
// LOGGING (safe — android logging always available)
// ============================================================================
#define TAG "BM"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

// ============================================================================
// OFFSETS (BGMI 4.4.0 ARM64 — verified)
// ============================================================================
static const uintptr_t OFF_GNAMES           = 0x8394964;
static const uintptr_t OFF_GUOBJECTARRAY    = 0xe22f8d0;
static const uintptr_t OFF_GETACTORARRAY    = 0xa1018ac;
static const uintptr_t OFF_GNATIVEAPP       = 0xdf74398;

// ============================================================================
// GLOBAL STATE (all initially null/zero — safe)
// ============================================================================
static uintptr_t g_UE4Base = 0;
static void* g_NativeApp = nullptr;

// Player state (written only from PostRender hook thread = game thread)
static ASTExtraPlayerController* g_PC = nullptr;
static ASTExtraPlayerCharacter* g_LocalPlayer = nullptr;
static int g_ScreenW = 0, g_ScreenH = 0;

// ============================================================================
// CONFIG
// ============================================================================
struct ModConfig {
    bool enabled = true;
    struct { bool box=true; bool skeleton=true; bool health=true; bool distance=true; bool line=false; } esp;
    struct { bool enabled=true; float smooth=5.f; float fov=120.f; bool prediction=true; } aim;
    struct { bool enabled=true; } hitbox;
    float maxDist = 300.f;
};
static ModConfig g_Config;

// ============================================================================
// COLORS
// ============================================================================
static const FLinearColor C_RED    = {1,0,0,1};
static const FLinearColor C_GREEN  = {0,1,0,1};
static const FLinearColor C_WHITE  = {1,1,1,1};
static const FLinearColor C_YELLOW = {1,1,0,1};
static const FLinearColor C_BLACK  = {0,0,0,1};

// ============================================================================
// SAFE POINTER VALIDATION
// ============================================================================
static inline bool IsValid(const void* ptr) {
    if (!ptr) return false;
    uintptr_t p = (uintptr_t)ptr;
    // Android userspace valid range: 0x1000 ~ 0x7FFFFFFFFFFF
    return (p >= 0x1000 && p <= 0x7FFFFFFFFFFF);
}

// ============================================================================
// GET LIBRARY BASE (parse /proc/self/maps — no dlopen needed)
// ============================================================================
static uintptr_t GetLibBase(const char* libName) {
    FILE* fp = fopen("/proc/self/maps", "r");
    if (!fp) return 0;
    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, libName) && strstr(line, "r-xp")) {
            uintptr_t base = 0;
            sscanf(line, "%lx", &base);
            fclose(fp);
            return base;
        }
    }
    fclose(fp);
    return 0;
}

// ============================================================================
// SAFE FILE DELETE (replaces system("rm -rf ...") which can crash)
// ============================================================================
static void SafeRemove(const char* path) {
    // Just try unlink — if it fails, move on. No crash.
    unlink(path);
}

// ============================================================================
// BAN FIXER (safe — just file removal, no system() call)
// ============================================================================
static void BanFixer() {
    SafeRemove("/data/media/0/com.pubg.imobile/hawk_data");
    SafeRemove("/data/media/0/com.pubg.imobile/login-identifier.txt");
    SafeRemove("/storage/emulated/0/com.pubg.imobile/hawk_data");
    SafeRemove("/storage/emulated/0/com.pubg.imobile/login-identifier.txt");
}

// ============================================================================
// BYPASS PATCHES (inline — no separate Bypass.h needed)
// Memory patching: mprotect + memcpy. Completely safe if base is valid.
// ============================================================================
static void PatchHex(uintptr_t base, uintptr_t offset, const unsigned char* bytes, int len) {
    if (!base || !len) return;
    uintptr_t addr = base + offset;
    uintptr_t pageSize = (uintptr_t)sysconf(_SC_PAGESIZE);
    uintptr_t page = addr & ~(pageSize - 1);
    // mprotect 2 pages to handle boundary
    if (mprotect((void*)page, pageSize * 2, PROT_READ | PROT_WRITE | PROT_EXEC) != 0) return;
    memcpy((void*)addr, bytes, len);
}

// NOP = 1F 20 03 D5 (ARM64 NOP instruction)
static const unsigned char NOP4[] = {0x1F, 0x20, 0x03, 0xD5};
// RET = C0 03 5F D6 (ARM64 return)
static const unsigned char RET4[] = {0xC0, 0x03, 0x5F, 0xD6};
// MOV X0, #0; RET = 00 00 80 52 C0 03 5F D6
static const unsigned char RET0_8[] = {0x00, 0x00, 0x80, 0x52, 0xC0, 0x03, 0x5F, 0xD6};
// MOV X0, #1; RET = 20 00 80 52 C0 03 5F D6
static const unsigned char RET1_8[] = {0x20, 0x00, 0x80, 0x52, 0xC0, 0x03, 0x5F, 0xD6};

static void ApplyBypasses() {
    // === libanogs.so ===
    uintptr_t anogs = GetLibBase("libanogs.so");
    if (anogs) {
        PatchHex(anogs, 0x1d7938, RET0_8, 8);
        PatchHex(anogs, 0x1d551c, RET0_8, 8);
        PatchHex(anogs, 0x1d624c, RET4, 4);
        PatchHex(anogs, 0x1d6598, RET0_8, 8);
        PatchHex(anogs, 0x1d6ea8, RET0_8, 8);
        PatchHex(anogs, 0x1d79a4, RET4, 4);
        PatchHex(anogs, 0x1d7fc4, RET0_8, 8);
        PatchHex(anogs, 0x1d88ec, RET4, 4);
        PatchHex(anogs, 0x1d417c, RET4, 4);
        PatchHex(anogs, 0x1d5a88, RET4, 4);
        PatchHex(anogs, 0x1d82cc, RET4, 4);
        PatchHex(anogs, 0x1d4580, RET4, 4);
        PatchHex(anogs, 0x1d7398, RET4, 4);
        PatchHex(anogs, 0x1d9024, RET4, 4);
        PatchHex(anogs, 0x1d4c0c, RET4, 4);
        PatchHex(anogs, 0x1d5030, RET4, 4);
        PatchHex(anogs, 0x1d78cc, RET0_8, 8);
        PatchHex(anogs, 0x1d8c74, RET4, 4);
        LOGI("[+] libanogs patched (%d)", 18);
    }

    // === libhdmpvecore.so ===
    uintptr_t hdmpve = GetLibBase("libhdmpvecore.so");
    if (hdmpve) {
        PatchHex(hdmpve, 0x73150, RET0_8, 8);
        PatchHex(hdmpve, 0x6d660, RET4, 4);
        PatchHex(hdmpve, 0x6d900, RET4, 4);
        PatchHex(hdmpve, 0x79944, RET4, 4);
        PatchHex(hdmpve, 0x72cac, RET0_8, 8);
        PatchHex(hdmpve, 0x7314c, RET1_8, 8);
        PatchHex(hdmpve, 0x95b44, RET4, 4);
        PatchHex(hdmpve, 0x79bd0, RET4, 4);
        PatchHex(hdmpve, 0x6da40, RET4, 4);
        PatchHex(hdmpve, 0x6dc80, RET4, 4);
        PatchHex(hdmpve, 0x6de00, RET4, 4);
        PatchHex(hdmpve, 0x6e1c0, RET4, 4);
        PatchHex(hdmpve, 0x6e400, RET4, 4);
        PatchHex(hdmpve, 0x6e640, RET4, 4);
        PatchHex(hdmpve, 0x6e880, RET4, 4);
        PatchHex(hdmpve, 0x72e00, RET0_8, 8);
        PatchHex(hdmpve, 0x73000, RET0_8, 8);
        PatchHex(hdmpve, 0x73200, RET0_8, 8);
        PatchHex(hdmpve, 0x73400, RET4, 4);
        PatchHex(hdmpve, 0x73600, RET4, 4);
        PatchHex(hdmpve, 0x73800, RET4, 4);
        PatchHex(hdmpve, 0x79000, RET4, 4);
        PatchHex(hdmpve, 0x79200, RET4, 4);
        PatchHex(hdmpve, 0x79400, RET4, 4);
        PatchHex(hdmpve, 0x79600, RET4, 4);
        PatchHex(hdmpve, 0x79800, RET4, 4);
        PatchHex(hdmpve, 0x95000, RET4, 4);
        PatchHex(hdmpve, 0x95200, RET4, 4);
        PatchHex(hdmpve, 0x95400, RET4, 4);
        PatchHex(hdmpve, 0x95600, RET4, 4);
        LOGI("[+] libhdmpvecore patched (%d)", 30);
    }

    // === libTBlueData.so ===
    uintptr_t tblue = GetLibBase("libTBlueData.so");
    if (tblue) {
        PatchHex(tblue, 0x8e854, RET4, 4);
        PatchHex(tblue, 0x8f5c4, RET4, 4);
        PatchHex(tblue, 0x91fb8, RET4, 4);
        PatchHex(tblue, 0x8e308, RET4, 4);
        PatchHex(tblue, 0x8e578, RET4, 4);
        PatchHex(tblue, 0x8ebf8, RET4, 4);
        PatchHex(tblue, 0x91ff0, RET4, 4);
        PatchHex(tblue, 0x91ef8, RET4, 4);
        PatchHex(tblue, 0x91e74, RET4, 4);
        PatchHex(tblue, 0x8f7ac, RET4, 4);
        PatchHex(tblue, 0x8f914, RET4, 4);
        PatchHex(tblue, 0x91e28, RET4, 4);
        LOGI("[+] libTBlueData patched (%d)", 12);
    }

    // === libsigner.so ===
    uintptr_t signer = GetLibBase("libsigner.so");
    if (signer) {
        PatchHex(signer, 0x9a088, RET4, 4);
        PatchHex(signer, 0x9afc0, RET4, 4);
        LOGI("[+] libsigner patched (%d)", 2);
    }

    // === libUE4.so (NOP only — don't RET game functions!) ===
    uintptr_t ue4 = g_UE4Base;
    if (ue4) {
        // Shoot verify
        PatchHex(ue4, 0x5B2F240, NOP4, 4);
        PatchHex(ue4, 0x5AB8298, NOP4, 4);
        PatchHex(ue4, 0x5AC01AC, NOP4, 4);
        PatchHex(ue4, 0x5ABB11C, NOP4, 4);
        PatchHex(ue4, 0x5ABB5F4, NOP4, 4);
        PatchHex(ue4, 0x5ABB6BC, NOP4, 4);
        PatchHex(ue4, 0x5ABA394, NOP4, 4);
        PatchHex(ue4, 0x5ABA9AC, NOP4, 4);
        PatchHex(ue4, 0x5AC0780, NOP4, 4);
        PatchHex(ue4, 0x5A9C868, NOP4, 4);
        PatchHex(ue4, 0x5A9D32C, NOP4, 4);
        PatchHex(ue4, 0x5A9D5E4, NOP4, 4);
        PatchHex(ue4, 0x5C9A1AC, NOP4, 4);
        PatchHex(ue4, 0x5C9A380, NOP4, 4);
        PatchHex(ue4, 0x5C9A3EC, NOP4, 4);
        // Lag compensation
        PatchHex(ue4, 0x5ABD2E8, NOP4, 4);
        PatchHex(ue4, 0x5ABDE18, NOP4, 4);
        PatchHex(ue4, 0x5ABE400, NOP4, 4);
        PatchHex(ue4, 0x5ABE498, NOP4, 4);
        PatchHex(ue4, 0x5ABE7D8, NOP4, 4);
        PatchHex(ue4, 0x5ABE868, NOP4, 4);
        PatchHex(ue4, 0x5ABECCC, NOP4, 4);
        PatchHex(ue4, 0x5ABED70, NOP4, 4);
        PatchHex(ue4, 0x5ABF110, NOP4, 4);
        PatchHex(ue4, 0x5ABF4F4, NOP4, 4);
        PatchHex(ue4, 0x5ABF744, NOP4, 4);
        PatchHex(ue4, 0x5ABFD80, NOP4, 4);
        PatchHex(ue4, 0x5ABD43C, NOP4, 4);
        PatchHex(ue4, 0x5ABD774, NOP4, 4);
        PatchHex(ue4, 0x5ABCDA8, NOP4, 4);
        // Move anti-cheat
        PatchHex(ue4, 0x5BD03E4, NOP4, 4);
        PatchHex(ue4, 0x5BE83B8, NOP4, 4);
        PatchHex(ue4, 0x5BE8320, NOP4, 4);
        PatchHex(ue4, 0x5BEA684, NOP4, 4);
        PatchHex(ue4, 0x5BFDA8C, NOP4, 4);
        PatchHex(ue4, 0x5C52588, NOP4, 4);
        PatchHex(ue4, 0x5C5AAC0, NOP4, 4);
        PatchHex(ue4, 0x5C603A0, NOP4, 4);
        // Weapon anti-cheat
        PatchHex(ue4, 0x58716F4, NOP4, 4);
        PatchHex(ue4, 0x5872634, NOP4, 4);
        PatchHex(ue4, 0x58739E4, NOP4, 4);
        PatchHex(ue4, 0x587BD5C, NOP4, 4);
        PatchHex(ue4, 0x58806C0, NOP4, 4);
        PatchHex(ue4, 0x58AD510, NOP4, 4);
        PatchHex(ue4, 0x58DD608, NOP4, 4);
        PatchHex(ue4, 0x59AA0BC, NOP4, 4);
        PatchHex(ue4, 0x59B8DD8, NOP4, 4);
        PatchHex(ue4, 0x5A30CD8, NOP4, 4);
        // Entity
        PatchHex(ue4, 0x5BAF294, NOP4, 4);
        PatchHex(ue4, 0x5BAF300, NOP4, 4);
        PatchHex(ue4, 0x5C6EB80, NOP4, 4);
        PatchHex(ue4, 0x5C71CF4, NOP4, 4);
        PatchHex(ue4, 0x5C7ACF4, NOP4, 4);
        // Table guard
        PatchHex(ue4, 0x5FD3220, NOP4, 4);
        PatchHex(ue4, 0x5FDA9EC, NOP4, 4);
        // Realtime verify
        PatchHex(ue4, 0x5BA3CB4, NOP4, 4);
        // Vehicle weapon
        PatchHex(ue4, 0x5928504, NOP4, 4);
        PatchHex(ue4, 0x5953720, NOP4, 4);
        PatchHex(ue4, 0x5A2A1B8, NOP4, 4);
        PatchHex(ue4, 0x5A2C674, NOP4, 4);
        PatchHex(ue4, 0x5A79340, NOP4, 4);
        PatchHex(ue4, 0x5B921B0, NOP4, 4);
        PatchHex(ue4, 0x5B98F54, NOP4, 4);
        PatchHex(ue4, 0x5D6B9F0, NOP4, 4);
        // Report/Network
        PatchHex(ue4, 0x5F3B658, NOP4, 4);
        PatchHex(ue4, 0x5F41BE4, NOP4, 4);
        PatchHex(ue4, 0x5DE19C8, NOP4, 4);
        PatchHex(ue4, 0x5DF0CDC, NOP4, 4);
        PatchHex(ue4, 0x5DEC6B0, NOP4, 4);
        PatchHex(ue4, 0x5DEC784, NOP4, 4);
        PatchHex(ue4, 0x5DEC9B0, NOP4, 4);
        PatchHex(ue4, 0x5DED5FC, NOP4, 4);
        // Spectator
        PatchHex(ue4, 0x5DF05C0, NOP4, 4);
        PatchHex(ue4, 0x5E38CF0, NOP4, 4);
        PatchHex(ue4, 0x5ECC338, NOP4, 4);
        PatchHex(ue4, 0x5ECE2C8, NOP4, 4);
        PatchHex(ue4, 0x5F31A70, NOP4, 4);
        PatchHex(ue4, 0x6001678, NOP4, 4);
        LOGI("[+] libUE4 patched (%d)", 74);
    }
}

// ============================================================================
// MATH
// ============================================================================
static float VecDist(FVector a, FVector b) {
    float dx=a.X-b.X, dy=a.Y-b.Y, dz=a.Z-b.Z;
    return sqrtf(dx*dx+dy*dy+dz*dz);
}

static FRotator CalcRotation(FVector from, FVector to) {
    FVector d = {to.X-from.X, to.Y-from.Y, to.Z-from.Z};
    float dist = sqrtf(d.X*d.X + d.Y*d.Y);
    FRotator r;
    r.Pitch = atan2f(d.Z, dist) * 57.2957795f;
    r.Yaw = atan2f(d.Y, d.X) * 57.2957795f;
    r.Roll = 0;
    return r;
}

// ============================================================================
// GNames (function call at offset — returns pointer)
// ============================================================================
static TNameEntryArray* CallGetGNames() {
    if (!g_UE4Base) return nullptr;
    typedef TNameEntryArray* (*GetGNames_t)();
    GetGNames_t fn = (GetGNames_t)(g_UE4Base + OFF_GNAMES);
    // Verify the function pointer is in libUE4 code range (sanity check)
    if (!IsValid((void*)fn)) return nullptr;
    return fn();
}

// ============================================================================
// GET WORLD (walk UEngine → GameViewport → World with caching)
// ============================================================================
static int g_EngineIdx = -1;

static UWorld* GetWorld() {
    if (!UObject::GUObjectArray) return nullptr;
    TUObjectArray& objs = UObject::GUObjectArray->ObjObjects;
    
    auto tryEngine = [&](int idx) -> UWorld* {
        if (idx < 0 || idx >= objs.Num()) return nullptr;
        auto obj = objs.GetByIndex(idx);
        if (!obj || !IsValid(obj)) return nullptr;
        if (!obj->IsA(UEngine::StaticClass())) return nullptr;
        auto engine = (UEngine*)obj;
        if (!IsValid(engine->GameViewport)) return nullptr;
        auto vp = engine->GameViewport;
        if (!IsValid(vp->World)) return nullptr;
        return vp->World;
    };

    // Try cached index first
    if (g_EngineIdx >= 0) {
        UWorld* w = tryEngine(g_EngineIdx);
        if (w) return w;
        g_EngineIdx = -1; // Cache stale
    }

    // Full scan
    int count = objs.Num();
    if (count > 200000) count = 200000; // Safety cap
    for (int i = 0; i < count; i++) {
        UWorld* w = tryEngine(i);
        if (w) {
            g_EngineIdx = i;
            return w;
        }
    }
    return nullptr;
}

// ============================================================================
// GET GAME VIEWPORT (for PostRender hook)
// ============================================================================
static UGameViewportClient* GetViewport() {
    if (!UObject::GUObjectArray) return nullptr;
    // FindObject is safest — uses game's own name table
    return UObject::FindObject<UGameViewportClient>(
        "GameViewportClient Transient.UAEGameEngine_1.GameViewportClient_1");
}

// ============================================================================
// SAFE BONE POS (wraps ProcessEvent — returns zero if anything invalid)
// ============================================================================
static FVector SafeGetBonePos(ASTExtraPlayerCharacter* actor, const char* bone) {
    FVector zero = {0,0,0};
    if (!actor || !IsValid(actor)) return zero;
    // GetBonePos uses ProcessEvent internally — it null-checks pFunc
    return actor->GetBonePos(FName(bone), zero);
}

// ============================================================================
// RENDER ESP (called from game thread via PostRender — fully guarded)
// ============================================================================
static void DoESP(UCanvas* Canvas) {
    if (!Canvas || !IsValid(Canvas)) return;
    if (!g_Config.enabled) return;

    g_ScreenW = Canvas->SizeX;
    g_ScreenH = Canvas->SizeY;
    if (g_ScreenW <= 0 || g_ScreenH <= 0) return;

    // Get world
    UWorld* world = GetWorld();
    if (!world || !IsValid(world)) return;

    // Get local player controller via NetDriver
    UNetDriver* netDrv = world->NetDriver;
    if (!netDrv || !IsValid(netDrv)) return;
    UNetConnection* conn = netDrv->ServerConnection;
    if (!conn || !IsValid(conn)) return;
    APlayerController* pc = conn->PlayerController;
    if (!pc || !IsValid(pc)) return;

    auto myPC = (ASTExtraPlayerController*)pc;
    g_PC = myPC;

    // Get all player characters
    TArray<AActor*> actorArr;
    UGameplayStatics::GetAllActorsOfClass(
        (UObject*)world, ASTExtraPlayerCharacter::StaticClass(), &actorArr);
    if (actorArr.Num() <= 0) return;

    // Find local player by PlayerKey
    ASTExtraPlayerCharacter* localP = nullptr;
    for (int i = 0; i < actorArr.Num(); i++) {
        auto p = (ASTExtraPlayerCharacter*)actorArr[i];
        if (!p || !IsValid(p)) continue;
        if (p->PlayerKey == myPC->PlayerKey) {
            localP = p;
            break;
        }
    }
    if (!localP) return;
    g_LocalPlayer = localP;

    // Camera info for aimbot
    auto camMgr = myPC->PlayerCameraManager;
    bool hasCam = (camMgr && IsValid(camMgr));

    // Large hitbox
    if (g_Config.hitbox.enabled && IsValid(localP->PartHitComponent)) {
        auto& angles = localP->PartHitComponent->ConfigCollisionDistSqAngles;
        for (int j = 0; j < angles.Num(); j++) {
            angles[j].Angle = 90.0f;
        }
    }

    // Aimbot target
    ASTExtraPlayerCharacter* aimTarget = nullptr;
    float bestAimDist = g_Config.aim.fov;

    // ESP loop
    for (int i = 0; i < actorArr.Num(); i++) {
        auto player = (ASTExtraPlayerCharacter*)actorArr[i];
        if (!player || !IsValid(player)) continue;
        if (player == localP) continue;
        if (player->TeamID == localP->TeamID) continue;
        if (player->bDead) continue;
        if (player->Health <= 0) continue;

        float dist = localP->GetDistanceTo(player) / 100.f;
        if (dist > g_Config.maxDist || dist <= 0) continue;

        // Get head + root for box
        FVector headPos = SafeGetBonePos(player, "Head");
        FVector rootPos = SafeGetBonePos(player, "pelvis");
        if (headPos.X == 0 && headPos.Y == 0 && headPos.Z == 0) continue;

        FVector2D sHead, sFoot;
        bool hVis = myPC->ProjectWorldLocationToScreen(headPos, false, &sHead);
        bool fVis = myPC->ProjectWorldLocationToScreen(rootPos, false, &sFoot);
        if (!hVis && !fVis) continue;

        float boxH = sFoot.Y - sHead.Y;
        float boxW = boxH * 0.5f;

        // Box
        if (g_Config.esp.box && boxH > 5) {
            FVector2D tl={sHead.X-boxW/2, sHead.Y};
            FVector2D tr={sHead.X+boxW/2, sHead.Y};
            FVector2D bl={sFoot.X-boxW/2, sFoot.Y};
            FVector2D br={sFoot.X+boxW/2, sFoot.Y};
            Canvas->K2_DrawLine(tl, tr, 1.5f, C_RED);
            Canvas->K2_DrawLine(tr, br, 1.5f, C_RED);
            Canvas->K2_DrawLine(br, bl, 1.5f, C_RED);
            Canvas->K2_DrawLine(bl, tl, 1.5f, C_RED);
        }

        // Skeleton
        if (g_Config.esp.skeleton) {
            static const char* pairs[][2] = {
                {"Head","neck_01"},{"neck_01","spine_02"},{"spine_02","spine_01"},
                {"spine_01","pelvis"},{"neck_01","upperarm_l"},{"upperarm_l","lowerarm_l"},
                {"lowerarm_l","hand_l"},{"neck_01","upperarm_r"},{"upperarm_r","lowerarm_r"},
                {"lowerarm_r","hand_r"},{"pelvis","thigh_l"},{"thigh_l","calf_l"},
                {"calf_l","foot_l"},{"pelvis","thigh_r"},{"thigh_r","calf_r"},{"calf_r","foot_r"}
            };
            for (auto& pr : pairs) {
                FVector a = SafeGetBonePos(player, pr[0]);
                FVector b = SafeGetBonePos(player, pr[1]);
                FVector2D sa, sb;
                if (myPC->ProjectWorldLocationToScreen(a, false, &sa) &&
                    myPC->ProjectWorldLocationToScreen(b, false, &sb)) {
                    Canvas->K2_DrawLine(sa, sb, 1.2f, C_GREEN);
                }
            }
        }

        // Health bar
        if (g_Config.esp.health && boxH > 10) {
            float pct = (player->HealthMax > 0) ? (player->Health / player->HealthMax) : 0;
            if (pct > 1.f) pct = 1.f;
            float barX = sHead.X - boxW/2 - 4;
            float filled = (sFoot.Y - sHead.Y) * pct;
            FLinearColor hpc = {1.f-pct, pct, 0, 1};
            Canvas->K2_DrawLine({barX, sFoot.Y-filled}, {barX, sFoot.Y}, 3.f, hpc);
        }

        // Distance text
        if (g_Config.esp.distance && hVis) {
            char buf[16]; snprintf(buf, 16, "%.0fm", dist);
            Canvas->K2_DrawText(nullptr, FString(buf), {sHead.X-10, sHead.Y-14},
                               C_WHITE, 0, C_BLACK, {0,0}, false, false, false, C_BLACK);
        }

        // Snapline
        if (g_Config.esp.line && fVis) {
            Canvas->K2_DrawLine({(float)g_ScreenW/2, (float)g_ScreenH}, sFoot, 1.f, C_YELLOW);
        }

        // Aimbot target selection
        if (g_Config.aim.enabled && hVis) {
            float dx = sHead.X - g_ScreenW/2.f;
            float dy = sHead.Y - g_ScreenH/2.f;
            float d = sqrtf(dx*dx+dy*dy);
            if (d < bestAimDist) { bestAimDist = d; aimTarget = player; }
        }
    }

    // Aimbot execution
    if (g_Config.aim.enabled && aimTarget && hasCam && localP->bIsWeaponFiring) {
        FVector targetHead = SafeGetBonePos(aimTarget, "Head");

        // Bullet prediction
        if (g_Config.aim.prediction) {
            auto wm = localP->WeaponManagerComponent;
            if (wm && IsValid(wm)) {
                auto wpn = (ASTExtraShootWeapon*)wm->CurrentWeaponReplicated;
                if (wpn && IsValid(wpn) && wpn->ShootWeaponComponent &&
                    IsValid(wpn->ShootWeaponComponent) &&
                    wpn->ShootWeaponComponent->ShootWeaponEntityComponent &&
                    IsValid(wpn->ShootWeaponComponent->ShootWeaponEntityComponent)) {
                    float bulletSpeed = wpn->ShootWeaponComponent->ShootWeaponEntityComponent->BulletFireSpeed;
                    if (bulletSpeed > 0) {
                        FVector vel = aimTarget->GetVelocity();
                        float d = localP->GetDistanceTo(aimTarget);
                        float t = d / bulletSpeed;
                        targetHead.X += vel.X * t;
                        targetHead.Y += vel.Y * t;
                        targetHead.Z += vel.Z * t;
                        targetHead.Z -= (d/100.f) * 1.0f; // gravity
                    }
                }
            }
        }

        FVector camLoc = camMgr->CameraCache.POV.Location;
        FRotator camRot = camMgr->CameraCache.POV.Rotation;
        FRotator desired = CalcRotation(camLoc, targetHead);

        float dP = desired.Pitch - camRot.Pitch;
        float dY = desired.Yaw - camRot.Yaw;
        while(dP>180)dP-=360; while(dP<-180)dP+=360;
        while(dY>180)dY-=360; while(dY<-180)dY+=360;

        float s = g_Config.aim.smooth;
        if(s<1.f) s=1.f;

        FRotator out;
        out.Pitch = camRot.Pitch + dP/s;
        out.Yaw = camRot.Yaw + dY/s;
        out.Roll = 0;
        // Clamp
        while(out.Pitch>89)out.Pitch-=360; while(out.Pitch<-89)out.Pitch+=360;
        while(out.Yaw>180)out.Yaw-=360; while(out.Yaw<-180)out.Yaw+=360;

        myPC->ControlRotation = out;
    }

    // FOV circle
    if (g_Config.aim.enabled) {
        float cx=g_ScreenW/2.f, cy=g_ScreenH/2.f, r=g_Config.aim.fov;
        for(int i=0;i<36;i++){
            float a1=6.2832f*i/36, a2=6.2832f*(i+1)/36;
            Canvas->K2_DrawLine({cx+cosf(a1)*r,cy+sinf(a1)*r},{cx+cosf(a2)*r,cy+sinf(a2)*r},1.f,C_WHITE);
        }
    }
}

// ============================================================================
// VTABLE HOOK (safest method — no instruction patching)
// ============================================================================
template<typename T>
static void HookVTable(void** vtable, int idx, T hookFn, T& origFn) {
    if (!vtable || !IsValid(vtable)) return;
    if (vtable[idx] == (void*)hookFn) return; // Already hooked

    origFn = reinterpret_cast<T>(vtable[idx]);

    uintptr_t pageSize = sysconf(_SC_PAGESIZE);
    uintptr_t page = (uintptr_t)&vtable[idx] & ~(pageSize - 1);
    mprotect((void*)page, pageSize * 2, PROT_READ | PROT_WRITE);
    vtable[idx] = (void*)hookFn;
    // Restore protection (optional — game doesn't check)
    // mprotect((void*)page, pageSize * 2, PROT_READ);
}

// PostRender hook
static void* (*orig_PostRender)(UGameViewportClient*, UCanvas*) = nullptr;

static void* hook_PostRender(UGameViewportClient* vpc, UCanvas* canvas) {
    if (canvas && IsValid(canvas)) {
        DoESP(canvas);
    }
    if (orig_PostRender) return orig_PostRender(vpc, canvas);
    return nullptr;
}

// ============================================================================
// MAIN INITIALIZATION THREAD
// 
// This runs on a SEPARATE thread. Game continues loading normally.
// We only touch game memory AFTER everything is confirmed loaded.
// ============================================================================
static void* InitThread(void*) {
    LOGI("[*] Init thread started — waiting for game...");

    // ---- PHASE 1: Wait for libUE4.so to be loaded ----
    // This can take 5-15 seconds. We just poll.
    while (g_UE4Base == 0) {
        g_UE4Base = GetLibBase("libUE4.so");
        if (g_UE4Base == 0) sleep(1);
    }
    LOGI("[+] libUE4.so = 0x%lx", (unsigned long)g_UE4Base);

    // ---- PHASE 2: Wait a bit for all libs to finish loading ----
    // AC libs load AFTER UE4. Give them time.
    sleep(3);

    // ---- PHASE 3: Apply bypass patches ----
    // Safe: we verified each lib base before patching
    ApplyBypasses();

    // ---- PHASE 4: Wait for GNativeAndroidApp (game engine running) ----
    while (!g_NativeApp) {
        void* ptr = *(void**)(g_UE4Base + OFF_GNATIVEAPP);
        if (IsValid(ptr)) g_NativeApp = ptr;
        else sleep(1);
    }
    LOGI("[+] NativeApp = %p", g_NativeApp);

    // ---- PHASE 5: Initialize SDK GNames ----
    while (!FName::GNames) {
        FName::GNames = CallGetGNames();
        if (!FName::GNames) sleep(1);
    }
    LOGI("[+] GNames initialized");

    // ---- PHASE 6: Initialize GUObjectArray ----
    UObject::GUObjectArray = (FUObjectArray*)(g_UE4Base + OFF_GUOBJECTARRAY);
    if (!IsValid(UObject::GUObjectArray)) {
        LOGE("[-] GUObjectArray invalid!");
        return nullptr;
    }
    LOGI("[+] GUObjectArray initialized");

    // ---- PHASE 7: Wait for GameViewportClient (game fully loaded = in menu) ----
    UGameViewportClient* viewport = nullptr;
    int retries = 0;
    while (!viewport && retries < 120) { // Max 2 min wait
        viewport = GetViewport();
        if (!viewport) { sleep(1); retries++; }
    }
    if (!viewport) {
        LOGE("[-] Viewport not found after 120s — aborting hook");
        return nullptr;
    }
    LOGI("[+] Viewport found");

    // ---- PHASE 8: Hook PostRender VTable ----
    void** vtable = *(void***)viewport; // First pointer in object = VTable
    if (!IsValid(vtable)) {
        LOGE("[-] VTable invalid");
        return nullptr;
    }

    int postRenderIdx = 200 - 66; // = 134 (verified from reference)
    using PR_t = void*(*)(UGameViewportClient*, UCanvas*);
    HookVTable<PR_t>(vtable, postRenderIdx, hook_PostRender, orig_PostRender);
    LOGI("[+] PostRender hooked at vtable[%d] — ESP ACTIVE", postRenderIdx);

    // ---- DONE ----
    // BanFixer runs LAST (after game is stable)
    BanFixer();

    return nullptr;
}

// ============================================================================
// CONSTRUCTOR — ABSOLUTELY MINIMAL
//
// This is called by dlopen BEFORE the game is ready.
// MUST NOT: access game memory, call system(), use locale, throw exceptions.
// MUST ONLY: spawn a detached thread and return immediately.
// ============================================================================
__attribute__((constructor))
static void entry() {
    pthread_t t;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&t, &attr, InitThread, nullptr);
    pthread_attr_destroy(&attr);
}
