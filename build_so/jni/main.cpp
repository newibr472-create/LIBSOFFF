// BGMI 4.4.0 — v6: Reference-based ESP + Aimbot + 808 bypass patches
// Uses ProcessEvent for K2_DrawLine, GetActorArray function call, VTable hook
#include "sdk.h"
#include "Main/Tools.h"
#include "Main/KittyMemory/KittyMemory.h"
#include "Main/KittyMemory/MemoryPatch.h"
#include <android/log.h>
// android_app forward declaration
struct android_app {
    void* userData;
    void* activity; // ANativeActivity*
    // ... we only need the pointer
};
#include <pthread.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <cmath>
#include <atomic>
#include <vector>
#include <algorithm>

#define TAG "BGMI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

// ===================== GLOBALS =====================
static android_app* g_App = nullptr;
static APlayerController* g_PlayerController = nullptr;
static ASTExtraPlayerCharacter* g_LocalPlayer = nullptr;
static int screenWidth = 0, screenHeight = 0;

// ===================== BYPASS =====================
static void ApplyBypass() {
    // Wait for all AC libraries
    while (!Tools::GetBaseAddress("libanogs.so")) usleep(300000);
    while (!Tools::GetBaseAddress("libUE4.so")) usleep(300000);
    for(int i=0;i<20;i++){if(Tools::GetBaseAddress("libhdmpvecore.so"))break;usleep(300000);}
    for(int i=0;i<10;i++){if(Tools::GetBaseAddress("libhdmpve.so"))break;usleep(300000);}
    for(int i=0;i<10;i++){if(Tools::GetBaseAddress("libTBlueData.so"))break;usleep(300000);}
    for(int i=0;i<10;i++){if(Tools::GetBaseAddress("libsigner.so"))break;usleep(300000);}
    sleep(5);

    auto patch = [](const char* lib, uintptr_t off, const std::string& hex) {
        auto p = MemoryPatch::createWithHex(lib, off, hex);
        if (p.isValid()) p.Modify();
    };

    // === libanogs.so + libsigner.so (20) ===
    patch("libanogs.so", 0x1d7938, "00 00 80 52 C0 03 5F D6");
    patch("libanogs.so", 0x1d551c, "00 00 80 52 C0 03 5F D6");
    patch("libanogs.so", 0x1d624c, "C0 03 5F D6");
    patch("libanogs.so", 0x1d6598, "00 00 80 52 C0 03 5F D6");
    patch("libanogs.so", 0x1d6ea8, "00 00 80 52 C0 03 5F D6");
    patch("libanogs.so", 0x1d79a4, "C0 03 5F D6");
    patch("libanogs.so", 0x1d7fc4, "00 00 80 52 C0 03 5F D6");
    patch("libanogs.so", 0x1d88ec, "C0 03 5F D6");
    patch("libanogs.so", 0x1d417c, "C0 03 5F D6");
    patch("libanogs.so", 0x1d5a88, "C0 03 5F D6");
    patch("libanogs.so", 0x1d82cc, "C0 03 5F D6");
    patch("libanogs.so", 0x1d4580, "C0 03 5F D6");
    patch("libanogs.so", 0x1d7398, "C0 03 5F D6");
    patch("libanogs.so", 0x1d9024, "C0 03 5F D6");
    patch("libanogs.so", 0x1d4c0c, "C0 03 5F D6");
    patch("libanogs.so", 0x1d5030, "C0 03 5F D6");
    patch("libanogs.so", 0x1d78cc, "00 00 80 52 C0 03 5F D6");
    patch("libanogs.so", 0x1d8c74, "C0 03 5F D6");
    patch("libsigner.so", 0x9a088, "C0 03 5F D6");
    patch("libsigner.so", 0x9afc0, "C0 03 5F D6");
    LOGI("[+] anogs+signer: 20");

    // === libhdmpvecore.so — include from header ===
    #define HPATCH(off, hex) patch("libhdmpvecore.so", off, hex)
    if (Tools::GetBaseAddress("libhdmpvecore.so")) {
        #include "bypass_hdmpvecore.h"
        LOGI("[+] hdmpvecore done");
    }
    #undef HPATCH

    // === libUE4.so (NOP only) ===
    #define UPATCH(off, hex) patch("libUE4.so", off, hex)
    #include "bypass_ue4.h"
    LOGI("[+] UE4 done");
    #undef UPATCH

    // === libTBlueData.so ===
    #define TPATCH(off, hex) patch("libTBlueData.so", off, hex)
    if (Tools::GetBaseAddress("libTBlueData.so")) {
        #include "bypass_tblue.h"
        LOGI("[+] TBlueData done");
    }
    #undef TPATCH

    // === libhdmpve.so ===
    #define MPATCH(off, hex) patch("libhdmpve.so", off, hex)
    if (Tools::GetBaseAddress("libhdmpve.so")) {
        #include "bypass_hdmpve.h"
        LOGI("[+] hdmpve done");
    }
    #undef MPATCH

    LOGI("[+] ALL BYPASS DONE (808 patches)");
}

