// BGMI 4.4.0 — v7: Stealth Engine + Phased Bypass + ESP/Aimbot
// Custom bypass engine — randomized, staggered, page-cloaked, logged
// NO sleep() blocking. Non-blocking polling + usleep micro-waits.
#include "sdk.h"
#include "stealth.h"
#include "bypass_patches.h"
#include "Main/Tools.h"
#include "Main/KittyMemory/MemoryPatch.h"
#include <pthread.h>
#include <unistd.h>
#include <sys/mman.h>
#include <cmath>
#include <atomic>
#include <vector>
#include <android/log.h>

#undef TAG
#undef LOGI
#undef LOGE
#define TAG "V7"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

// Globals
static APlayerController* g_PC = nullptr;
static std::atomic<bool> g_Ready{false};
static stealth::Engine g_Engine;

// Non-blocking wait for library
static uintptr_t waitLib(const char* name, int maxMs) {
    uintptr_t base = 0;
    for (int i = 0; i < maxMs/50; i++) {
        base = stealth::GetLib(name);
        if (base) return base;
        usleep(50000); // 50ms polls, NOT seconds
    }
    return 0;
}

// ===================== PHASED BYPASS =====================
// Phase 1: Critical AC libs (before login screen)
static void BypassPhase1() {
    LOGI("=== PHASE 1: AC CORE ===");
    
    // anogs MUST be patched first
    if (!waitLib("libanogs.so", 60000)) { LOGE("anogs not found!"); return; }
    
    std::vector<stealth::Patch> anogs(patches_anogs, patches_anogs + 18);
    int ok = g_Engine.applyGroup(anogs, 0, 500, 3000);
    LOGI("[P1] anogs: %d/18", ok);
    
    // signer
    if (waitLib("libsigner.so", 5000)) {
        std::vector<stealth::Patch> signer(patches_signer, patches_signer + 2);
        ok = g_Engine.applyGroup(signer, 18, 200, 1000);
        LOGI("[P1] signer: %d/2", ok);
    }
    
    LOGI("=== PHASE 1 DONE: %d total ===", g_Engine.total());
}

// Phase 2: Analytics/telemetry (after libs loaded, before match)
static void BypassPhase2() {
    LOGI("=== PHASE 2: TELEMETRY ===");
    
    if (waitLib("libhdmpvecore.so", 30000)) {
        std::vector<stealth::Patch> hdcore(patches_hdmpvecore, patches_hdmpvecore + 381);
        int ok = g_Engine.applyGroup(hdcore, 20, 100, 800);
        LOGI("[P2] hdmpvecore: %d/381", ok);
    }
    
    if (waitLib("libhdmpve.so", 10000)) {
        std::vector<stealth::Patch> hdm(patches_hdmpve, patches_hdmpve + 27);
        int ok = g_Engine.applyGroup(hdm, 781, 200, 1500);
        LOGI("[P2] hdmpve: %d/27", ok);
    }
    
    if (waitLib("libTBlueData.so", 10000)) {
        std::vector<stealth::Patch> tblue(patches_TBlueData, patches_TBlueData + 36);
        int ok = g_Engine.applyGroup(tblue, 745, 200, 1500);
        LOGI("[P2] TBlueData: %d/36", ok);
    }
    
    LOGI("=== PHASE 2 DONE: %d total ===", g_Engine.total());
}

// Phase 3: Game engine AC (INSIDE match, after world loads)
static void BypassPhase3() {
    LOGI("=== PHASE 3: UE4 AC ===");
    
    if (!waitLib("libUE4.so", 10000)) { LOGE("UE4 not found!"); return; }
    
    std::vector<stealth::Patch> ue4(patches_UE4, patches_UE4 + 344);
    int ok = g_Engine.applyGroup(ue4, 401, 50, 500); // Fast — UE4 patches are NOPs
    LOGI("[P3] UE4: %d/344", ok);
    
    LOGI("=== PHASE 3 DONE: %d total, %d failed ===", g_Engine.total(), g_Engine.failed());
}

// ===================== ESP/AIMBOT =====================
struct FTransformLocal {
    float Rotation[4], Translation[4], Scale3D[4];
};

