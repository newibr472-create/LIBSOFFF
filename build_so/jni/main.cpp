// BGMI 4.4.0 — v8: Fixed timing + canvas status display
// KRAFTON logo crash fix: delay anogs patches too
// libsigner DISABLED. hdmpvecore/hdmpve/TBlueData moved to post-world.
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
#include <cstdio>

#undef TAG
#undef LOGI
#undef LOGE
#define TAG "V8"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

// Status string for canvas display
static char g_Status[128] = "INIT";
static std::atomic<int> g_PatchOK{0};
static std::atomic<int> g_PatchFail{0};
static std::atomic<bool> g_Ready{false};

static APlayerController* g_PC = nullptr;
static stealth::Engine g_Engine;

// Non-blocking lib wait
static uintptr_t waitLib(const char* name, int maxMs) {
    uintptr_t base = 0;
    for (int i = 0; i < maxMs/100; i++) {
        base = stealth::GetLib(name);
        if (base) return base;
        usleep(100000);
    }
    return 0;
}

// ===================== PHASED BYPASS =====================
// Phase 1: anogs ONLY — after KRAFTON logo (wait for UE4 first!)
static void BypassPhase1() {
    snprintf(g_Status, sizeof(g_Status), "P1: anogs 0/%d", 18);
    
    std::vector<stealth::Patch> anogs(patches_anogs, patches_anogs + 18);
    int ok = g_Engine.applyGroup(anogs, 0, 800, 4000);
    g_PatchOK += ok;
    
    snprintf(g_Status, sizeof(g_Status), "P1 DONE: %d/18", ok);
    LOGI("[P1] anogs: %d/18", ok);
}

// Phase 2: UE4 NOPs (after world loads)
static void BypassPhase2() {
    snprintf(g_Status, sizeof(g_Status), "P2: UE4 0/%d", 344);
    
    std::vector<stealth::Patch> ue4(patches_UE4, patches_UE4 + 344);
    int ok = g_Engine.applyGroup(ue4, 20, 50, 300);
    g_PatchOK += ok;
    
    snprintf(g_Status, sizeof(g_Status), "P2 DONE: %d/344", ok);
    LOGI("[P2] UE4: %d/344", ok);
}

// Phase 3: Telemetry (10 sec after world — least critical)
static void BypassPhase3() {
    snprintf(g_Status, sizeof(g_Status), "P3: HD 0/%d", 444);
    
    int total = 0;
    if (stealth::GetLib("libhdmpvecore.so")) {
        std::vector<stealth::Patch> hdc(patches_hdmpvecore, patches_hdmpvecore + 381);
        total += g_Engine.applyGroup(hdc, 364, 50, 200);
    }
    if (stealth::GetLib("libhdmpve.so")) {
        std::vector<stealth::Patch> hdm(patches_hdmpve, patches_hdmpve + 27);
        total += g_Engine.applyGroup(hdm, 745, 100, 500);
    }
    if (stealth::GetLib("libTBlueData.so")) {
        std::vector<stealth::Patch> tb(patches_TBlueData, patches_TBlueData + 36);
        total += g_Engine.applyGroup(tb, 772, 100, 500);
    }
    g_PatchOK += total;
    
    // libsigner: DISABLED (causes ban detection)
    // if (stealth::GetLib("libsigner.so")) { ... }
    
    snprintf(g_Status, sizeof(g_Status), "ALL DONE: %d/%d", g_PatchOK.load(), 808);
    LOGI("[P3] telemetry: %d, TOTAL: %d", total, g_PatchOK.load());
}

// ===================== BONE/ESP =====================
struct FTransformLocal { float Rotation[4], Translation[4], Scale3D[4]; };

static FVector GetBoneWorld(uintptr_t mesh, int idx) {
    FVector z{};
    if (!mesh || !Tools::IsPtrValid((void*)mesh)) return z;
    struct { FTransformLocal* Data; int32_t Count; int32_t Max; } bones{};
    Tools::PVM_ReadAddr((void*)(mesh + 0x600), &bones, sizeof(bones));
    if (!bones.Data || bones.Count<=0 || idx>=bones.Count || idx<0) return z;
    if (!Tools::IsPtrValid(bones.Data)) return z;
    FTransformLocal bone{}, comp{};
    Tools::PVM_ReadAddr(&bones.Data[idx], &bone, sizeof(bone));
    Tools::PVM_ReadAddr((void*)(mesh + 0x1D0), &comp, sizeof(comp));
    FVector r{comp.Translation[0]+bone.Translation[0], comp.Translation[1]+bone.Translation[1], comp.Translation[2]+bone.Translation[2]};
    if(fabsf(r.X)>500000) return z;
    return r;
}

static float normA(float a){while(a>180)a-=360;while(a<-180)a+=360;return a;}

