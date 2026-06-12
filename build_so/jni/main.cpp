// BGMI 4.4.0 — v4: Bypass (no UE4 NOPs) + ESP + Aimbot + Large Hitbox
// Corrected offsets from NIKON SDK dump
#include <pthread.h>
#include <unistd.h>
#include <android/log.h>
#include <cmath>
#include <cstring>
#include "core/memory.h"
#include "core/bypass.h"
#include "sdk/types.h"

#define TAG "M"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

static uintptr_t g_UE4 = 0;

// PRNG
static uint32_t rngS = 0xCAFE;
static float rng(float a, float b) {
    rngS ^= rngS<<13; rngS ^= rngS>>17; rngS ^= rngS<<5;
    return a + ((float)(rngS&0xFFFF)/65535.f)*(b-a);
}

static float normA(float a) { while(a>180)a-=360; while(a<-180)a+=360; return a; }

// Bone world pos
static FVector boneWorld(uintptr_t mesh, int idx) {
    FVector z{};
    if (!mesh || !mem::IsValid((void*)mesh)) return z;
    TArray<FTransform> bones;
    if (!mem::Read((void*)(mesh + Off::BoneArray), &bones, sizeof(bones))) return z;
    if (!bones.IsValid() || idx >= bones.Count || idx < 0) return z;
    if (!mem::IsValid(bones.Data)) return z;
    FTransform t, c;
    if (!mem::Read(&bones.Data[idx], &t, sizeof(t))) return z;
    if (!mem::Read((void*)(mesh + Off::Comp_ComponentToWorld), &c, sizeof(c))) return z;
    FVector r = c.GetLocation() + t.GetLocation();
    if (__builtin_fabsf(r.X)>500000) return z;
    return r;
}

// W2S
static float gW, gH;
static FVector gCamLoc;
static FRotator gCamRot;
static float gFov;

static bool w2s(FVector world, float &sx, float &sy) {
    FVector d = world - gCamLoc;
    float sp=sinf(gCamRot.Pitch*0.0174533f), cp=cosf(gCamRot.Pitch*0.0174533f);
    float sy_=sinf(gCamRot.Yaw*0.0174533f), cy=cosf(gCamRot.Yaw*0.0174533f);
    float cr=cosf(gCamRot.Roll*0.0174533f), sr=sinf(gCamRot.Roll*0.0174533f);
    FVector fwd={cp*cy, cp*sy_, sp};
    FVector right={-(sr*sp*cy-cr*sy_), -(sr*sp*sy_+cr*cy), sr*cp};
    FVector up={-(cr*sp*cy+sr*sy_), cy*sr-cr*sp*sy_, cr*cp};
    float w = d.X*fwd.X + d.Y*fwd.Y + d.Z*fwd.Z;
    if (w < 1.f) return false;
    float x = d.X*right.X + d.Y*right.Y + d.Z*right.Z;
    float y = d.X*up.X + d.Y*up.Y + d.Z*up.Z;
    float t = tanf(gFov*0.0174533f*0.5f);
    if (t < 0.001f) return false;
    sx = gW*0.5f + (x/w/t)*(gW*0.5f);
    sy = gH*0.5f - (y/w/t)*(gH*0.5f);
    return (sx > -500 && sx < gW+500 && sy > -500 && sy < gH+500);
}

// K2_DrawLine
typedef void(*DrawLine_t)(void*, FVector2D, FVector2D, float, FLinearColor);
static DrawLine_t pDraw = nullptr;

static void line(void* c, float x1, float y1, float x2, float y2, FLinearColor col, float th=1.f) {
    if (!pDraw||!c) return;
    pDraw(c, {x1,y1}, {x2,y2}, th, col);
}

// ESP + Aimbot in PostRender
typedef void*(*PostRender_t)(void*, void*);
static PostRender_t gOrig = nullptr;
static void** gShadow = nullptr;
static int gFrames = 0;
static uintptr_t gLastWorld = 0;
static int gWorldAge = 0;

// Aimbot state
static FVector gAimTarget{};
static bool gHasTarget = false;

