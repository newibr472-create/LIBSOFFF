/*
 * features/aimbot.h — Humanized aimbot
 * Smooth with acceleration curve, jitter, and FOV check
 */
#pragma once
#include "../sdk/types.h"

namespace aimbot {

struct Config {
    bool enabled      = true;
    float fov         = 100.0f;   // Screen pixel FOV
    float smooth      = 6.0f;    // Higher = slower (more human)
    bool prediction   = true;    // Lead targets
    bool humanize     = true;    // Add micro-jitter
    float jitter      = 0.3f;    // Jitter magnitude (degrees)
};

extern Config cfg;

// Set by ESP each frame
extern uintptr_t g_ClosestTarget;
extern float     g_ClosestDist;
extern FVector   g_TargetHead;

// Apply aimbot — called from PostRender AFTER ESP
void Apply(uintptr_t myPC, uintptr_t camMgr, uintptr_t ue4Base);

} // namespace aimbot