static void RenderESP(UCanvas* Canvas, int SW, int SH) {
    // ALWAYS draw status line at top (even before ESP works)
    // Format: "V8 | P:XXX/808 | F:X"
    // We'll draw it as a colored line at top indicating progress
    float progress = (float)g_PatchOK.load() / 808.0f;
    float barW = SW * progress;
    // Green progress bar at very top
    Canvas->K2_DrawLine({0, 2}, {barW, 2}, 4.f, {0, 1, 0, 0.7f});
    // Red for remaining
    if (barW < SW)
        Canvas->K2_DrawLine({barW, 2}, {(float)SW, 2}, 4.f, {1, 0, 0, 0.3f});

    if (!g_PC || !Tools::IsPtrValid(g_PC)) return;

    uintptr_t camMgr=0;
    Tools::PVM_ReadAddr((void*)((uintptr_t)g_PC + 0x548), &camMgr, sizeof(camMgr));
    if (!camMgr || !Tools::IsPtrValid((void*)camMgr)) return;

    FMinimalViewInfo pov{};
    Tools::PVM_ReadAddr((void*)(camMgr + 0x530), &pov, sizeof(pov));
    if (pov.FOV<=0||pov.FOV>170) pov.FOV=90.f;

    uintptr_t myPawn=0;
    Tools::PVM_ReadAddr((void*)((uintptr_t)g_PC + 0x528), &myPawn, sizeof(myPawn));
    if (!myPawn || !Tools::IsPtrValid((void*)myPawn)) return;

    int32_t myTeam=-1;
    Tools::PVM_ReadAddr((void*)(myPawn + 0x998), &myTeam, sizeof(myTeam));

    auto World = GetFullWorld();
    if (!World || !Tools::IsPtrValid(World)) return;

    ULevel* level=nullptr;
    Tools::PVM_ReadAddr((void*)((uintptr_t)World + 0x30), &level, sizeof(level));
    if (!level || !Tools::IsPtrValid(level)) return;

    typedef struct{uintptr_t base;int32_t count;int32_t max;}ActorArr;
    typedef ActorArr*(*GetActorsFn)(uintptr_t);
    auto fn = (GetActorsFn)(UE4 + Off::GetActorArray);
    ActorArr* arr = fn((uintptr_t)level);
    if (!arr || arr->count<=0 || arr->count>50000) return;

    FLinearColor red{1,0.3f,0.3f,1}, white{1,1,1,0.8f};
    float bestAng=100; FVector bestTgt{}; bool hasTgt=false;
    int scan = arr->count<300 ? arr->count : 300;

    for (int i=0; i<scan; i++) {
        uintptr_t actor=0;
        Tools::PVM_ReadAddr((void*)(arr->base+i*8), &actor, sizeof(actor));
        if (!actor||actor==myPawn||!Tools::IsPtrValid((void*)actor)) continue;

        int32_t team=-1;
        Tools::PVM_ReadAddr((void*)(actor+0x998), &team, sizeof(team));
        if (team<0||team>200||team==myTeam) continue;

        uint8_t dead=0;
        Tools::PVM_ReadAddr((void*)(actor+0x0E7C), &dead, 1);
        if (dead) continue;

        float hp=0;
        Tools::PVM_ReadAddr((void*)(actor+0x0E60), &hp, sizeof(hp));
        if (hp<=0) continue;

        uintptr_t mesh=0;
        Tools::PVM_ReadAddr((void*)(actor+0x510), &mesh, sizeof(mesh));
        if (!mesh||!Tools::IsPtrValid((void*)mesh)) continue;

        FVector head=GetBoneWorld(mesh,6), root=GetBoneWorld(mesh,0);
        if (head.Size()<1||root.Size()<1) continue;

        FVector2D hs{},rs{};
        if (!g_PC->ProjectWorldLocationToScreen(head,true,hs)) continue;
        if (!g_PC->ProjectWorldLocationToScreen(root,true,rs)) continue;

        float bH=rs.Y-hs.Y;
        if (bH<5||bH>2000) continue;
        float bW=bH*0.45f, cx=(hs.X+rs.X)*0.5f;
        float cl=bH*0.15f, L=cx-bW/2, R=cx+bW/2;

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
        for(auto&b:bones){
            FVector ba=GetBoneWorld(mesh,b[0]),bb=GetBoneWorld(mesh,b[1]);
            if(ba.Size()<1||bb.Size()<1)continue;
            FVector2D sa{},sb{};
            if(g_PC->ProjectWorldLocationToScreen(ba,true,sa)&&g_PC->ProjectWorldLocationToScreen(bb,true,sb))
                Canvas->K2_DrawLine(sa,sb,1.0f,white);
        }

        // Health bar
        float hpMax=0;
        Tools::PVM_ReadAddr((void*)(actor+0x0E64),&hpMax,sizeof(hpMax));
        if(hpMax>0){
            float ratio=hp/hpMax;if(ratio>1)ratio=1;
            float barX=L-5;
            Canvas->K2_DrawLine({barX,hs.Y},{barX,rs.Y},3.f,{0.1f,0.1f,0.1f,0.6f});
            float filled=hs.Y+bH*(1.f-ratio);
            Canvas->K2_DrawLine({barX,filled},{barX,rs.Y},2.f,{1-ratio,ratio,0,1});
        }

        // Aimbot target
        FVector d=head-pov.Location;
        float hz=sqrtf(d.X*d.X+d.Y*d.Y);
        float yaw=atan2f(d.Y,d.X)*57.2957795f;
        float pitch=atan2f(d.Z,hz)*57.2957795f;
        float ang=sqrtf(normA(yaw-pov.Rotation.Yaw)*normA(yaw-pov.Rotation.Yaw)+normA(pitch-pov.Rotation.Pitch)*normA(pitch-pov.Rotation.Pitch));
        if(ang<bestAng){bestAng=ang;bestTgt=head;hasTgt=true;}
    }

    // Aimbot
    if(hasTgt&&bestAng<15.f){
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
            if(nr.Pitch>89)nr.Pitch=89;if(nr.Pitch<-89)nr.Pitch=-89;
            Tools::PVM_WriteAddr((void*)((uintptr_t)g_PC+0x4E0),&nr,sizeof(nr));
        }
    }
}

