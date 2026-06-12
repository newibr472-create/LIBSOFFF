// BGMI 4.4.0 ARM64 — v2.2 (Crash-safe: No direct function calls)
// ESP drawing DISABLED until stable. Only bypass + aimbot (memory writes only)
#include <pthread.h>
#include <unistd.h>
#include <android/log.h>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include "core/memory.h"
#include "core/bypass.h"
#include "sdk/types.h"

#define TAG "M"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

static uintptr_t g_UE4 = 0;
static bool g_Active = false;

// Aimbot config
static struct {
    bool enabled = true;
    float fov = 100.0f;
    float smooth = 6.0f;
    float jitter = 0.3f;
} AimCfg;

// Simple PRNG for jitter
static uint32_t g_Seed = 0xDEAD;
static float RandF(float mn, float mx) {
    g_Seed ^= g_Seed << 13; g_Seed ^= g_Seed >> 17; g_Seed ^= g_Seed << 5;
    return mn + ((float)(g_Seed & 0xFFFF) / 65535.0f) * (mx - mn);
}

static float NormAngle(float a) {
    while (a > 180.f) a -= 360.f;
    while (a < -180.f) a += 360.f;
    return a;
}

// Bone world position
static FVector GetBoneWorld(uintptr_t mesh, int idx) {
    FVector z{};
    if (!mesh || !mem::IsValid((void*)mesh)) return z;
    TArray<FTransform> bones;
    if (!mem::Read((void*)(mesh + Off::BoneArray), &bones, sizeof(bones))) return z;
    if (!bones.IsValid() || idx >= bones.Count || idx < 0) return z;
    if (!mem::IsValid(bones.Data)) return z;
    FTransform t;
    if (!mem::Read(&bones.Data[idx], &t, sizeof(t))) return z;
    FTransform c2w;
    if (!mem::Read((void*)(mesh + Off::ComponentToWorld), &c2w, sizeof(c2w))) return z;
    FVector r = c2w.GetLocation() + t.GetLocation();
    if (fabsf(r.X) > 500000 || fabsf(r.Y) > 500000) return z;
    return r;
}

