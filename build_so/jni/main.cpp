// BGMI 4.4.0 — v8.1: Direct hook (no FindObject dependency) + UE4 NOP delay fix
// ESP: Direct canvas draw without ProcessEvent (call native function pointer)
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

static std::atomic<int> g_PatchOK{0};
static std::atomic<bool> g_ESPReady{false};
static stealth::Engine g_Engine;

// PlayerController found via offset chain
static uintptr_t g_MyPC = 0;

// Non-blocking lib wait
static uintptr_t waitLib(const char* name, int maxMs) {
    for (int i = 0; i < maxMs/100; i++) {
        uintptr_t b = stealth::GetLib(name);
        if (b) return b;
        usleep(100000);
    }
    return 0;
}

// ===================== BYPASS PHASES =====================
static void BypassPhase1() {
    LOGI("=== P1: anogs ===");
    std::vector<stealth::Patch> anogs(patches_anogs, patches_anogs + 18);
    g_PatchOK += g_Engine.applyGroup(anogs, 0, 800, 4000);
    LOGI("[P1] done: %d", g_PatchOK.load());
}

static void BypassPhase2() {
    LOGI("=== P2: UE4 ===");
    std::vector<stealth::Patch> ue4(patches_UE4, patches_UE4 + 344);
    g_PatchOK += g_Engine.applyGroup(ue4, 20, 50, 300);
    LOGI("[P2] done: %d", g_PatchOK.load());
}

static void BypassPhase3() {
    LOGI("=== P3: telemetry ===");
    int t = 0;
    if (stealth::GetLib("libhdmpvecore.so")) {
        std::vector<stealth::Patch> h(patches_hdmpvecore, patches_hdmpvecore + 381);
        t += g_Engine.applyGroup(h, 364, 50, 200);
    }
    if (stealth::GetLib("libhdmpve.so")) {
        std::vector<stealth::Patch> h(patches_hdmpve, patches_hdmpve + 27);
        t += g_Engine.applyGroup(h, 745, 100, 500);
    }
    if (stealth::GetLib("libTBlueData.so")) {
        std::vector<stealth::Patch> h(patches_TBlueData, patches_TBlueData + 36);
        t += g_Engine.applyGroup(h, 772, 100, 500);
    }
    g_PatchOK += t;
    LOGI("[P3] done: %d total", g_PatchOK.load());
}

// ===================== DIRECT DRAW (NO ProcessEvent) =====================
// K2_DrawLine native: We call ProcessEvent using raw vtable call
// ProcessEvent = VTable[76] on any UObject
// UFunction* for K2_DrawLine found by iterating GUObjectArray

static uintptr_t g_pK2DrawLine = 0; // cached UFunction*

struct DrawLineParams {
    FVector2D A;
    FVector2D B;
    float Thickness;
    FLinearColor Color;
};

static void CallDrawLine(uintptr_t canvas, FVector2D a, FVector2D b, float th, FLinearColor col) {
    if (!canvas || !g_pK2DrawLine) return;
    
    DrawLineParams params;
    params.A = a; params.B = b; params.Thickness = th; params.Color = col;
    
    // Set FUNC_Native flag on UFunction
    uint32_t flags = 0;
    Tools::PVM_ReadAddr((void*)(g_pK2DrawLine + 0xB0), &flags, sizeof(flags)); // FunctionFlags offset
    uint32_t newFlags = flags | 0x400;
    Tools::PVM_WriteAddr((void*)(g_pK2DrawLine + 0xB0), &newFlags, sizeof(newFlags));
    
    // Call ProcessEvent via vtable[76]
    void** vt = nullptr;
    Tools::PVM_ReadAddr((void*)canvas, &vt, sizeof(vt));
    if (!vt || !Tools::IsPtrValid(vt)) return;
    
    void* peFunc = nullptr;
    Tools::PVM_ReadAddr(&vt[76], &peFunc, sizeof(peFunc));
    if (!peFunc) return;
    
    // Direct call: ProcessEvent(this, UFunction*, params*)
    typedef void(*ProcessEventFn)(void*, void*, void*);
    ((ProcessEventFn)peFunc)((void*)canvas, (void*)g_pK2DrawLine, &params);
    
    // Restore flags
    Tools::PVM_WriteAddr((void*)(g_pK2DrawLine + 0xB0), &flags, sizeof(flags));
}