// ===================== WORLD ACCESS =====================
static UWorld* GetFullWorld() {
    static int GWorldIdx = 0;
    auto& objs = UObject::GetGlobalObjects();
    
    if (GWorldIdx == 0) {
        for (int i = 0; i < objs.Num(); i++) {
            auto obj = objs.GetByIndex(i);
            if (!obj || !Tools::IsPtrValid(obj)) continue;
            if (obj->IsA(UEngine::StaticClass())) {
                auto engine = (UEngine*)obj;
                if (!engine || !Tools::IsPtrValid(engine)) continue;
                // GameViewport at dynamic offset — use FindObject instead
                auto vp = UObject::FindObject<UGameViewportClient>(
                    "GameViewportClient Transient.UAEGameEngine_1.GameViewportClient_1");
                if (vp && Tools::IsPtrValid(vp)) {
                    GWorldIdx = i;
                    // World is at a specific offset in UGameViewportClient
                    // Read it from the viewport's World member
                    uintptr_t vpAddr = (uintptr_t)vp;
                    UWorld* w = nullptr;
                    Tools::PVM_ReadAddr((void*)(vpAddr + 0x78), &w, sizeof(w));
                    return w;
                }
            }
        }
    } else {
        auto engine = (UEngine*)objs.GetByIndex(GWorldIdx);
        if (engine && Tools::IsPtrValid(engine)) {
            auto vp = UObject::FindObject<UGameViewportClient>(
                "GameViewportClient Transient.UAEGameEngine_1.GameViewportClient_1");
            if (vp && Tools::IsPtrValid(vp)) {
                uintptr_t vpAddr = (uintptr_t)vp;
                UWorld* w = nullptr;
                Tools::PVM_ReadAddr((void*)(vpAddr + 0x78), &w, sizeof(w));
                return w;
            }
        }
    }
    return nullptr;
}

// Get actors using function call at UE4+GetActorArray
struct ActorArray {
    uintptr_t base;
    int32_t count;
    int32_t max;
};

static std::vector<UObject*> GetActors() {
    std::vector<UObject*> result;
    auto World = GetFullWorld();
    if (!World || !Tools::IsPtrValid(World)) return result;
    
    ULevel* level = nullptr;
    Tools::PVM_ReadAddr((void*)((uintptr_t)World + 0x30), &level, sizeof(level));
    if (!level || !Tools::IsPtrValid(level)) return result;
    
    // Call GetActorArray function
    typedef ActorArray*(*GetActorArrayFn)(uintptr_t);
    auto fn = (GetActorArrayFn)(UE4 + Off::GetActorArray);
    ActorArray* arr = fn((uintptr_t)level);
    if (!arr || arr->count <= 0 || arr->count > 50000) return result;
    
    for (int i = 0; i < arr->count && i < 500; i++) {
        uintptr_t actor = 0;
        Tools::PVM_ReadAddr((void*)(arr->base + i * sizeof(uintptr_t)), &actor, sizeof(actor));
        if (actor && Tools::IsPtrValid((void*)actor))
            result.push_back((UObject*)actor);
    }
    return result;
}

// ===================== DRAW HELPERS =====================
static void DrawLine(UCanvas* Canvas, FVector2D from, FVector2D to, float thickness, FLinearColor color) {
    if (!Canvas) return;
    Canvas->K2_DrawLine(from, to, thickness, color);
}

static void DrawBox(UCanvas* Canvas, float x, float y, float w, float h, FLinearColor color, float th=1.5f) {
    float cl = h * 0.15f;
    // Top-left
    DrawLine(Canvas, {x,y}, {x+cl,y}, th, color);
    DrawLine(Canvas, {x,y}, {x,y+cl}, th, color);
    // Top-right
    DrawLine(Canvas, {x+w-cl,y}, {x+w,y}, th, color);
    DrawLine(Canvas, {x+w,y}, {x+w,y+cl}, th, color);
    // Bottom-left
    DrawLine(Canvas, {x,y+h-cl}, {x,y+h}, th, color);
    DrawLine(Canvas, {x,y+h}, {x+cl,y+h}, th, color);
    // Bottom-right
    DrawLine(Canvas, {x+w,y+h-cl}, {x+w,y+h}, th, color);
    DrawLine(Canvas, {x+w-cl,y+h}, {x+w,y+h}, th, color);
}