static FVector GetBoneWorld(uintptr_t mesh, int idx) {
    FVector z{};
    if (!mesh || !Tools::IsPtrValid((void*)mesh)) return z;
    struct { FTransformLocal* Data; int32_t Count; int32_t Max; } bones{};
    Tools::PVM_ReadAddr((void*)(mesh + 0x600), &bones, sizeof(bones));
    if (!bones.Data || bones.Count <= 0 || idx >= bones.Count || idx < 0) return z;
    if (!Tools::IsPtrValid(bones.Data)) return z;
    FTransformLocal bone{}, comp{};
    Tools::PVM_ReadAddr(&bones.Data[idx], &bone, sizeof(bone));
    Tools::PVM_ReadAddr((void*)(mesh + 0x1D0), &comp, sizeof(comp));
    FVector r{comp.Translation[0]+bone.Translation[0], comp.Translation[1]+bone.Translation[1], comp.Translation[2]+bone.Translation[2]};
    if (fabsf(r.X)>500000) return z;
    return r;
}

static float normA(float a){while(a>180)a-=360;while(a<-180)a+=360;return a;}

static void RenderESP(UCanvas* Canvas, int SW, int SH) {
    if (!g_PC || !Tools::IsPtrValid(g_PC)) return;

    uintptr_t camMgr=0;
    Tools::PVM_ReadAddr((void*)((uintptr_t)g_PC + 0x548), &camMgr, sizeof(camMgr));
    if (!camMgr || !Tools::IsPtrValid((void*)camMgr)) return;

    FMinimalViewInfo pov{};
    Tools::PVM_ReadAddr((void*)(camMgr + 0x530), &pov, sizeof(pov));
    if (pov.FOV <= 0 || pov.FOV > 170) pov.FOV = 90.f;

    uintptr_t myPawn=0;
    Tools::PVM_ReadAddr((void*)((uintptr_t)g_PC + 0x528), &myPawn, sizeof(myPawn));
    if (!myPawn || !Tools::IsPtrValid((void*)myPawn)) return;

    int32_t myTeam=-1;
    Tools::PVM_ReadAddr((void*)(myPawn + 0x998), &myTeam, sizeof(myTeam));

    // Get world + actors
    auto World = GetFullWorld();
    if (!World || !Tools::IsPtrValid(World)) return;

    ULevel* level=nullptr;
    Tools::PVM_ReadAddr((void*)((uintptr_t)World + 0x30), &level, sizeof(level));
    if (!level || !Tools::IsPtrValid(level)) return;

    // Call GetActorArray
    typedef struct { uintptr_t base; int32_t count; int32_t max; } ActorArr;
    typedef ActorArr*(*GetActorsFn)(uintptr_t);
    auto fn = (GetActorsFn)(UE4 + Off::GetActorArray);
    ActorArr* arr = fn((uintptr_t)level);
    if (!arr || arr->count <= 0 || arr->count > 50000) return;

    FLinearColor red{1,0.3f,0.3f,1}, white{1,1,1,0.8f};
    float bestAng=100; FVector bestTgt{}; bool hasTgt=false;
    int scan = arr->count < 300 ? arr->count : 300;

    for (int i = 0; i < scan; i++) {
        uintptr_t actor=0;
        Tools::PVM_ReadAddr((void*)(arr->base + i*8), &actor, sizeof(actor));
        if (!actor || actor == myPawn || !Tools::IsPtrValid((void*)actor)) continue;

        int32_t team=-1;
        Tools::PVM_ReadAddr((void*)(actor + 0x998), &team, sizeof(team));
        if (team<0||team>200||team==myTeam) continue;

        uint8_t dead=0;
        Tools::PVM_ReadAddr((void*)(actor + 0x0E7C), &dead, 1);
        if (dead) continue;

        float hp=0;
        Tools::PVM_ReadAddr((void*)(actor + 0x0E60), &hp, sizeof(hp));
        if (hp <= 0) continue;

        uintptr_t mesh=0;
        Tools::PVM_ReadAddr((void*)(actor + 0x510), &mesh, sizeof(mesh));
        if (!mesh || !Tools::IsPtrValid((void*)mesh)) continue;

        FVector head = GetBoneWorld(mesh, 6);
        FVector root = GetBoneWorld(mesh, 0);
        if (head.Size()<1 || root.Size()<1) continue;

        FVector2D hs{}, rs{};
        if (!g_PC->ProjectWorldLocationToScreen(head, true, hs)) continue;
        if (!g_PC->ProjectWorldLocationToScreen(root, true, rs)) continue;

        float bH = rs.Y - hs.Y;
        if (bH<5||bH>2000) continue;
        float bW = bH*0.45f, cx = (hs.X+rs.X)*0.5f;
        float cl = bH*0.15f;
        float L=cx-bW/2, R=cx+bW/2;

        // Corner box
        Canvas->K2_DrawLine({L,hs.Y},{L+cl,hs.Y},1.5f,red);
        Canvas->K2_DrawLine({L,hs.Y},{L,hs.Y+cl},1.5f,red);
        Canvas->K2_DrawLine({R-cl,hs.Y},{R,hs.Y},1.5f,red);
        Canvas->K2_DrawLine({R,hs.Y},{R,hs.Y+cl},1.5f,red);
        Canvas->K2_DrawLine({L,rs.Y-cl},{L,rs.Y},1.5f,red);
        Canvas->K2_DrawLine({L,rs.Y},{L+cl,rs.Y},1.5f,red);
        Canvas->K2_DrawLine({R,rs.Y-cl},{R,rs.Y},1.5f,red);
        Canvas->K2_DrawLine({R-cl,rs.Y},{R,rs.Y},1.5f,red);

        // Skeleton
        int bones[][2]={{6,5},{5,4},{4,2},{2,0},{5,11},{11,12},{12,13},{5,32},{32,33},{33,34},{0,48},{48,49},{49,50},{0,53},{53,54},{54,55}};
        for (auto& b : bones) {
            FVector ba=GetBoneWorld(mesh,b[0]), bb=GetBoneWorld(mesh,b[1]);
            if(ba.Size()<1||bb.Size()<1) continue;
            FVector2D sa{},sb{};
            if(g_PC->ProjectWorldLocationToScreen(ba,true,sa) && g_PC->ProjectWorldLocationToScreen(bb,true,sb))
                Canvas->K2_DrawLine(sa,sb,1.0f,white);
        }

        // Health bar
        float hpMax=0;
        Tools::PVM_ReadAddr((void*)(actor+0x0E64),&hpMax,sizeof(hpMax));
        if(hpMax>0){
            float ratio=hp/hpMax; if(ratio>1)ratio=1;
            float barX=L-5;
            Canvas->K2_DrawLine({barX,hs.Y},{barX,rs.Y},3.f,{0.1f,0.1f,0.1f,0.6f});
            float filled=hs.Y+bH*(1.f-ratio);
            Canvas->K2_DrawLine({barX,filled},{barX,rs.Y},2.f,{1-ratio,ratio,0,1});
        }

        // Aimbot scoring
        FVector d=head-pov.Location;
        float hz=sqrtf(d.X*d.X+d.Y*d.Y);
        float yaw=atan2f(d.Y,d.X)*57.2957795f;
        float pitch=atan2f(d.Z,hz)*57.2957795f;
        float ang=sqrtf(normA(yaw-pov.Rotation.Yaw)*normA(yaw-pov.Rotation.Yaw)+normA(pitch-pov.Rotation.Pitch)*normA(pitch-pov.Rotation.Pitch));
        if(ang<bestAng){bestAng=ang;bestTgt=head;hasTgt=true;}
    }

    // FOV circle
    float cx2=SW/2.f,cy2=SH/2.f,r=100.f;
    for(int i=0;i<36;i++){
        float a1=6.2832f*i/36,a2=6.2832f*(i+1)/36;
        Canvas->K2_DrawLine({cx2+cosf(a1)*r,cy2+sinf(a1)*r},{cx2+cosf(a2)*r,cy2+sinf(a2)*r},0.5f,{1,1,1,0.3f});
    }

    // Aimbot
    if(hasTgt && bestAng<15.f){
        FRotator cur{};
        Tools::PVM_ReadAddr((void*)((uintptr_t)g_PC+0x4E0),&cur,sizeof(cur));
        FVector d=bestTgt-pov.Location;
        float hz=sqrtf(d.X*d.X+d.Y*d.Y);
        float dP=normA(atan2f(d.Z,hz)*57.2957795f-cur.Pitch);
        float dY=normA(atan2f(d.Y,d.X)*57.2957795f-cur.Yaw);
        float err=sqrtf(dP*dP+dY*dY);
        if(err>0.1f){
            float sm=5.f+(err<10?3.f:0.f);
            FRotator nr;
            nr.Pitch=normA(cur.Pitch+dP/sm);
            nr.Yaw=normA(cur.Yaw+dY/sm);
            nr.Roll=0;
            if(nr.Pitch>89)nr.Pitch=89; if(nr.Pitch<-89)nr.Pitch=-89;
            Tools::PVM_WriteAddr((void*)((uintptr_t)g_PC+0x4E0),&nr,sizeof(nr));
        }
    }
}