static void* hookPR(void* vp, void* canvas) {
    gFrames++;
    if (!canvas || !mem::IsValid(canvas) || gFrames < 120) goto end;
    
    {
        // World stability check
        uintptr_t pw = 0;
        mem::Read((void*)(g_UE4 + Off::GWorld), &pw, sizeof(uintptr_t));
        if (pw != gLastWorld) { gLastWorld = pw; gWorldAge = 0; goto end; }
        gWorldAge++;
        if (gWorldAge < 300) goto end;
        if (!pw || !mem::IsValid((void*)pw)) goto end;

        // Setup draw
        if (!pDraw) {
            uintptr_t addr = g_UE4 + 0x8DEE10C; // K2_DrawLine
            if (mem::IsValid((void*)addr)) pDraw = (DrawLine_t)addr;
            else goto end;
        }

        // Screen size
        int32_t sw=0, sh=0;
        mem::Read((void*)((uintptr_t)canvas + Off::Canvas_SizeX), &sw, sizeof(int32_t));
        mem::Read((void*)((uintptr_t)canvas + Off::Canvas_SizeY), &sh, sizeof(int32_t));
        gW = (float)sw; gH = (float)sh;
        if (gW < 100 || gH < 100) goto end;

        // Camera: World → GameInstance → LocalPlayers[0] → PC → CamMgr
        uintptr_t gi=0;
        mem::Read((void*)(pw + Off::World_OwningGameInstance), &gi, sizeof(uintptr_t));
        if (!gi || !mem::IsValid((void*)gi)) goto end;

        TArray<uintptr_t> lp;
        mem::Read((void*)(gi + Off::GI_LocalPlayers), &lp, sizeof(lp));
        if (!lp.IsValid() || !mem::IsValid(lp.Data)) goto end;

        uintptr_t localP=0;
        mem::Read(&lp.Data[0], &localP, sizeof(uintptr_t));
        if (!localP || !mem::IsValid((void*)localP)) goto end;

        uintptr_t myPC=0;
        mem::Read((void*)(localP + Off::Player_PlayerController), &myPC, sizeof(uintptr_t));
        if (!myPC || !mem::IsValid((void*)myPC)) goto end;

        uintptr_t camMgr=0;
        mem::Read((void*)(myPC + Off::PC_PlayerCameraManager), &camMgr, sizeof(uintptr_t));
        if (!camMgr || !mem::IsValid((void*)camMgr)) goto end;

        // Read camera (CameraCachePOV = 0x0530)
        FMinimalViewInfo pov;
        mem::Read((void*)(camMgr + Off::CamMgr_CachePOV), &pov, sizeof(pov));
        gCamLoc = pov.Location; gCamRot = pov.Rotation; gFov = pov.FOV;
        if (gFov <= 0 || gFov > 170) gFov = 90.f;

        // My pawn
        uintptr_t myPawn=0;
        mem::Read((void*)(myPC + Off::PC_AcknowledgedPawn), &myPawn, sizeof(uintptr_t));
        if (!myPawn || !mem::IsValid((void*)myPawn)) goto end;

        int32_t myTeam=-1;
        mem::Read((void*)(myPawn + Off::TeamID), &myTeam, sizeof(int32_t));

        FVector myLoc{};
        uintptr_t myRoot=0;
        mem::Read((void*)(myPawn + Off::Actor_RootComponent), &myRoot, sizeof(uintptr_t));
        if (myRoot && mem::IsValid((void*)myRoot))
            mem::Read((void*)(myRoot + Off::Comp_RelativeLocation), &myLoc, sizeof(FVector));

        // Actors
        uintptr_t level=0;
        mem::Read((void*)(pw + Off::World_PersistentLevel), &level, sizeof(uintptr_t));
        if (!level || !mem::IsValid((void*)level)) goto end;

        TArray<uintptr_t> actors;
        mem::Read((void*)(level + Off::Level_Actors), &actors, sizeof(actors));
        if (!actors.IsValid() || actors.Count > 50000 || !mem::IsValid(actors.Data)) goto end;

        FLinearColor cBox={1,0.3f,0.3f,1}, cBone={0.9f,0.9f,0.9f,0.8f};
        float bestAng = 100.f;
        gHasTarget = false;

        int scan = actors.Count < 200 ? actors.Count : 200;
        for (int i = 0; i < scan; i++) {
            uintptr_t a=0;
            mem::Read(&actors.Data[i], &a, sizeof(uintptr_t));
            if (!a || !mem::IsValid((void*)a)) continue;

            int32_t team=-1;
            mem::Read((void*)(a + Off::TeamID), &team, sizeof(int32_t));
            if (team<0||team>200||team==myTeam) continue;

            uint8_t dead=0;
            mem::Read((void*)(a + Off::bDead), &dead, 1);
            if (dead) continue;

            float hp=0;
            mem::Read((void*)(a + Off::Health), &hp, sizeof(float));
            if (hp <= 0) continue;

            uintptr_t mesh=0;
            mem::Read((void*)(a + Off::Char_Mesh), &mesh, sizeof(uintptr_t));
            if (!mesh || !mem::IsValid((void*)mesh)) continue;

            FVector head = boneWorld(mesh, 6);
            FVector root = boneWorld(mesh, 0);
            if (head.Length()<1 || root.Length()<1) continue;

            float dist = (head - myLoc).Length() / 100.f;
            if (dist > 350.f || dist < 0.5f) continue;

            float hsx, hsy, rsx, rsy;
            if (!w2s(head, hsx, hsy) || !w2s(root, rsx, rsy)) continue;

            // Corner box
            float h = rsy - hsy; if (h<5||h>2000) continue;
            float w_= h*0.4f, cx=(hsx+rsx)*0.5f;
            float L=cx-w_*0.5f, R=cx+w_*0.5f, cl=h*0.15f;
            line(canvas,L,hsy,L+cl,hsy,cBox,1.5f);
            line(canvas,L,hsy,L,hsy+cl,cBox,1.5f);
            line(canvas,R-cl,hsy,R,hsy,cBox,1.5f);
            line(canvas,R,hsy,R,hsy+cl,cBox,1.5f);
            line(canvas,L,rsy-cl,L,rsy,cBox,1.5f);
            line(canvas,L,rsy,L+cl,rsy,cBox,1.5f);
            line(canvas,R,rsy-cl,R,rsy,cBox,1.5f);
            line(canvas,R-cl,rsy,R,rsy,cBox,1.5f);

            // Skeleton (simplified: head-neck-spine-pelvis + arms + legs)
            int bones[][2] = {{6,5},{5,4},{4,2},{2,0},{5,11},{11,12},{12,13},{5,32},{32,33},{33,34},{0,48},{48,49},{49,50},{0,53},{53,54},{54,55}};
            for (auto& b : bones) {
                FVector ba = boneWorld(mesh, b[0]), bb = boneWorld(mesh, b[1]);
                if (ba.Length()<1||bb.Length()<1) continue;
                float ax,ay,bx,by;
                if (w2s(ba,ax,ay) && w2s(bb,bx,by)) line(canvas,ax,ay,bx,by,cBone,1.f);
            }

            // Health bar
            float hpMax=0;
            mem::Read((void*)(a + Off::HealthMax), &hpMax, sizeof(float));
            if (hpMax > 0) {
                float ratio = hp/hpMax; if(ratio>1)ratio=1;
                float barX = L - 5.f;
                line(canvas,barX,hsy,barX,rsy,{0.1f,0.1f,0.1f,0.6f},3.f);
                float filled = hsy + h*(1.f-ratio);
                line(canvas,barX,filled,barX,rsy,{1.f-ratio,ratio,0,1},2.f);
            }

            // Aimbot target selection (angular distance)
            FVector d2 = head - gCamLoc;
            float hz = sqrtf(d2.X*d2.X+d2.Y*d2.Y);
            float yaw = atan2f(d2.Y,d2.X)*57.2957795f;
            float pitch = atan2f(d2.Z,hz)*57.2957795f;
            float ang = sqrtf(normA(yaw-gCamRot.Yaw)*normA(yaw-gCamRot.Yaw) + normA(pitch-gCamRot.Pitch)*normA(pitch-gCamRot.Pitch));
            if (ang < bestAng) { bestAng=ang; gAimTarget=head; gHasTarget=true; }
        }

        // Aimbot apply
        if (gHasTarget && bestAng < 15.f) {
            FRotator cur;
            mem::Read((void*)(myPC + Off::ControlRotation), &cur, sizeof(FRotator));
            FVector d3 = gAimTarget - gCamLoc;
            float hz2 = sqrtf(d3.X*d3.X+d3.Y*d3.Y);
            float dP = normA(atan2f(d3.Z,hz2)*57.2957795f - cur.Pitch);
            float dY = normA(atan2f(d3.Y,d3.X)*57.2957795f - cur.Yaw);
            float err = sqrtf(dP*dP+dY*dY);
            if (err > 0.1f) {
                float factor = 1.f - powf(1.f-(err>30.f?1.f:err/30.f), 2.5f);
                float sm = 6.f * (0.3f + 0.7f*(1.f-factor));
                if (sm<1.5f) sm=1.5f;
                FRotator nr;
                nr.Pitch = normA(cur.Pitch + dP/sm + rng(-0.3f,0.3f));
                nr.Yaw = normA(cur.Yaw + dY/sm + rng(-0.3f,0.3f));
                nr.Roll = 0;
                if (nr.Pitch>89) nr.Pitch=89; if (nr.Pitch<-89) nr.Pitch=-89;
                mem::Write((void*)(myPC + Off::ControlRotation), &nr, sizeof(FRotator));
            }
        }

        // FOV circle
        float cx2=gW/2, cy2=gH/2, r=100.f;
        for (int i=0;i<36;i++) {
            float a1=6.2832f*i/36, a2=6.2832f*(i+1)/36;
            line(canvas,cx2+cosf(a1)*r,cy2+sinf(a1)*r,cx2+cosf(a2)*r,cy2+sinf(a2)*r,{1,1,1,0.3f},0.5f);
        }
    }

end:
    if (gOrig) return gOrig(vp, canvas);
    return nullptr;
}