// ===================== BONE READING =====================
struct FTransform {
    float Rotation[4];
    float Translation[4];
    float Scale3D[4];
};

static FVector GetBoneWorldPos(uintptr_t mesh, int boneIdx) {
    FVector zero{};
    if (!mesh || !Tools::IsPtrValid((void*)mesh)) return zero;
    
    // BoneArray at mesh + 0x600 (TArray<FTransform>)
    struct { FTransform* Data; int32_t Count; int32_t Max; } bones{};
    Tools::PVM_ReadAddr((void*)(mesh + 0x600), &bones, sizeof(bones));
    if (!bones.Data || bones.Count <= 0 || boneIdx >= bones.Count || boneIdx < 0) return zero;
    if (!Tools::IsPtrValid(bones.Data)) return zero;
    
    FTransform bone{}, comp{};
    Tools::PVM_ReadAddr(&bones.Data[boneIdx], &bone, sizeof(bone));
    // ComponentToWorld at mesh + 0x1D0
    Tools::PVM_ReadAddr((void*)(mesh + 0x1D0), &comp, sizeof(comp));
    
    FVector result;
    result.X = comp.Translation[0] + bone.Translation[0];
    result.Y = comp.Translation[1] + bone.Translation[1];
    result.Z = comp.Translation[2] + bone.Translation[2];
    
    if (fabsf(result.X) > 500000) return zero;
    return result;
}

// ===================== ESP RENDER =====================
static float normAngle(float a) { while(a>180)a-=360; while(a<-180)a+=360; return a; }