// Aimbot thread — runs independently, reads game state, writes ControlRotation
static void *aim_thread(void *) {
    LOGI("[*] Aim thread waiting for activation");
    while (!g_Active) sleep(1);
    LOGI("[*] Aim thread active");
    
    while (true) {
        usleep(16000); // ~60hz
        if (!AimCfg.enabled || !g_UE4) continue;
        
        // Read world
        uintptr_t pWorld = 0;
        mem::Read((void*)(g_UE4 + Off::GWorld), &pWorld, sizeof(uintptr_t));
        if (!pWorld || !mem::IsValid((void*)pWorld)) continue;
        
        // GameInstance → LocalPlayers[0] → PC
        uintptr_t gi = 0;
        mem::Read((void*)(pWorld + Off::OwningGameInstance), &gi, sizeof(uintptr_t));
        if (!gi || !mem::IsValid((void*)gi)) continue;
        
        TArray<uintptr_t> lp;
        mem::Read((void*)(gi + Off::LocalPlayers), &lp, sizeof(lp));
        if (!lp.IsValid()) continue;
        
        uintptr_t localP = 0;
        mem::Read(&lp.Data[0], &localP, sizeof(uintptr_t));
        if (!localP || !mem::IsValid((void*)localP)) continue;
        
        uintptr_t myPC = 0;
        mem::Read((void*)(localP + Off::PlayerController), &myPC, sizeof(uintptr_t));
        if (!myPC || !mem::IsValid((void*)myPC)) continue;
        
        uintptr_t camMgr = 0;
        mem::Read((void*)(myPC + Off::PlayerCameraMgr), &camMgr, sizeof(uintptr_t));
        if (!camMgr || !mem::IsValid((void*)camMgr)) continue;
        
        uintptr_t myPawn = 0;
        mem::Read((void*)(myPC + Off::AcknowledgedPawn), &myPawn, sizeof(uintptr_t));
        if (!myPawn || !mem::IsValid((void*)myPawn)) continue;
        
        // Camera
        FMinimalViewInfo pov;
        mem::Read((void*)(camMgr + Off::CameraCachePOV), &pov, sizeof(pov));
        if (pov.FOV <= 0 || pov.FOV > 170) continue;
        
        FRotator curRot;
        mem::Read((void*)(myPC + Off::ControlRotation), &curRot, sizeof(FRotator));
        
        // My team & position
        int32_t myTeam = -1;
        mem::Read((void*)(myPawn + Off::TeamID), &myTeam, sizeof(int32_t));
        
        FVector myLoc{};
        uintptr_t myRoot = 0;
        mem::Read((void*)(myPawn + Off::RootComponent), &myRoot, sizeof(uintptr_t));
        if (myRoot && mem::IsValid((void*)myRoot))
            mem::Read((void*)(myRoot + Off::RelativeLocation), &myLoc, sizeof(FVector));
        
        // Actors
        uintptr_t level = 0;
        mem::Read((void*)(pWorld + Off::PersistentLevel), &level, sizeof(uintptr_t));
        if (!level || !mem::IsValid((void*)level)) continue;
        
        TArray<uintptr_t> actors;
        mem::Read((void*)(level + Off::Actors), &actors, sizeof(actors));
        if (!actors.IsValid() || actors.Count > 50000 || actors.Count < 1) continue;
        if (!mem::IsValid(actors.Data)) continue;
        
        // Find closest enemy head
        float bestScreenDist = AimCfg.fov;
        FVector bestHead{};
        bool found = false;
        
        int maxScan = actors.Count < 200 ? actors.Count : 200;
        for (int i = 0; i < maxScan; i++) {
            uintptr_t actor = 0;
            mem::Read(&actors.Data[i], &actor, sizeof(uintptr_t));
            if (!actor || !mem::IsValid((void*)actor)) continue;
            
            int32_t team = -1;
            mem::Read((void*)(actor + Off::TeamID), &team, sizeof(int32_t));
            if (team < 0 || team > 200 || team == myTeam) continue;
            
            uint8_t dead = 0;
            mem::Read((void*)(actor + Off::bDead), &dead, sizeof(uint8_t));
            if (dead) continue;
            
            float hp = 0;
            mem::Read((void*)(actor + Off::Health), &hp, sizeof(float));
            if (hp <= 0) continue;
            
            uintptr_t mesh = 0;
            mem::Read((void*)(actor + Off::Mesh), &mesh, sizeof(uintptr_t));
            if (!mesh || !mem::IsValid((void*)mesh)) continue;
            
            FVector head = GetBoneWorld(mesh, 6); // Head bone
            if (head.Length() < 1.0f) continue;
            
            float dist = (head - myLoc).Length() / 100.0f;
            if (dist > 350.0f || dist < 0.5f) continue;
            
            // Calculate screen distance (angular)
            FVector d = head - pov.Location;
            float h = sqrtf(d.X*d.X + d.Y*d.Y);
            float yaw = atan2f(d.Y, d.X) * 57.2957795f;
            float pitch = atan2f(d.Z, h) * 57.2957795f;
            float dY = NormAngle(yaw - curRot.Yaw);
            float dP = NormAngle(pitch - curRot.Pitch);
            float angDist = sqrtf(dY*dY + dP*dP);
            
            if (angDist < bestScreenDist) {
                bestScreenDist = angDist;
                bestHead = head;
                found = true;
            }
        }
        
        if (!found) continue;
        
        // Smooth aim
        FVector d = bestHead - pov.Location;
        float h = sqrtf(d.X*d.X + d.Y*d.Y);
        float desYaw = atan2f(d.Y, d.X) * 57.2957795f;
        float desPitch = atan2f(d.Z, h) * 57.2957795f;
        
        float dP = NormAngle(desPitch - curRot.Pitch);
        float dY = NormAngle(desYaw - curRot.Yaw);
        
        if (fabsf(dP) < 0.1f && fabsf(dY) < 0.1f) continue; // Dead zone
        
        // Humanized smooth (acceleration curve)
        float totalErr = sqrtf(dP*dP + dY*dY);
        float factor = 1.0f - powf(1.0f - (totalErr > 30.f ? 1.f : totalErr/30.f), 2.5f);
        float smooth = AimCfg.smooth * (0.3f + 0.7f * (1.0f - factor));
        if (smooth < 1.5f) smooth = 1.5f;
        
        FRotator newRot;
        newRot.Pitch = curRot.Pitch + dP / smooth + RandF(-AimCfg.jitter, AimCfg.jitter);
        newRot.Yaw = curRot.Yaw + dY / smooth + RandF(-AimCfg.jitter, AimCfg.jitter);
        newRot.Roll = 0;
        newRot.Pitch = NormAngle(newRot.Pitch);
        newRot.Yaw = NormAngle(newRot.Yaw);
        if (newRot.Pitch > 89.f) newRot.Pitch = 89.f;
        if (newRot.Pitch < -89.f) newRot.Pitch = -89.f;
        
        // Write rotation (memory write only — no function call)
        mem::Write((void*)(myPC + Off::ControlRotation), &newRot, sizeof(FRotator));
    }
    return nullptr;
}

// Main init thread
static void *worker_thread(void *) {
    LOGI("[*] Init started");
    
    // Wait libs
    while (!mem::GetBase("libUE4.so")) usleep(500000);
    g_UE4 = mem::GetBase("libUE4.so");
    LOGI("[+] UE4=0x%lx", (unsigned long)g_UE4);
    
    while (!mem::GetBase("libanogs.so")) usleep(500000);
    LOGI("[+] anogs found");
    
    for (int i = 0; i < 10; i++) {
        if (mem::GetBase("libanort.so")) break;
        usleep(500000);
    }
    
    // Let libs init
    sleep(12);
    
    // Apply bypass
    bypass::Apply();
    LOGI("[+] Bypass done");
    
    // Wait for match (GWorld valid + LocalPlayer exists)
    LOGI("[*] Waiting for match...");
    for (int i = 0; i < 300; i++) { // 5 min max
        uintptr_t pw = 0;
        mem::Read((void*)(g_UE4 + Off::GWorld), &pw, sizeof(uintptr_t));
        if (pw && mem::IsValid((void*)pw)) {
            uintptr_t gi = 0;
            mem::Read((void*)(pw + Off::OwningGameInstance), &gi, sizeof(uintptr_t));
            if (gi && mem::IsValid((void*)gi)) {
                TArray<uintptr_t> lp;
                mem::Read((void*)(gi + Off::LocalPlayers), &lp, sizeof(lp));
                if (lp.IsValid()) {
                    LOGI("[+] Game ready at %d sec", i);
                    break;
                }
            }
        }
        sleep(1);
    }
    
    // Extra wait for stability
    sleep(10);
    
    // Activate aimbot
    g_Active = true;
    LOGI("[+] ACTIVE");
    
    return nullptr;
}

__attribute__((constructor))
void __attribute__((used)) entry() {
    pthread_t t1, t2;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&t1, &attr, worker_thread, nullptr);
    pthread_create(&t2, &attr, aim_thread, nullptr);
    pthread_attr_destroy(&attr);
}