// Find UFunction by name substring
static uintptr_t FindFunctionByName(const char* search) {
    if (!UObject::GUObjectArray || !UObject::GNames) return 0;
    auto& objs = UObject::GetGlobalObjects();
    int max = objs.Num();
    if (max > 200000) max = 200000;
    
    for (int i = 0; i < max; i++) {
        auto obj = objs.GetByIndex(i);
        if (!obj || !Tools::IsPtrValid(obj)) continue;
        
        std::string name = obj->GetFullName();
        if (name.find(search) != std::string::npos) {
            LOGI("[+] Found '%s' at idx %d", search, i);
            return (uintptr_t)obj;
        }
    }
    return 0;
}

// W2S — mathematical (no ProcessEvent needed)
static bool WorldToScreen(FVector world, FVector camLoc, FRotator camRot, float fov, float sw, float sh, FVector2D& out) {
    FVector d = world - camLoc;
    float sp=sinf(camRot.Pitch*0.0174533f), cp=cosf(camRot.Pitch*0.0174533f);
    float sy=sinf(camRot.Yaw*0.0174533f), cy=cosf(camRot.Yaw*0.0174533f);
    FVector fwd{cp*cy, cp*sy, sp};
    FVector right{-(sinf(camRot.Roll*0.0174533f)*sp*cy - cosf(camRot.Roll*0.0174533f)*sy),
                  -(sinf(camRot.Roll*0.0174533f)*sp*sy + cosf(camRot.Roll*0.0174533f)*cy),
                  sinf(camRot.Roll*0.0174533f)*cp};
    FVector up{-(cosf(camRot.Roll*0.0174533f)*sp*cy + sinf(camRot.Roll*0.0174533f)*sy),
               cosf(camRot.Roll*0.0174533f)*(-sp*sy) + sinf(camRot.Roll*0.0174533f)*cy,
               cosf(camRot.Roll*0.0174533f)*cp};
    float w = d.X*fwd.X + d.Y*fwd.Y + d.Z*fwd.Z;
    if (w < 1.f) return false;
    float x = d.X*right.X + d.Y*right.Y + d.Z*right.Z;
    float y = d.X*up.X + d.Y*up.Y + d.Z*up.Z;
    float t = tanf(fov * 0.0174533f * 0.5f);
    if (t < 0.001f) return false;
    out.X = sw*0.5f + (x/w/t)*(sw*0.5f);
    out.Y = sh*0.5f - (y/w/t)*(sh*0.5f);
    return true;
}

// ===================== BONE/ESP =====================
struct FTransformLocal { float Rot[4], Trans[4], Scale[4]; };

static FVector GetBoneWorld(uintptr_t mesh, int idx) {
    FVector z{};
    if (!mesh || !Tools::IsPtrValid((void*)mesh)) return z;
    struct { FTransformLocal* Data; int32_t Count; int32_t Max; } bones{};
    Tools::PVM_ReadAddr((void*)(mesh + 0x600), &bones, sizeof(bones));
    if (!bones.Data || bones.Count<=0 || idx>=bones.Count) return z;
    if (!Tools::IsPtrValid(bones.Data)) return z;
    FTransformLocal bone{}, comp{};
    Tools::PVM_ReadAddr(&bones.Data[idx], &bone, sizeof(bone));
    Tools::PVM_ReadAddr((void*)(mesh + 0x1D0), &comp, sizeof(comp));
    return {comp.Trans[0]+bone.Trans[0], comp.Trans[1]+bone.Trans[1], comp.Trans[2]+bone.Trans[2]};
}

static float normA(float a){while(a>180)a-=360;while(a<-180)a+=360;return a;}