static void RenderESP(UCanvas* Canvas, int SW, int SH) {
    if (!g_PlayerController || !Tools::IsPtrValid(g_PlayerController)) return;
    
    // Get camera info
    uintptr_t camMgr = 0;
    Tools::PVM_ReadAddr((void*)((uintptr_t)g_PlayerController + 0x548), &camMgr, sizeof(camMgr));
    if (!camMgr || !Tools::IsPtrValid((void*)camMgr)) return;
    
    FMinimalViewInfo pov{};
    Tools::PVM_ReadAddr((void*)(camMgr + 0x530), &pov, sizeof(pov));
    if (pov.FOV <= 0 || pov.FOV > 170) pov.FOV = 90.f;
    
    // Get local pawn
    uintptr_t myPawn = 0;
    Tools::PVM_ReadAddr((void*)((uintptr_t)g_PlayerController + 0x528), &myPawn, sizeof(myPawn));
    if (!myPawn || !Tools::IsPtrValid((void*)myPawn)) return;
    
    int32_t myTeam = -1;
    Tools::PVM_ReadAddr((void*)(myPawn + 0x998), &myTeam, sizeof(myTeam));
    
    // Get actors
    auto actors = GetActors();
    
    FLinearColor red{1,0.3f,0.3f,1}, white{1,1,1,0.8f}, green{0,1,0,1};
    float bestAngle = 100.f;
    FVector bestTarget{};
    bool hasTarget = false;
    
    for (auto* actor : actors) {
        if ((uintptr_t)actor == myPawn) continue;
        if (!Tools::IsPtrValid(actor)) continue;
        
        // TeamID
        int32_t team = -1;
        Tools::PVM_ReadAddr((void*)((uintptr_t)actor + 0x998), &team, sizeof(team));
        if (team < 0 || team > 200 || team == myTeam) continue;
        
        // Dead check
        uint8_t dead = 0;
        Tools::PVM_ReadAddr((void*)((uintptr_t)actor + 0x0E7C), &dead, 1);
        if (dead) continue;
        
        // Health
        float hp = 0;
        Tools::PVM_ReadAddr((void*)((uintptr_t)actor + 0x0E60), &hp, sizeof(hp));
        if (hp <= 0) continue;
        
        // Mesh
        uintptr_t mesh = 0;
        Tools::PVM_ReadAddr((void*)((uintptr_t)actor + 0x510), &mesh, sizeof(mesh));
        if (!mesh || !Tools::IsPtrValid((void*)mesh)) continue;
        
        // Head and root bones
        FVector head = GetBoneWorldPos(mesh, 6);
        FVector root = GetBoneWorldPos(mesh, 0);
        if (head.Size() < 1 || root.Size() < 1) continue;
        
        // W2S
        FVector2D headScreen{}, rootScreen{};
        if (!g_PlayerController->ProjectWorldLocationToScreen(head, true, headScreen)) continue;
        if (!g_PlayerController->ProjectWorldLocationToScreen(root, true, rootScreen)) continue;
        
        float boxH = rootScreen.Y - headScreen.Y;
        if (boxH < 5 || boxH > 2000) continue;
        float boxW = boxH * 0.45f;
        float cx = (headScreen.X + rootScreen.X) * 0.5f;
        
        // Corner box
        DrawBox(Canvas, cx - boxW/2, headScreen.Y, boxW, boxH, red, 1.5f);
        
        // Skeleton
        int skelPairs[][2] = {
            {6,5},{5,4},{4,2},{2,0},  // head-spine
            {5,11},{11,12},{12,13},   // left arm
            {5,32},{32,33},{33,34},   // right arm
            {0,48},{48,49},{49,50},   // left leg
            {0,53},{53,54},{54,55}    // right leg
        };
        for (auto& p : skelPairs) {
            FVector ba = GetBoneWorldPos(mesh, p[0]);
            FVector bb = GetBoneWorldPos(mesh, p[1]);
            if (ba.Size() < 1 || bb.Size() < 1) continue;
            FVector2D sa{}, sb{};
            if (g_PlayerController->ProjectWorldLocationToScreen(ba, true, sa) &&
                g_PlayerController->ProjectWorldLocationToScreen(bb, true, sb)) {
                DrawLine(Canvas, sa, sb, 1.0f, white);
            }
        }
        
        // Health bar
        float hpMax = 0;
        Tools::PVM_ReadAddr((void*)((uintptr_t)actor + 0x0E64), &hpMax, sizeof(hpMax));
        if (hpMax > 0) {
            float ratio = hp / hpMax; if (ratio > 1) ratio = 1;
            float barX = cx - boxW/2 - 5;
            DrawLine(Canvas, {barX, headScreen.Y}, {barX, rootScreen.Y}, 3.f, {0.1f,0.1f,0.1f,0.6f});
            float filled = headScreen.Y + boxH * (1.f - ratio);
            FLinearColor hpColor{1.f-ratio, ratio, 0, 1};
            DrawLine(Canvas, {barX, filled}, {barX, rootScreen.Y}, 2.f, hpColor);
        }
        
        // Aimbot target
        FVector d = head - pov.Location;
        float hz = sqrtf(d.X*d.X + d.Y*d.Y);
        float yaw = atan2f(d.Y, d.X) * 57.2957795f;
        float pitch = atan2f(d.Z, hz) * 57.2957795f;
        float dY = normAngle(yaw - pov.Rotation.Yaw);
        float dP = normAngle(pitch - pov.Rotation.Pitch);
        float angle = sqrtf(dY*dY + dP*dP);
        if (angle < bestAngle) {
            bestAngle = angle;
            bestTarget = head;
            hasTarget = true;
        }
    }
    
    // FOV circle
    float cx2 = SW/2.f, cy2 = SH/2.f, r = 100.f;
    for (int i = 0; i < 36; i++) {
        float a1 = 6.2832f*i/36, a2 = 6.2832f*(i+1)/36;
        DrawLine(Canvas, {cx2+cosf(a1)*r, cy2+sinf(a1)*r}, {cx2+cosf(a2)*r, cy2+sinf(a2)*r}, 0.5f, {1,1,1,0.3f});
    }
    
    // Aimbot apply
    if (hasTarget && bestAngle < 15.f) {
        FRotator cur{};
        Tools::PVM_ReadAddr((void*)((uintptr_t)g_PlayerController + 0x4E0), &cur, sizeof(cur));
        FVector d = bestTarget - pov.Location;
        float hz = sqrtf(d.X*d.X + d.Y*d.Y);
        float dP = normAngle(atan2f(d.Z, hz)*57.2957795f - cur.Pitch);
        float dY = normAngle(atan2f(d.Y, d.X)*57.2957795f - cur.Yaw);
        float err = sqrtf(dP*dP + dY*dY);
        if (err > 0.1f) {
            float sm = 6.f;
            FRotator nr;
            nr.Pitch = normAngle(cur.Pitch + dP/sm);
            nr.Yaw = normAngle(cur.Yaw + dY/sm);
            nr.Roll = 0;
            if (nr.Pitch > 89) nr.Pitch = 89;
            if (nr.Pitch < -89) nr.Pitch = -89;
            Tools::PVM_WriteAddr((void*)((uintptr_t)g_PlayerController + 0x4E0), &nr, sizeof(nr));
        }
    }
}

