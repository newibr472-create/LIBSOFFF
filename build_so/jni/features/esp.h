/*
 * features/esp.h — Clean minimal ESP rendering
 * Renders: Box, Skeleton lines, Health bar, Distance text
 */
#pragma once
#include "../sdk/types.h"
#include "../core/memory.h"

namespace esp {

struct Config {
    bool enabled     = true;
    bool box         = true;
    bool skeleton    = true;
    bool health      = true;
    bool distance    = true;
    bool snapline    = false;
    float maxDist    = 350.0f;  // meters
};

extern Config cfg;

// Called from PostRender hook with Canvas + game state
void Render(uintptr_t canvas, uintptr_t ue4Base);

} // namespace esp