// ===================== POSTRENDER =====================
void*(*orig_PR)(void*,void*);
void* hook_PR(void* vp, void* canvas) {
    if (!canvas || !Tools::IsPtrValid(canvas) || !g_ESPReady.load()) 
        goto end;
    
    {
        // Read canvas size
        int32_t sw=0, sh=0;
        Tools::PVM_ReadAddr((void*)((uintptr_t)canvas + 0x44), &sw, sizeof(sw));
        Tools::PVM_ReadAddr((void*)((uintptr_t)canvas + 0x48), &sh, sizeof(sh));
        if (sw < 100 || sh < 100) goto end;
        
        // Status bar — green progress
        float progress = (float)g_PatchOK.load() / 808.0f;
        CallDrawLine((uintptr_t)canvas, {0,2}, {sw*progress,2}, 4.f, {0,1,0,0.7f});
        if (sw*progress < sw)
            CallDrawLine((uintptr_t)canvas, {sw*progress,2}, {(float)sw,2}, 4.f, {1,0,0,0.3f});
        
        if (!g_MyPC) goto end;
        
        // Camera
        uintptr_t camMgr=0;
        Tools::PVM_ReadAddr((void*)(g_MyPC + 0x548), &camMgr, sizeof(camMgr));
        if (!camMgr || !Tools::IsPtrValid((void*)camMgr)) goto end;
        
        // CameraCache.POV = camMgr + 0x530
        // FMinimalViewInfo: Location(+0), LocationLocalSpace(+0xC), Rotation(+0x18), FOV(+0x24)
        FVector camLoc{};
        FRotator camRot{};
        float fov = 90.f;
        Tools::PVM_ReadAddr((void*)(camMgr + 0x530), &camLoc, sizeof(camLoc));
        Tools::PVM_ReadAddr((void*)(camMgr + 0x530 + 0x18), &camRot, sizeof(camRot));
        Tools::PVM_ReadAddr((void*)(camMgr + 0x530 + 0x24), &fov, sizeof(fov));
        if (fov <= 0 || fov > 170) fov = 90.f;
        
        // My pawn
        uintptr_t myPawn=0;
        Tools::PVM_ReadAddr((void*)(g_MyPC + 0x528), &myPawn, sizeof(myPawn));
        if (!myPawn || !Tools::IsPtrValid((void*)myPawn)) goto end;
        
        int32_t myTeam=-1;
        Tools::PVM_ReadAddr((void*)(myPawn + 0x998), &myTeam, sizeof(myTeam));
        
        // World → Level → Actors (via function call)
        uintptr_t worldPtr = 0;
        // Get world from GameInstance chain: PC → 0x470 is wrong path
        // Better: directly read World from what we cached
        // Use GUObjectArray approach to find world
        uintptr_t gi=0;
        Tools::PVM_ReadAddr((void*)(g_MyPC + 0x528 - 0x528 + 0x470), &gi, sizeof(gi)); // Won't work
        // Actually get PersistentLevel from World pointer we already know
        // Let's get it from the outer chain: PlayerController → World
        // AController has WorldPrivate — but we need level
        // Simplest: use GetActorArray function on PersistentLevel from UWorld
        
        // We need UWorld. Get it via the GameInstance → World chain
        // PC→Outer is the Level, Level→Outer is World... no
        // PC→GetWorld() would need ProcessEvent
        // Let's just read world from GUObjectArray (cached in GetFullWorld)
        auto world = GetFullWorld();
        if (!world || !Tools::IsPtrValid(world)) goto end;
        
        uintptr_t level=0;
        Tools::PVM_ReadAddr((void*)((uintptr_t)world + 0x30), &level, sizeof(level));
        if (!level || !Tools::IsPtrValid((void*)level)) goto end;
        
        // GetActorArray function call
        typedef struct{uintptr_t base;int32_t count;int32_t max;}ActArr;
        typedef ActArr*(*GetActFn)(uintptr_t);
        auto fn = (GetActFn)(UE4 + Off::GetActorArray);
        ActArr* arr = fn((uintptr_t)level);
        if (!arr || arr->count<=0 || arr->count>50000) goto end;
        
        float bestAng=100; FVector bestTgt{}; bool hasTgt=false;
        int scan = arr->count < 200 ? arr->count : 200;
        FLinearColor red{1,0.3f,0.3f,1}, white{1,1,1,0.8f};
        
        for (int i=0; i<scan; i++) {
            uintptr_t actor=0;
            Tools::PVM_ReadAddr((void*)(arr->base+i*8), &actor, sizeof(actor));
            if (!actor || actor==myPawn || !Tools::IsPtrValid((void*)actor)) continue;
            
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
            if (!mesh || !Tools::IsPtrValid((void*)mesh)) continue;
            
            FVector head=GetBoneWorld(mesh,6), root=GetBoneWorld(mesh,0);
            if (head.Size()<1 || root.Size()<1) continue;
            
            FVector2D hs{},rs{};
            if (!WorldToScreen(head,camLoc,camRot,fov,(float)sw,(float)sh,hs)) continue;
            if (!WorldToScreen(root,camLoc,camRot,fov,(float)sw,(float)sh,rs)) continue;
            
            float bH=rs.Y-hs.Y;
            if (bH<5||bH>2000) continue;
            float bW=bH*0.45f, cx=(hs.X+rs.X)*0.5f;
            float cl=bH*0.15f, L=cx-bW/2, R=cx+bW/2;
            
            // Corner box
            CallDrawLine((uintptr_t)canvas,{L,hs.Y},{L+cl,hs.Y},1.5f,red);
            CallDrawLine((uintptr_t)canvas,{L,hs.Y},{L,hs.Y+cl},1.5f,red);
            CallDrawLine((uintptr_t)canvas,{R-cl,hs.Y},{R,hs.Y},1.5f,red);
            CallDrawLine((uintptr_t)canvas,{R,hs.Y},{R,hs.Y+cl},1.5f,red);
            CallDrawLine((uintptr_t)canvas,{L,rs.Y-cl},{L,rs.Y},1.5f,red);
            CallDrawLine((uintptr_t)canvas,{L,rs.Y},{L+cl,rs.Y},1.5f,red);
            CallDrawLine((uintptr_t)canvas,{R,rs.Y-cl},{R,rs.Y},1.5f,red);
            CallDrawLine((uintptr_t)canvas,{R-cl,rs.Y},{R,rs.Y},1.5f,red);
            
            // Skeleton
            int bones[][2]={{6,5},{5,4},{4,2},{2,0},{5,11},{11,12},{12,13},{5,32},{32,33},{33,34},{0,48},{48,49},{49,50},{0,53},{53,54},{54,55}};
            for(auto&b:bones){
                FVector ba=GetBoneWorld(mesh,b[0]),bb=GetBoneWorld(mesh,b[1]);
                if(ba.Size()<1||bb.Size()<1)continue;
                FVector2D sa{},sb{};
                if(WorldToScreen(ba,camLoc,camRot,fov,(float)sw,(float)sh,sa)&&
                   WorldToScreen(bb,camLoc,camRot,fov,(float)sw,(float)sh,sb))
                    CallDrawLine((uintptr_t)canvas,sa,sb,1.0f,white);
            }
            
            // Health bar
            float hpMax=0;
            Tools::PVM_ReadAddr((void*)(actor+0x0E64),&hpMax,sizeof(hpMax));
            if(hpMax>0){
                float ratio=hp/hpMax;if(ratio>1)ratio=1;
                float barX=L-5;
                CallDrawLine((uintptr_t)canvas,{barX,hs.Y},{barX,rs.Y},3.f,{0.1f,0.1f,0.1f,0.6f});
                float filled=hs.Y+bH*(1.f-ratio);
                CallDrawLine((uintptr_t)canvas,{barX,filled},{barX,rs.Y},2.f,{1-ratio,ratio,0,1});
            }
            
            // Aimbot scoring
            FVector d=head-camLoc;
            float hz=sqrtf(d.X*d.X+d.Y*d.Y);
            float yaw=atan2f(d.Y,d.X)*57.2957795f;
            float pitch=atan2f(d.Z,hz)*57.2957795f;
            float ang=sqrtf(normA(yaw-camRot.Yaw)*normA(yaw-camRot.Yaw)+normA(pitch-camRot.Pitch)*normA(pitch-camRot.Pitch));
            if(ang<bestAng){bestAng=ang;bestTgt=head;hasTgt=true;}
        }
        
        // Aimbot
        if(hasTgt&&bestAng<15.f){
            FRotator cur{};
            Tools::PVM_ReadAddr((void*)(g_MyPC+0x4E0),&cur,sizeof(cur));
            FVector d=bestTgt-camLoc;
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
                Tools::PVM_WriteAddr((void*)(g_MyPC+0x4E0),&nr,sizeof(nr));
            }
        }
    }
    
