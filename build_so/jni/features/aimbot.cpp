/*
 * features/aimbot.cpp — Humanized aimbot with acceleration curve
 * 
 * What makes it different from legacy:
 * - Non-linear smoothing (acceleration curve — fast far from target, slow near)
 * - Micro-jitter simulating human hand tremor
 * - Dead-zone (don't aim if already close enough)
 * - No snapping — always interpolation
 */
#include "aimbot.h"
#include "../core/memory.h"
#include "../sdk/types.h"
#include <cmath>
#include <cstdlib>
#include <android/log.h>

namespace aimbot {

Config cfg;

uintptr_t g_ClosestTarget = 0;
float     g_ClosestDist = 99999.0f;
FVector   g_TargetHead{};

// ============================================================================
// Pseudo-random for jitter (no need for crypto quality)
// ============================================================================
static uint32_t g_Seed = 0x12345678;

static float RandFloat(float min, float max) {
    g_Seed ^= g_Seed << 13;
    g_Seed ^= g_Seed >> 17;
    g_Seed ^= g_Seed << 5;
    float t = (float)(g_Seed & 0xFFFF) / 65535.0f;
    return min + t * (max - min);
}

// ============================================================================
// Angle math
// ============================================================================
static float NormalizeAngle(float a) {
    while (a > 180.0f) a -= 360.0f;
    while (a < -180.0f) a += 360.0f;
    return a;
}

static FRotator CalcRotation(FVector from, FVector to) {
    FVector d = to - from;
    float horiz = d.Length2D();
    FRotator r;
    r.Pitch = atan2f(d.Z, horiz) * 57.2957795f;
    r.Yaw = atan2f(d.Y, d.X) * 57.2957795f;
    r.Roll = 0;
    return r;
}

// ============================================================================
// Humanized smoothing — acceleration curve
// When far from target: move fast (catch up)
// When near target: slow down (precision)
// Formula: effective_smooth = base_smooth * (1 - pow(normalized_error, 0.4))
// ============================================================================
static float HumanSmooth(float error, float maxError, float baseSmooth) {
    if (maxError <= 0) return baseSmooth;
    float normalized = fabsf(error) / maxError;
    if (normalized > 1.0f) normalized = 1.0f;
    
    // Acceleration curve: stronger correction when far
    float factor = 1.0f - powf(1.0f - normalized, 2.5f);
    float smooth = baseSmooth * (0.3f + 0.7f * (1.0f - factor));
    if (smooth < 1.5f) smooth = 1.5f;
    return smooth;
}

// ============================================================================
// Apply aimbot
// ============================================================================
void Apply(uintptr_t myPC, uintptr_t camMgr, uintptr_t ue4Base) {
    if (!cfg.enabled) return;
    if (!g_ClosestTarget) return;
    if (g_ClosestDist > cfg.fov) return;  // Outside FOV circle
    if (!myPC || !camMgr) return;
    
    // Read current camera
    FMinimalViewInfo pov;
    mem::Read((void *)(camMgr + Off::CameraCachePOV), &pov, sizeof(pov));
    
    FRotator current;
    mem::Read((void *)(myPC + Off::ControlRotation), &current, sizeof(FRotator));
    
    // Target position (head)
    FVector targetPos = g_TargetHead;
    
    // Calculate desired rotation
    FRotator desired = CalcRotation(pov.Location, targetPos);
    
    // Delta angles (normalized)
    float dPitch = NormalizeAngle(desired.Pitch - current.Pitch);
    float dYaw   = NormalizeAngle(desired.Yaw - current.Yaw);
    
    // Dead zone — don't adjust if already very close (prevents jitter at lock)
    float totalError = sqrtf(dPitch * dPitch + dYaw * dYaw);
    if (totalError < 0.1f) return;
    
    // Humanized smoothing
    float pitchSmooth = HumanSmooth(dPitch, 30.0f, cfg.smooth);
    float yawSmooth   = HumanSmooth(dYaw, 30.0f, cfg.smooth);
    
    // Apply smoothed correction
    FRotator newRot;
    newRot.Pitch = current.Pitch + dPitch / pitchSmooth;
    newRot.Yaw   = current.Yaw + dYaw / yawSmooth;
    newRot.Roll  = 0;
    
    // Add micro-jitter (human hand tremor)
    if (cfg.humanize) {
        newRot.Pitch += RandFloat(-cfg.jitter, cfg.jitter);
        newRot.Yaw   += RandFloat(-cfg.jitter, cfg.jitter);
    }
    
    // Clamp
    newRot.Pitch = NormalizeAngle(newRot.Pitch);
    newRot.Yaw   = NormalizeAngle(newRot.Yaw);
    if (newRot.Pitch > 89.0f) newRot.Pitch = 89.0f;
    if (newRot.Pitch < -89.0f) newRot.Pitch = -89.0f;
    
    // Write new rotation
    mem::Write((void *)(myPC + Off::ControlRotation), &newRot, sizeof(FRotator));
}

} // namespace aimbot