// Install shadow vtable hook
static bool installHook() {
    uintptr_t pw=0;
    mem::Read((void*)(g_UE4 + Off::GWorld), &pw, sizeof(uintptr_t));
    if (!pw || !mem::IsValid((void*)pw)) return false;

    uintptr_t gi=0;
    mem::Read((void*)(pw + Off::World_OwningGameInstance), &gi, sizeof(uintptr_t));
    if (!gi || !mem::IsValid((void*)gi)) return false;

    TArray<uintptr_t> lp;
    mem::Read((void*)(gi + Off::GI_LocalPlayers), &lp, sizeof(lp));
    if (!lp.IsValid()) return false;

    uintptr_t localP=0;
    mem::Read(&lp.Data[0], &localP, sizeof(uintptr_t));
    if (!localP || !mem::IsValid((void*)localP)) return false;

    // ULocalPlayer::ViewportClient = 0x0058
    uintptr_t vpc=0;
    mem::Read((void*)(localP + Off::LocalPlayer_ViewportClient), &vpc, sizeof(uintptr_t));
    if (!vpc || !mem::IsValid((void*)vpc)) return false;

    void** origVT=nullptr;
    mem::Read((void*)vpc, &origVT, sizeof(void**));
    if (!origVT || !mem::IsValid(origVT)) return false;

    gShadow = mem::CloneVTable(origVT, 256);
    if (!gShadow) return false;

    gOrig = (PostRender_t)gShadow[134];
    gShadow[134] = (void*)hookPR;
    mem::SwapVTable((void*)vpc, gShadow);

    LOGI("[+] Hooked VT[134]");
    return true;
}