end:
    return orig_PR(vp, canvas);
}

// Hook via direct vtable swap on GameViewportClient
static bool DoHook() {
    // Find GameViewportClient via offset chain:
    // GUObjectArray → scan for UEngine → +0x810 = GameViewport
    auto& objs = UObject::GetGlobalObjects();
    int max = objs.Num();
    if (max > 200000) max = 200000;
    
    uintptr_t gvp = 0;
    
    // Method 1: Direct chain World → OwningGameInstance → ... 
    // Method 2: Scan for object named GameViewportClient
    for (int i = 0; i < max; i++) {
        auto obj = objs.GetByIndex(i);
        if (!obj || !Tools::IsPtrValid(obj)) continue;
        
        std::string name = obj->GetName();
        if (name == "GameViewportClient_1") {
            gvp = (uintptr_t)obj;
            LOGI("[+] GVP found at idx %d", i);
            break;
        }
    }
    
    if (!gvp) {
        LOGE("[-] GVP not found");
        return false;
    }
    
    // Read vtable pointer
    void** VT = nullptr;
    Tools::PVM_ReadAddr((void*)gvp, &VT, sizeof(VT));
    if (!VT || !Tools::IsPtrValid(VT)) return false;
    
    // Read original function at index 134
    void* origFunc = nullptr;
    Tools::PVM_ReadAddr(&VT[134], &origFunc, sizeof(origFunc));
    if (!origFunc) return false;
    orig_PR = (void*(*)(void*,void*))origFunc;
    
    // Swap vtable entry
    size_t ps = sysconf(_SC_PAGESIZE);
    void* page = (void*)((uintptr_t)&VT[134] & ~(ps-1));
    stealth::raw_mprotect(page, ps*2, PROT_READ|PROT_WRITE|PROT_EXEC);
    void* hookPtr = (void*)hook_PR;
    memcpy(&VT[134], &hookPtr, sizeof(void*));
    __builtin___clear_cache((char*)&VT[134], (char*)(&VT[134]+1));
    stealth::raw_mprotect(page, ps*2, PROT_READ|PROT_EXEC);
    
    LOGI("[+] Hooked VT[134]");
    return true;
}

