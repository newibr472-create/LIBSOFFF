/*
 * features/esp.cpp v2.1 — SAFE ESP
 * 
 * KEY FIX: Don't call K2_DrawLine directly!
 * Instead, use Dobby-style hook on PostRender which already gives us Canvas.
 * For drawing, we use the Canvas VTable approach:
 *   Canvas->DrawItem calls go through virtual dispatch — SAFE.
 * 
 * Actually safest approach: Use Canvas->K2_DrawLine via ProcessEvent
 * But that's complex. Simplest SAFE approach:
 *   Read bone positions, do W2S, write to ControlRotation (aimbot)
 *   For ESP visual: hook HUD DrawRect/DrawLine via UFunction call
 * 
 * For now: ESP uses direct Canvas function call BUT with full validation
 * and crash guard (siglongjmp in main.cpp catches any crash)
 */
#include "esp.h"
#include "aimbot.h"
#include "../core/memory.h"
#include <cmath>
#include <android/log.h>

#define TAG "E"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

namespace esp {

Config cfg;

// ============================================================================
// Canvas drawing — validated function pointer call
// ============================================================================
static uintptr_t g_UE4 = 0;

// K2_DrawLine signature: void K2_DrawLine(UCanvas*, FVector2D A, FVector2D B, float Thick, FLinearColor C)
typedef void (*DrawLine_t)(void *, FVector2D, FVector2D, float, FLinearColor);
static DrawLine_t pDrawLine = nullptr;
static bool g_DrawValidated = false;

static bool ValidateDrawFunction() {
    if (!g_UE4) return false;
    uintptr_t funcAddr = g_UE4 + Off::K2_DrawLine;
    
    // Check if address is in libUE4 range
    uintptr_t ue4End = mem::GetEnd("libUE4.so");
    if (funcAddr < g_UE4 || funcAddr >= ue4End) return false;
    
    // Check if it looks like valid code (first 4 bytes readable)
    uint32_t firstInstr = 0;
    if (!mem::Read((void *)funcAddr, &firstInstr, 4)) return false;
    if (firstInstr == 0) return false; // NULL = bad
    
    pDrawLine = (DrawLine_t)funcAddr;
    return true;
}

static void Line(void *canvas, float x1, float y1, float x2, float y2, 
                 FLinearColor color, float thick = 1.0f) {
    if (!pDrawLine || !canvas) return;
    // Bounds check — don't draw outside screen
    if (x1 < -5000 || x1 > 5000 || y1 < -5000 || y1 > 5000) return;
    if (x2 < -5000 || x2 > 5000 || y2 < -5000 || y2 > 5000) return;
    pDrawLine(canvas, {x1, y1}, {x2, y2}, thick, color);
}

// ============================================================================
// World-to-Screen
// ============================================================================
struct CameraState {
    FVector location;
    FRotator rotation;
    float fov;
};

static CameraState g_Cam;
static float g_ScreenW = 0, g_ScreenH = 0;

static bool WorldToScreen(FVector world, FVector2D &screen) {
    FVector delta = world - g_Cam.location;
    
    float sp = sinf(g_Cam.rotation.Pitch * 0.0174533f);
    float cp = cosf(g_Cam.rotation.Pitch * 0.0174533f);
    float sy = sinf(g_Cam.rotation.Yaw   * 0.0174533f);
    float cy = cosf(g_Cam.rotation.Yaw   * 0.0174533f);
    float sr = sinf(g_Cam.rotation.Roll   * 0.0174533f);
    float cr = cosf(g_Cam.rotation.Roll   * 0.0174533f);
    
    FVector forward = {cp * cy, cp * sy, sp};
    FVector right   = {-(sr * sp * cy - cr * sy), -(sr * sp * sy + cr * cy), sr * cp};
    FVector up      = {-(cr * sp * cy + sr * sy), cy * sr - cr * sp * sy, cr * cp};
    
    float w = delta.X * forward.X + delta.Y * forward.Y + delta.Z * forward.Z;
    if (w < 1.0f) return false;
    
    float x = delta.X * right.X + delta.Y * right.Y + delta.Z * right.Z;
    float y = delta.X * up.X + delta.Y * up.Y + delta.Z * up.Z;
    
    float fovRad = g_Cam.fov * 0.0174533f * 0.5f;
    float tanFov = tanf(fovRad);
    if (tanFov < 0.001f) return false;
    
    screen.X = (g_ScreenW * 0.5f) + (x / w / tanFov) * (g_ScreenW * 0.5f);
    screen.Y = (g_ScreenH * 0.5f) - (y / w / tanFov) * (g_ScreenH * 0.5f);
    
    // Sanity check
    if (screen.X < -500 || screen.X > g_ScreenW + 500) return false;
    if (screen.Y < -500 || screen.Y > g_ScreenH + 500) return false;
    
    return true;
}

// ============================================================================
// Bone position
// ============================================================================
static FVector GetBoneWorldPos(uintptr_t mesh, int boneIdx) {
    FVector zero{};
    if (!mesh || !mem::IsValid((void *)mesh)) return zero;
    
    uintptr_t boneArrayPtr = mesh + Off::BoneArray;
    TArray<FTransform> bones;
    if (!mem::Read((void *)boneArrayPtr, &bones, sizeof(bones))) return zero;
    if (!bones.IsValid() || boneIdx >= bones.Count || boneIdx < 0) return zero;
    if (!mem::IsValid(bones.Data)) return zero;
    
    FTransform transform;
    if (!mem::Read(&bones.Data[boneIdx], &transform, sizeof(transform))) return zero;
    
    FTransform c2w;
    if (!mem::Read((void *)(mesh + Off::ComponentToWorld), &c2w, sizeof(c2w))) return zero;
    
    FVector boneLocal = transform.GetLocation();
    FVector compWorld = c2w.GetLocation();
    
    // Sanity: positions shouldn't be astronomical
    FVector result = compWorld + boneLocal;
    if (fabsf(result.X) > 500000 || fabsf(result.Y) > 500000 || fabsf(result.Z) > 500000) return zero;
    
    return result;
}

// ============================================================================
// Draw corner box
// ============================================================================
static void DrawBox(void *canvas, FVector2D top, FVector2D bottom, FLinearColor color) {
    float h = bottom.Y - top.Y;
    if (h < 5.0f || h > 2000.0f) return; // Sanity
    float w = h * 0.4f;
    float cx = (top.X + bottom.X) * 0.5f;
    
    float left = cx - w * 0.5f;
    float right_x = cx + w * 0.5f;
    float cornerLen = h * 0.15f;
    
    Line(canvas, left, top.Y, left + cornerLen, top.Y, color, 1.5f);
    Line(canvas, left, top.Y, left, top.Y + cornerLen, color, 1.5f);
    Line(canvas, right_x - cornerLen, top.Y, right_x, top.Y, color, 1.5f);
    Line(canvas, right_x, top.Y, right_x, top.Y + cornerLen, color, 1.5f);
    Line(canvas, left, bottom.Y - cornerLen, left, bottom.Y, color, 1.5f);
    Line(canvas, left, bottom.Y, left + cornerLen, bottom.Y, color, 1.5f);
    Line(canvas, right_x, bottom.Y - cornerLen, right_x, bottom.Y, color, 1.5f);
    Line(canvas, right_x - cornerLen, bottom.Y, right_x, bottom.Y, color, 1.5f);
}

// ============================================================================
// Draw health bar
// ============================================================================
static void DrawHealthBar(void *canvas, FVector2D top, FVector2D bottom, float hp, float maxHp) {
    if (maxHp <= 0) return;
    float ratio = hp / maxHp;
    if (ratio > 1.0f) ratio = 1.0f;
    if (ratio < 0.0f) ratio = 0.0f;
    
    float h = bottom.Y - top.Y;
    float w = h * 0.4f;
    float cx = (top.X + bottom.X) * 0.5f;
    float left = cx - w * 0.5f;
    float barX = left - 5.0f;
    float filled = top.Y + h * (1.0f - ratio);
    
    Line(canvas, barX, top.Y, barX, bottom.Y, {0.1f, 0.1f, 0.1f, 0.6f}, 3.0f);
    FLinearColor hpColor = {1.0f - ratio, ratio, 0.0f, 1.0f};
    Line(canvas, barX, filled, barX, bottom.Y, hpColor, 2.0f);
}

// ============================================================================
// Draw skeleton
// ============================================================================
enum Bone : int {
    Head = 6, Neck = 5, Spine2 = 4, Spine1 = 2, Pelvis = 0,
    LShoulder = 11, LElbow = 12, LWrist = 13,
    RShoulder = 32, RElbow = 33, RWrist = 34,
    LThigh = 48, LKnee = 49, LAnkle = 50,
    RThigh = 53, RKnee = 54, RAnkle = 55
};

struct BonePair { int from; int to; };
static const BonePair g_Skeleton[] = {
    {Head, Neck}, {Neck, Spine2}, {Spine2, Spine1}, {Spine1, Pelvis},
    {Neck, LShoulder}, {LShoulder, LElbow}, {LElbow, LWrist},
    {Neck, RShoulder}, {RShoulder, RElbow}, {RElbow, RWrist},
    {Pelvis, LThigh}, {LThigh, LKnee}, {LKnee, LAnkle},
    {Pelvis, RThigh}, {RThigh, RKnee}, {RKnee, RAnkle}
};

static void DrawSkeleton(void *canvas, uintptr_t mesh, FLinearColor color) {
    for (const auto &bone : g_Skeleton) {
        FVector a = GetBoneWorldPos(mesh, bone.from);
        FVector b = GetBoneWorldPos(mesh, bone.to);
        if (a.Length() < 1.0f || b.Length() < 1.0f) continue;
        
        FVector2D sa, sb;
        if (!WorldToScreen(a, sa) || !WorldToScreen(b, sb)) continue;
        Line(canvas, sa.X, sa.Y, sb.X, sb.Y, color, 1.0f);
    }
}

// ============================================================================
// Main render
// ============================================================================
void Render(uintptr_t canvas, uintptr_t ue4Base) {
    if (!cfg.enabled) return;
    if (!canvas || !ue4Base) return;
    
    g_UE4 = ue4Base;
    
    // Validate draw function ONCE
    if (!g_DrawValidated) {
        if (!ValidateDrawFunction()) return; // Can't draw yet
        g_DrawValidated = true;
        LOGI("[+] K2_DrawLine validated");
    }
    
    // Get screen size
    int32_t sw = 0, sh = 0;
    mem::Read((void *)(canvas + Off::SizeX), &sw, sizeof(int32_t));
    mem::Read((void *)(canvas + Off::SizeY), &sh, sizeof(int32_t));
    g_ScreenW = (float)sw;
    g_ScreenH = (float)sh;
    if (g_ScreenW < 100 || g_ScreenH < 100) return; // Not ready
    if (g_ScreenW > 4000 || g_ScreenH > 4000) return; // Sanity
    
    // Get camera
    uintptr_t pWorld = 0;
    mem::Read((void *)(ue4Base + Off::GWorld), &pWorld, sizeof(uintptr_t));
    if (!pWorld || !mem::IsValid((void *)pWorld)) return;
    
    uintptr_t gameInst = 0;
    mem::Read((void *)(pWorld + Off::OwningGameInstance), &gameInst, sizeof(uintptr_t));
    if (!gameInst || !mem::IsValid((void *)gameInst)) return;
    
    TArray<uintptr_t> localPlayers;
    mem::Read((void *)(gameInst + Off::LocalPlayers), &localPlayers, sizeof(localPlayers));
    if (!localPlayers.IsValid()) return;
    
    uintptr_t localPlayer = 0;
    mem::Read(&localPlayers.Data[0], &localPlayer, sizeof(uintptr_t));
    if (!localPlayer || !mem::IsValid((void *)localPlayer)) return;
    
    uintptr_t myPC = 0;
    mem::Read((void *)(localPlayer + Off::PlayerController), &myPC, sizeof(uintptr_t));
    if (!myPC || !mem::IsValid((void *)myPC)) return;
    
    uintptr_t camMgr = 0;
    mem::Read((void *)(myPC + Off::PlayerCameraMgr), &camMgr, sizeof(uintptr_t));
    if (!camMgr || !mem::IsValid((void *)camMgr)) return;
    
    // Read camera
    FMinimalViewInfo pov;
    mem::Read((void *)(camMgr + Off::CameraCachePOV), &pov, sizeof(pov));
    g_Cam.location = pov.Location;
    g_Cam.rotation = pov.Rotation;
    g_Cam.fov = pov.FOV;
    if (g_Cam.fov <= 0 || g_Cam.fov > 170) g_Cam.fov = 90.0f;
    
    // Get my pawn
    uintptr_t myPawn = 0;
    mem::Read((void *)(myPC + Off::AcknowledgedPawn), &myPawn, sizeof(uintptr_t));
    if (!myPawn || !mem::IsValid((void *)myPawn)) return;
    
    // My team
    int32_t myTeam = -1;
    mem::Read((void *)(myPawn + Off::TeamID), &myTeam, sizeof(int32_t));
    
    // My position
    FVector myLoc{};
    uintptr_t myRoot = 0;
    mem::Read((void *)(myPawn + Off::RootComponent), &myRoot, sizeof(uintptr_t));
    if (myRoot && mem::IsValid((void *)myRoot)) {
        mem::Read((void *)(myRoot + Off::RelativeLocation), &myLoc, sizeof(FVector));
    }
    
    // Get actors from level
    uintptr_t level = 0;
    mem::Read((void *)(pWorld + Off::PersistentLevel), &level, sizeof(uintptr_t));
    if (!level || !mem::IsValid((void *)level)) return;
    
    TArray<uintptr_t> actors;
    mem::Read((void *)(level + Off::Actors), &actors, sizeof(actors));
    if (!actors.IsValid() || actors.Count > 100000 || actors.Count < 1) return;
    if (!mem::IsValid(actors.Data)) return;
    
    // Colors
    FLinearColor cEnemy = {1.0f, 0.3f, 0.3f, 1.0f};
    FLinearColor cBone  = {0.9f, 0.9f, 0.9f, 0.8f};
    
    // Reset aimbot target
    aimbot::g_ClosestTarget = 0;
    aimbot::g_ClosestDist = 99999.0f;
    
    // Scan actors (limit to prevent frame drops)
    int maxScan = actors.Count < 300 ? actors.Count : 300;
    
    for (int i = 0; i < maxScan; i++) {
        uintptr_t actor = 0;
        mem::Read(&actors.Data[i], &actor, sizeof(uintptr_t));
        if (!actor || !mem::IsValid((void *)actor)) continue;
        
        // Quick filter: TeamID must be valid and different from ours
        int32_t teamId = -1;
        mem::Read((void *)(actor + Off::TeamID), &teamId, sizeof(int32_t));
        if (teamId < 0 || teamId > 200) continue;
        if (teamId == myTeam) continue;
        
        // Check alive
        uint8_t dead = 0;
        mem::Read((void *)(actor + Off::bDead), &dead, sizeof(uint8_t));
        if (dead) continue;
        
        float hp = 0, hpMax = 0;
        mem::Read((void *)(actor + Off::Health), &hp, sizeof(float));
        mem::Read((void *)(actor + Off::HealthMax), &hpMax, sizeof(float));
        if (hp <= 0 || hpMax <= 0) continue;
        
        // Get mesh
        uintptr_t mesh = 0;
        mem::Read((void *)(actor + Off::Mesh), &mesh, sizeof(uintptr_t));
        if (!mesh || !mem::IsValid((void *)mesh)) continue;
        
        // Head & root
        FVector headPos = GetBoneWorldPos(mesh, Bone::Head);
        FVector rootPos = GetBoneWorldPos(mesh, Bone::Pelvis);
        if (headPos.Length() < 1.0f || rootPos.Length() < 1.0f) continue;
        
        // Distance
        float dist = (headPos - myLoc).Length() / 100.0f;
        if (dist > cfg.maxDist || dist < 0.5f) continue;
        
        // W2S
        FVector2D headScreen, rootScreen;
        if (!WorldToScreen(headPos, headScreen) || !WorldToScreen(rootPos, rootScreen)) continue;
        
        FVector2D top = headScreen;
        top.Y -= 10.0f;
        FVector2D bottom = rootScreen;
        
        // Draw
        if (cfg.box) DrawBox((void *)canvas, top, bottom, cEnemy);
        if (cfg.skeleton) DrawSkeleton((void *)canvas, mesh, cBone);
        if (cfg.health) DrawHealthBar((void *)canvas, top, bottom, hp, hpMax);
        
        // Aimbot tracking
        float screenDist = sqrtf(
            (headScreen.X - g_ScreenW * 0.5f) * (headScreen.X - g_ScreenW * 0.5f) +
            (headScreen.Y - g_ScreenH * 0.5f) * (headScreen.Y - g_ScreenH * 0.5f));
        
        if (screenDist < aimbot::g_ClosestDist) {
            aimbot::g_ClosestDist = screenDist;
            aimbot::g_ClosestTarget = actor;
            aimbot::g_TargetHead = headPos;
        }
    }
}

} // namespace esp
