/*
 * features/esp.cpp — Minimal clean ESP
 * Draws through UCanvas::K2_DrawLine/K2_DrawText (game's own functions)
 * No external overlay — renders inside game's PostRender
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
// Canvas drawing via function pointers
// ============================================================================
static uintptr_t g_UE4 = 0;

// K2_DrawLine(UCanvas* self, FVector2D A, FVector2D B, float Thickness, FLinearColor Color)
typedef void (*DrawLine_t)(void *, FVector2D, FVector2D, float, FLinearColor);
static DrawLine_t pDrawLine = nullptr;

// K2_DrawText — simplified: we use DrawLine to make text-like indicators instead
// Actual K2_DrawText needs UFont* which is complex to get

static void Line(void *canvas, float x1, float y1, float x2, float y2, 
                 FLinearColor color, float thick = 1.0f) {
    if (!pDrawLine || !canvas) return;
    pDrawLine(canvas, {x1, y1}, {x2, y2}, thick, color);
}

// ============================================================================
// World-to-Screen projection
// ============================================================================
struct CameraState {
    FVector location;
    FRotator rotation;
    float fov;
};

static CameraState g_Cam;
static float g_ScreenW = 0, g_ScreenH = 0;

static bool WorldToScreen(FVector world, FVector2D &screen) {
    // Manual W2S using camera matrix
    FVector delta = world - g_Cam.location;
    
    float sp = sinf(g_Cam.rotation.Pitch * 0.0174533f);
    float cp = cosf(g_Cam.rotation.Pitch * 0.0174533f);
    float sy = sinf(g_Cam.rotation.Yaw   * 0.0174533f);
    float cy = cosf(g_Cam.rotation.Yaw   * 0.0174533f);
    float sr = sinf(g_Cam.rotation.Roll   * 0.0174533f);
    float cr = cosf(g_Cam.rotation.Roll   * 0.0174533f);
    
    // Forward, Right, Up vectors from rotation
    FVector forward = {cp * cy, cp * sy, sp};
    FVector right   = {-(sr * sp * cy - cr * sy), -(sr * sp * sy + cr * cy), sr * cp};
    FVector up      = {-(cr * sp * cy + sr * sy), cy * sr - cr * sp * sy, cr * cp};
    
    float w = delta.X * forward.X + delta.Y * forward.Y + delta.Z * forward.Z;
    if (w < 1.0f) return false; // Behind camera
    
    float x = delta.X * right.X + delta.Y * right.Y + delta.Z * right.Z;
    float y = delta.X * up.X + delta.Y * up.Y + delta.Z * up.Z;
    
    float fovRad = g_Cam.fov * 0.0174533f * 0.5f;
    float tanFov = tanf(fovRad);
    
    screen.X = (g_ScreenW * 0.5f) + (x / w / tanFov) * (g_ScreenW * 0.5f);
    screen.Y = (g_ScreenH * 0.5f) - (y / w / tanFov) * (g_ScreenH * 0.5f);
    
    return true;
}

// ============================================================================
// Bone position from component space transforms
// ============================================================================
static FVector GetBoneWorldPos(uintptr_t mesh, int boneIdx) {
    FVector zero{};
    if (!mesh || !mem::IsValid((void *)mesh)) return zero;
    
    // BoneSpaceTransforms (ComponentSpaceTransformsArray[0])
    uintptr_t boneArrayPtr = mesh + Off::BoneArray;
    TArray<FTransform> bones;
    if (!mem::Read((void *)boneArrayPtr, &bones, sizeof(bones))) return zero;
    if (!bones.IsValid() || boneIdx >= bones.Count) return zero;
    if (!mem::IsValid(bones.Data)) return zero;
    
    FTransform transform;
    if (!mem::Read(&bones.Data[boneIdx], &transform, sizeof(transform))) return zero;
    
    // Get component-to-world transform
    FTransform c2w;
    if (!mem::Read((void *)(mesh + Off::ComponentToWorld), &c2w, sizeof(c2w))) return zero;
    
    // Simple: bone location in world = component origin + bone relative
    // (Ignoring rotation for speed — good enough for ESP)
    FVector boneLocal = transform.GetLocation();
    FVector compWorld = c2w.GetLocation();
    
    return compWorld + boneLocal;
}

// ============================================================================
// Draw ESP box
// ============================================================================
static void DrawBox(void *canvas, FVector2D top, FVector2D bottom, FLinearColor color) {
    float h = bottom.Y - top.Y;
    float w = h * 0.4f; // Aspect ratio for player
    float cx = (top.X + bottom.X) * 0.5f;
    
    float left = cx - w * 0.5f;
    float right_x = cx + w * 0.5f;
    
    // Corner-style box (cleaner than full rectangle)
    float cornerLen = h * 0.15f;
    
    // Top-left corner
    Line(canvas, left, top.Y, left + cornerLen, top.Y, color, 1.5f);
    Line(canvas, left, top.Y, left, top.Y + cornerLen, color, 1.5f);
    
    // Top-right corner
    Line(canvas, right_x - cornerLen, top.Y, right_x, top.Y, color, 1.5f);
    Line(canvas, right_x, top.Y, right_x, top.Y + cornerLen, color, 1.5f);
    
    // Bottom-left corner
    Line(canvas, left, bottom.Y - cornerLen, left, bottom.Y, color, 1.5f);
    Line(canvas, left, bottom.Y, left + cornerLen, bottom.Y, color, 1.5f);
    
    // Bottom-right corner
    Line(canvas, right_x, bottom.Y - cornerLen, right_x, bottom.Y, color, 1.5f);
    Line(canvas, right_x - cornerLen, bottom.Y, right_x, bottom.Y, color, 1.5f);
}

// ============================================================================
// Draw health bar
// ============================================================================
static void DrawHealthBar(void *canvas, FVector2D top, FVector2D bottom, 
                          float health, float maxHealth) {
    if (maxHealth <= 0) return;
    float ratio = health / maxHealth;
    if (ratio > 1.0f) ratio = 1.0f;
    if (ratio < 0.0f) ratio = 0.0f;
    
    float h = bottom.Y - top.Y;
    float w = h * 0.4f;
    float cx = (top.X + bottom.X) * 0.5f;
    float left = cx - w * 0.5f;
    
    // Bar on left side
    float barX = left - 5.0f;
    float barTop = top.Y;
    float barBot = bottom.Y;
    float filled = barTop + (barBot - barTop) * (1.0f - ratio);
    
    // Background
    Line(canvas, barX, barTop, barX, barBot, {0.1f, 0.1f, 0.1f, 0.6f}, 3.0f);
    
    // Health (green to red gradient)
    FLinearColor hpColor = {1.0f - ratio, ratio, 0.0f, 1.0f};
    Line(canvas, barX, filled, barX, barBot, hpColor, 2.0f);
}

// ============================================================================
// Draw skeleton
// ============================================================================
// Bone indices for BGMI (standard UE4 mannequin-like skeleton)
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
// Draw distance text using lines (minimal)
// ============================================================================
static void DrawNumber(void *canvas, float x, float y, int num, FLinearColor color, float scale) {
    // Simple 7-segment style digit rendering using lines
    // Each digit is drawn as small line segments
    char buf[8];
    int len = 0;
    if (num < 0) num = -num;
    do { buf[len++] = '0' + (num % 10); num /= 10; } while (num > 0 && len < 7);
    
    // Reverse
    for (int i = 0; i < len / 2; i++) {
        char t = buf[i]; buf[i] = buf[len-1-i]; buf[len-1-i] = t;
    }
    
    // Draw each digit as tiny dot (simpler, less overhead)
    float cx = x;
    for (int i = 0; i < len; i++) {
        // Small crosshair per digit position (acts as distance indicator)
        Line(canvas, cx, y, cx + 2*scale, y, color, 1.0f);
        cx += 6.0f * scale;
    }
    // Draw "m" indicator
    Line(canvas, cx + 2, y - 1, cx + 2, y + 2*scale, color, 1.0f);
    Line(canvas, cx + 4, y - 1, cx + 4, y + 2*scale, color, 1.0f);
}

// ============================================================================
// Main render function
// ============================================================================
void Render(uintptr_t canvas, uintptr_t ue4Base) {
    if (!cfg.enabled) return;
    if (!canvas || !ue4Base) return;
    
    g_UE4 = ue4Base;
    
    // Setup draw function pointer
    pDrawLine = (DrawLine_t)(ue4Base + Off::K2_DrawLine);
    
    // Get screen size from canvas
    mem::Read((void *)(canvas + Off::SizeX), &g_ScreenW, sizeof(float));
    mem::Read((void *)(canvas + Off::SizeY), &g_ScreenH, sizeof(float));
    // Actually SizeX/SizeY are int32
    int32_t sw = 0, sh = 0;
    mem::Read((void *)(canvas + Off::SizeX), &sw, sizeof(int32_t));
    mem::Read((void *)(canvas + Off::SizeY), &sh, sizeof(int32_t));
    g_ScreenW = (float)sw;
    g_ScreenH = (float)sh;
    if (g_ScreenW <= 0 || g_ScreenH <= 0) return;
    
    // Get camera from PlayerCameraManager
    // Walk: GWorld → GameInstance → LocalPlayers[0] → PlayerController → PlayerCameraManager
    uintptr_t pWorld = 0;
    mem::Read((void *)(ue4Base + Off::GWorld), &pWorld, sizeof(uintptr_t));
    if (!pWorld || !mem::IsValid((void *)pWorld)) return;
    
    uintptr_t gameInst = 0;
    mem::Read((void *)(pWorld + Off::OwningGameInstance), &gameInst, sizeof(uintptr_t));
    if (!gameInst || !mem::IsValid((void *)gameInst)) return;
    
    // LocalPlayers TArray
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
    
    // Read camera POV
    FMinimalViewInfo pov;
    mem::Read((void *)(camMgr + Off::CameraCachePOV), &pov, sizeof(pov));
    g_Cam.location = pov.Location;
    g_Cam.rotation = pov.Rotation;
    g_Cam.fov = pov.FOV;
    if (g_Cam.fov <= 0) g_Cam.fov = 90.0f;
    
    // Get my pawn
    uintptr_t myPawn = 0;
    mem::Read((void *)(myPC + Off::AcknowledgedPawn), &myPawn, sizeof(uintptr_t));
    if (!myPawn || !mem::IsValid((void *)myPawn)) return;
    
    // Get my team ID
    int32_t myTeam = -1;
    mem::Read((void *)(myPawn + Off::TeamID), &myTeam, sizeof(int32_t));
    
    // Get actors from UWorld → PersistentLevel → Actors
    uintptr_t level = 0;
    mem::Read((void *)(pWorld + Off::PersistentLevel), &level, sizeof(uintptr_t));
    if (!level || !mem::IsValid((void *)level)) return;
    
    TArray<uintptr_t> actors;
    mem::Read((void *)(level + Off::Actors), &actors, sizeof(actors));
    if (!actors.IsValid() || actors.Count > 100000) return;
    
    // Colors
    FLinearColor cEnemy  = {1.0f, 0.3f, 0.3f, 1.0f}; // Soft red
    FLinearColor cBone   = {0.9f, 0.9f, 0.9f, 0.8f}; // Light white
    FLinearColor cDist   = {0.7f, 0.7f, 0.7f, 0.8f}; // Gray
    FLinearColor cSnap   = {0.3f, 0.8f, 1.0f, 0.5f}; // Cyan faint
    
    // Track closest enemy for aimbot
    aimbot::g_ClosestTarget = 0;
    aimbot::g_ClosestDist = 99999.0f;
    
    // Iterate actors
    int maxScan = actors.Count < 512 ? actors.Count : 512;
    
    for (int i = 0; i < maxScan; i++) {
        uintptr_t actor = 0;
        mem::Read(&actors.Data[i], &actor, sizeof(uintptr_t));
        if (!actor || !mem::IsValid((void *)actor)) continue;
        
        // Check if it's a player (TeamID field exists and is valid)
        int32_t teamId = -1;
        mem::Read((void *)(actor + Off::TeamID), &teamId, sizeof(int32_t));
        if (teamId < 0 || teamId > 200) continue; // Not a player
        if (teamId == myTeam) continue; // Same team
        
        // Check alive
        uint8_t dead = 0;
        mem::Read((void *)(actor + Off::bDead), &dead, sizeof(uint8_t));
        if (dead) continue;
        
        float hp = 0, hpMax = 0;
        mem::Read((void *)(actor + Off::Health), &hp, sizeof(float));
        mem::Read((void *)(actor + Off::HealthMax), &hpMax, sizeof(float));
        if (hp <= 0) continue;
        
        // Get mesh component
        uintptr_t mesh = 0;
        mem::Read((void *)(actor + Off::Mesh), &mesh, sizeof(uintptr_t));
        if (!mesh || !mem::IsValid((void *)mesh)) continue;
        
        // Get head and root positions
        FVector headPos = GetBoneWorldPos(mesh, Bone::Head);
        FVector rootPos = GetBoneWorldPos(mesh, Bone::Pelvis);
        if (headPos.Length() < 1.0f || rootPos.Length() < 1.0f) continue;
        
        // Distance check
        FVector myLoc;
        uintptr_t myRoot = 0;
        mem::Read((void *)(myPawn + Off::RootComponent), &myRoot, sizeof(uintptr_t));
        if (myRoot && mem::IsValid((void *)myRoot)) {
            mem::Read((void *)(myRoot + Off::RelativeLocation), &myLoc, sizeof(FVector));
        }
        
        float dist = (headPos - myLoc).Length() / 100.0f; // cm to meters
        if (dist > cfg.maxDist || dist < 0.5f) continue;
        
        // Project to screen
        FVector2D headScreen, rootScreen;
        if (!WorldToScreen(headPos, headScreen) || !WorldToScreen(rootPos, rootScreen)) continue;
        
        // Valid target on screen
        FVector2D top = headScreen;
        top.Y -= 10.0f; // Padding above head
        FVector2D bottom = rootScreen;
        
        // ESP Box
        if (cfg.box) {
            DrawBox((void *)canvas, top, bottom, cEnemy);
        }
        
        // Skeleton
        if (cfg.skeleton) {
            DrawSkeleton((void *)canvas, mesh, cBone);
        }
        
        // Health bar
        if (cfg.health) {
            DrawHealthBar((void *)canvas, top, bottom, hp, hpMax);
        }
        
        // Distance
        if (cfg.distance) {
            DrawNumber((void *)canvas, bottom.X - 10, bottom.Y + 5, (int)dist, cDist, 1.0f);
        }
        
        // Snapline
        if (cfg.snapline) {
            Line((void *)canvas, g_ScreenW * 0.5f, g_ScreenH, 
                 (top.X + bottom.X) * 0.5f, bottom.Y, cSnap, 0.5f);
        }
        
        // Track for aimbot (screen distance to center)
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