// ===================== POSTRENDER HOOK =====================
void*(*orig_PR)(UGameViewportClient*,UCanvas*);
void* hook_PR(UGameViewportClient* vp, UCanvas* canvas) {
    if (canvas && Tools::IsPtrValid(canvas) && g_Ready.load()) {
        if (canvas->SizeX > 0 && canvas->SizeY > 0)
            RenderESP(canvas, canvas->SizeX, canvas->SizeY);
    }
    return orig_PR(vp, canvas);
}

static bool HookPostRender() {
    auto gvp = UObject::FindObject<UGameViewportClient>(
        "GameViewportClient Transient.UAEGameEngine_1.GameViewportClient_1");
    if (!gvp || !Tools::IsPtrValid(gvp)) return false;

    void** VT = *(void***)gvp;
    if (!VT || !Tools::IsPtrValid(VT)) return false;

    int idx = 134;
    orig_PR = (void*(*)(UGameViewportClient*,UCanvas*))VT[idx];

    size_t ps = sysconf(_SC_PAGESIZE);
    void* page = (void*)((uintptr_t)&VT[idx] & ~(ps-1));
    stealth::raw_mprotect(page, ps*2, PROT_READ|PROT_WRITE|PROT_EXEC);
    __atomic_store_n(&VT[idx], (void*)hook_PR, __ATOMIC_SEQ_CST);
    stealth::raw_mprotect(page, ps*2, PROT_READ|PROT_EXEC);

    LOGI("[+] PostRender VT[134] hooked");
    return true;
}