// ===================== POSTRENDER HOOK =====================
void* (*orig_PostRender)(UGameViewportClient*, UCanvas*);
void* new_PostRender(UGameViewportClient* ViewportClient, UCanvas* Canvas) {
    if (ViewportClient && Canvas && Tools::IsPtrValid(Canvas)) {
        if (Canvas->SizeX > 0 && Canvas->SizeY > 0) {
            screenWidth = Canvas->SizeX;
            screenHeight = Canvas->SizeY;
            RenderESP(Canvas, screenWidth, screenHeight);
        }
    }
    return orig_PostRender(ViewportClient, Canvas);
}

static int GetPostRenderIndex() { return 134; }

static void HookPostRender() {
    auto GViewport = UObject::FindObject<UGameViewportClient>(
        "GameViewportClient Transient.UAEGameEngine_1.GameViewportClient_1");
    if (!GViewport || !Tools::IsPtrValid(GViewport)) return;
    
    void** VTable = *(void***)GViewport;
    if (!VTable || !Tools::IsPtrValid(VTable)) return;
    
    int idx = GetPostRenderIndex();
    orig_PostRender = (void*(*)(UGameViewportClient*, UCanvas*))VTable[idx];
    
    // mprotect + swap
    unsigned long page_size = sysconf(_SC_PAGESIZE);
    void* page = (void*)((uintptr_t)&VTable[idx] & ~(page_size - 1));
    mprotect(page, page_size * 2, PROT_READ | PROT_WRITE | PROT_EXEC);
    __atomic_store_n(&VTable[idx], (void*)new_PostRender, __ATOMIC_SEQ_CST);
    
    LOGI("[+] PostRender hooked at VTable[%d]", idx);
}

// ===================== MAIN THREAD =====================
static void* MainThread(void*) {
    LOGI("[*] v6 starting");
    
    // Wait for UE4
    while (!Tools::GetBaseAddress("libUE4.so")) sleep(1);
    UE4 = Tools::GetBaseAddress("libUE4.so");
    LOGI("[+] UE4 = 0x%lx", (unsigned long)UE4);
    
    // Get android_app
    while (!g_App) {
        g_App = *(android_app**)(UE4 + Off::GNativeAndroidApp);
        sleep(1);
    }
    LOGI("[+] g_App OK");
    
    // Init GNames
    GetGNamesFunc = (TNameEntryArray*(*)())(UE4 + Off::GNames);
    while (!UObject::GNames) {
        UObject::GNames = GetGNamesFunc();
        sleep(1);
    }
    LOGI("[+] GNames OK");
    
    // Init GUObjectArray
    UObject::GUObjectArray = (FUObjectArray*)(UE4 + Off::GUObjectArray);
    LOGI("[+] GUObjectArray OK");
    
    // Apply bypass
    ApplyBypass();
    
    // Wait for game to fully load
    sleep(55);
    
    // Find local player and controller
    for (int i = 0; i < 30; i++) {
        auto World = GetFullWorld();
        if (!World) { sleep(2); continue; }
        
        uintptr_t gi = 0;
        Tools::PVM_ReadAddr((void*)((uintptr_t)World + 0x470), &gi, sizeof(gi));
        if (!gi || !Tools::IsPtrValid((void*)gi)) { sleep(2); continue; }
        
        struct { UObject** Data; int32_t Count; int32_t Max; } lp{};
        Tools::PVM_ReadAddr((void*)(gi + 0x48), &lp, sizeof(lp));
        if (!lp.Data || lp.Count <= 0) { sleep(2); continue; }
        
        UObject* localPlayer = nullptr;
        Tools::PVM_ReadAddr(&lp.Data[0], &localPlayer, sizeof(localPlayer));
        if (!localPlayer || !Tools::IsPtrValid(localPlayer)) { sleep(2); continue; }
        
        // PlayerController at ULocalPlayer + 0x30
        Tools::PVM_ReadAddr((void*)((uintptr_t)localPlayer + 0x30), &g_PlayerController, sizeof(g_PlayerController));
        if (g_PlayerController && Tools::IsPtrValid(g_PlayerController)) {
            LOGI("[+] PlayerController found");
            break;
        }
        sleep(2);
    }
    
    if (!g_PlayerController) {
        LOGE("[-] No PlayerController, aborting");
        return nullptr;
    }
    
    // Hook PostRender
    HookPostRender();
    
    LOGI("[+] v6 ACTIVE!");
    return nullptr;
}

__attribute__((constructor))
void lib_entry() {
    pthread_t t;
    pthread_attr_t a;
    pthread_attr_init(&a);
    pthread_attr_setdetachstate(&a, PTHREAD_CREATE_DETACHED);
    pthread_create(&t, &a, MainThread, nullptr);
    pthread_attr_destroy(&a);
}