// Find K2_DrawLine UFunction
static bool FindDrawFunction() {
    g_pK2DrawLine = FindFunctionByName("K2_DrawLine");
    if (g_pK2DrawLine) {
        LOGI("[+] K2_DrawLine = 0x%lx", (unsigned long)g_pK2DrawLine);
        return true;
    }
    LOGE("[-] K2_DrawLine not found");
    return false;
}

// ===================== MAIN =====================
static void* MainThread(void*) {
    LOGI("[*] v8.1 start");
    g_Engine.init();

    UE4 = waitLib("libUE4.so", 120000);
    if (!UE4) { LOGE("UE4 timeout"); return nullptr; }
    LOGI("[+] UE4=0x%lx", (unsigned long)UE4);

    // Wait for KRAFTON logo (10 sec)
    waitLib("libanogs.so", 60000);
    for (int i=0;i<100;i++) usleep(100000);
    
    // Phase 1: anogs
    BypassPhase1();

    // Init SDK
    GetGNamesFunc = (TNameEntryArray*(*)())(UE4 + Off::GNames);
    for (int i=0;i<60;i++) {
        UObject::GNames = GetGNamesFunc();
        if (UObject::GNames) break;
        usleep(500000);
    }
    if (!UObject::GNames){LOGE("GNames fail");return nullptr;}
    UObject::GUObjectArray = (FUObjectArray*)(UE4 + Off::GUObjectArray);
    LOGI("[+] SDK OK");

    // Wait for World
    UWorld* world = nullptr;
    for (int i=0;i<180;i++) {
        world = GetFullWorld();
        if (world && Tools::IsPtrValid(world)) break;
        usleep(2000000);
    }
    if (!world){LOGE("World timeout");return nullptr;}
    LOGI("[+] World");

    // Find PlayerController via offset chain
    for (int i=0;i<30;i++) {
        world = GetFullWorld();
        if(!world){usleep(2000000);continue;}
        uintptr_t gi=0;
        Tools::PVM_ReadAddr((void*)((uintptr_t)world+0x470),&gi,sizeof(gi));
        if(!gi||!Tools::IsPtrValid((void*)gi)){usleep(2000000);continue;}
        struct{void**Data;int32_t Count;int32_t Max;}lp{};
        Tools::PVM_ReadAddr((void*)(gi+0x48),&lp,sizeof(lp));
        if(!lp.Data||lp.Count<=0){usleep(2000000);continue;}
        uintptr_t localP=0;
        Tools::PVM_ReadAddr(&lp.Data[0],&localP,sizeof(localP));
        if(!localP||!Tools::IsPtrValid((void*)localP)){usleep(2000000);continue;}
        Tools::PVM_ReadAddr((void*)(localP+0x30),&g_MyPC,sizeof(g_MyPC));
        if(g_MyPC&&Tools::IsPtrValid((void*)g_MyPC)){LOGI("[+] PC=0x%lx",(unsigned long)g_MyPC);break;}
        usleep(2000000);
    }
    if(!g_MyPC){LOGE("PC fail");return nullptr;}

    // Find K2_DrawLine UFunction
    FindDrawFunction();

    // Hook PostRender
    for (int i=0;i<20;i++) {
        if (DoHook()) break;
        usleep(3000000);
    }
    
    g_ESPReady.store(true);
    LOGI("[+] ESP READY");
    
    // Phase 2: UE4 NOPs — 60 sec AFTER hook (deep in game)
    for (int i=0;i<600;i++) usleep(100000); // 60 sec
    BypassPhase2();
    
    // Phase 3: telemetry — another 60 sec later  
    for (int i=0;i<600;i++) usleep(100000); // 60 sec
    BypassPhase3();
    
    LOGI("[+] ALL PHASES COMPLETE: %d patches", g_PatchOK.load());
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