static void *worker(void *) {
    LOGI("[*] v4 start");

    while (!mem::GetBase("libUE4.so")) usleep(500000);
    g_UE4 = mem::GetBase("libUE4.so");
    LOGI("[+] UE4=0x%lx", (unsigned long)g_UE4);

    while (!mem::GetBase("libanogs.so")) usleep(500000);
    for (int i=0;i<10;i++) { if(mem::GetBase("libanort.so")) break; usleep(500000); }

    sleep(12);
    bypass::Apply();
    LOGI("[+] Bypass done");

    // Wait for world
    for (int i=0;i<180;i++) {
        uintptr_t pw=0;
        mem::Read((void*)(g_UE4 + Off::GWorld), &pw, sizeof(uintptr_t));
        if (pw && mem::IsValid((void*)pw)) { LOGI("[+] World up"); break; }
        sleep(1);
    }

    sleep(40);

    // Hook
    for (int i=0;i<20;i++) {
        if (installHook()) { LOGI("[+] ACTIVE"); break; }
        sleep(3);
    }
    return nullptr;
}

__attribute__((constructor))
void __attribute__((used)) entry() {
    pthread_t t;
    pthread_attr_t a;
    pthread_attr_init(&a);
    pthread_attr_setdetachstate(&a, PTHREAD_CREATE_DETACHED);
    pthread_create(&t, &a, worker, nullptr);
    pthread_attr_destroy(&a);
}