// ===================== POSTRENDER HOOK =====================
void*(*orig_PR)(UGameViewportClient*,UCanvas*);
void* hook_PR(UGameViewportClient* vp, UCanvas* canvas) {
    if (canvas && Tools::IsPtrValid(canvas)) {
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
    orig_PR = (void*(*)(UGameViewportClient*,UCanvas*))VT[134];
    size_t ps = sysconf(_SC_PAGESIZE);
    void* page = (void*)((uintptr_t)&VT[134] & ~(ps-1));
    stealth::raw_mprotect(page, ps*2, PROT_READ|PROT_WRITE|PROT_EXEC);
    __atomic_store_n(&VT[134], (void*)hook_PR, __ATOMIC_SEQ_CST);
    stealth::raw_mprotect(page, ps*2, PROT_READ|PROT_EXEC);
    LOGI("[+] Hooked VT[134]");
    return true;
}

// ===================== MAIN =====================
static void* MainThread(void*) {
    LOGI("[*] v8 start");
    g_Engine.init();
    snprintf(g_Status, sizeof(g_Status), "Waiting UE4...");

    // Wait UE4 — game must pass KRAFTON logo first
    UE4 = waitLib("libUE4.so", 120000);
    if (!UE4) { LOGE("UE4 timeout"); return nullptr; }
    LOGI("[+] UE4=0x%lx", (unsigned long)UE4);

    // Wait for anogs to load
    waitLib("libanogs.so", 60000);
    
    // IMPORTANT: Wait for KRAFTON logo to pass (10 sec after UE4 loads)
    snprintf(g_Status, sizeof(g_Status), "Waiting logo...");
    for (int i=0; i<100; i++) usleep(100000); // 10 sec in 100ms chunks
    
    // === PHASE 1: anogs only ===
    BypassPhase1();

    // === Init SDK ===
    snprintf(g_Status, sizeof(g_Status), "SDK init...");
    GetGNamesFunc = (TNameEntryArray*(*)())(UE4 + Off::GNames);
    for (int i=0; i<60; i++) {
        UObject::GNames = GetGNamesFunc();
        if (UObject::GNames) break;
        usleep(500000);
    }
    if (!UObject::GNames) { LOGE("GNames fail"); return nullptr; }
    UObject::GUObjectArray = (FUObjectArray*)(UE4 + Off::GUObjectArray);
    LOGI("[+] SDK OK");

    // === Wait for World (lobby/match) ===
    snprintf(g_Status, sizeof(g_Status), "Waiting world...");
    UWorld* world = nullptr;
    for (int i=0; i<180; i++) {
        world = GetFullWorld();
        if (world && Tools::IsPtrValid(world)) break;
        usleep(2000000);
    }
    if (!world) { LOGE("World timeout"); return nullptr; }
    LOGI("[+] World OK");

    // === PHASE 2: UE4 NOPs (in-game) ===
    BypassPhase2();
    
    // === Wait 10 more seconds then Phase 3 ===
    snprintf(g_Status, sizeof(g_Status), "P3 wait...");
    for (int i=0; i<100; i++) usleep(100000); // 10 sec
    BypassPhase3();

    // === Find PlayerController ===
    snprintf(g_Status, sizeof(g_Status), "Finding PC...");
    for (int i=0; i<30; i++) {
        world = GetFullWorld();
        if (!world){usleep(2000000);continue;}
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
        if(g_PC&&Tools::IsPtrValid(g_PC)){LOGI("[+] PC");break;}
        usleep(2000000);
    }
    if(!g_PC){LOGE("PC fail");return nullptr;}

    // === Hook PostRender ===
    for (int i=0; i<20; i++) {
        if (HookPostRender()) break;
        usleep(3000000);
    }

    g_Ready.store(true);
    snprintf(g_Status, sizeof(g_Status), "ACTIVE %d/%d", g_PatchOK.load(), 808);
    LOGI("[+] v8 ACTIVE — %d patches OK", g_PatchOK.load());
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