// ===================== MAIN =====================
static void* MainThread(void*) {
    LOGI("[*] v7 — stealth engine init");
    g_Engine.init();

    // === Wait for UE4 (non-blocking) ===
    UE4 = waitLib("libUE4.so", 120000);
    if (!UE4) { LOGE("UE4 timeout"); return nullptr; }
    LOGI("[+] UE4=0x%lx", (unsigned long)UE4);

    // === PHASE 1: AC core (immediate) ===
    BypassPhase1();

    // === Init SDK ===
    GetGNamesFunc = (TNameEntryArray*(*)())(UE4 + Off::GNames);
    for (int i=0; i<60; i++) {
        UObject::GNames = GetGNamesFunc();
        if (UObject::GNames) break;
        usleep(500000);
    }
    if (!UObject::GNames) { LOGE("GNames fail"); return nullptr; }
    LOGI("[+] GNames OK");

    UObject::GUObjectArray = (FUObjectArray*)(UE4 + Off::GUObjectArray);
    LOGI("[+] GUObjectArray OK");

    // === PHASE 2: Telemetry (after engine up) ===
    BypassPhase2();

    // === Wait for World (non-blocking, polls every 2s) ===
    UWorld* world = nullptr;
    for (int i=0; i<90; i++) {
        world = GetFullWorld();
        if (world && Tools::IsPtrValid(world)) break;
        usleep(2000000);
    }
    if (!world) { LOGE("World timeout"); return nullptr; }
    LOGI("[+] World OK");

    // === PHASE 3: UE4 AC (in-game) ===
    BypassPhase3();

    // === Find PlayerController ===
    for (int i=0; i<30; i++) {
        world = GetFullWorld();
        if (!world) { usleep(2000000); continue; }
        uintptr_t gi=0;
        Tools::PVM_ReadAddr((void*)((uintptr_t)world+0x470),&gi,sizeof(gi));
        if(!gi||!Tools::IsPtrValid((void*)gi)){usleep(2000000);continue;}
        struct{UObject**Data;int32_t Count;int32_t Max;}lp{};
        Tools::PVM_ReadAddr((void*)(gi+0x48),&lp,sizeof(lp));
        if(!lp.Data||lp.Count<=0){usleep(2000000);continue;}
        UObject* localP=nullptr;
        Tools::PVM_ReadAddr(&lp.Data[0],&localP,sizeof(localP));
        if(!localP||!Tools::IsPtrValid(localP)){usleep(2000000);continue;}
        Tools::PVM_ReadAddr((void*)((uintptr_t)localP+0x30),&g_PC,sizeof(g_PC));
        if(g_PC&&Tools::IsPtrValid(g_PC)){LOGI("[+] PC found");break;}
        usleep(2000000);
    }
    if(!g_PC){LOGE("PC fail");return nullptr;}

    // === Hook PostRender ===
    for (int i=0; i<20; i++) {
        if (HookPostRender()) break;
        usleep(3000000);
    }

    g_Ready.store(true);
    LOGI("[+] v7 ACTIVE — %d patches, %d fails", g_Engine.total(), g_Engine.failed());
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
