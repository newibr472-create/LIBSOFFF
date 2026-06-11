# BGMI 4.4.0 ARM64 — Architecture Document

## Overview
Production-quality game modification library using proper UE4 SDK integration via ProcessEvent.

## Module Structure
```
build_so/
├── main.cpp              — Entry, initialization, PostRender hook, ESP/Aimbot
├── sdk_functions.cpp     — Extracted SDK ProcessEvent wrappers (7 functions)
├── Bypass.h              — 809 memory patches (hex_patch + mprotect)
├── build.sh              — Build script
├── SDK.hpp               — Master SDK include (includes all SDK headers)
├── SDK/                  — NIKON 4.4.0 dumped SDK (classes, structs, functions)
│   ├── NIKON_Basic.hpp/cpp         — Core types (TArray, FName, UObject)
│   ├── NIKON_CoreUObject_*         — UObject, FindObject, IsA
│   ├── NIKON_Engine_*              — AActor, APlayerController, UCanvas, UWorld
│   └── NIKON_ShadowTrackerExtra_*  — ASTExtraPlayerCharacter, GetBonePos
├── Main/
│   ├── Tools.h           — IsPtrValid, GetLibBase (/proc/self/maps)
│   ├── MemoryTools.h     — WriteAddr (mprotect + memcpy + cache flush)
│   └── InlineHook.h      — ARM64 inline hook (LDR X17; BR X17; addr)
└── android/
    └── log.h             — Stub (resolved via dlsym at runtime)
```

## Initialization Sequence
```
1. __attribute__((constructor)) → pthread_create(main_thread)
2. Wait for libUE4.so to load (poll /proc/self/maps)
3. InitSDK():
   - Get UE4 base address
   - FName::GNames = call function at UE4+0x8394964
   - UObject::GUObjectArray = pointer at UE4+0xe22f8d0
4. ApplyAllBypasses() — 809 mprotect+memcpy patches
5. Wait for game world (poll GetFullWorld())
6. HookPostRender() — vtable swap on AHUD
7. g_Initialized = true → PostRender hook now draws ESP
```

## Data Flow
```
PostRender(Canvas) called by game every frame
  ├── GetFullWorld() → UEngine→GameViewport→World
  ├── Get LocalPlayer → GameInstance→LocalPlayers[0]→PC
  ├── Get PlayerArray → GameState→PlayerArray
  ├── For each ASTExtraPlayerCharacter:
  │   ├── Check: alive (Health > 0, !bDead)
  │   ├── Check: enemy (TeamID != myTeamID)
  │   ├── GetBonePos("Head"/"neck_01"/etc) → ProcessEvent
  │   ├── ProjectWorldLocationToScreen() → ProcessEvent
  │   └── K2_DrawLine/K2_DrawText on UCanvas → ProcessEvent
  └── Aimbot: GetControlRotation → calc smooth → SetControlRotation
```

## Key Design Decisions

### Why ProcessEvent (not raw memory)?
- Type-safe — SDK functions validate parameters
- Version-resilient — offsets are in SDK structs, not hardcoded
- Game-compatible — same call path as game's own code
- No desyncs — camera matrices, transforms computed by engine

### Why PostRender hook (not overlay)?
- Draws using game's own UCanvas → perfect sync with frame
- No Android SYSTEM_ALERT_WINDOW permission needed
- No separate overlay process to detect
- Pixel-perfect with game's coordinate system

### Why vtable swap (not Dobby)?
- No external dependencies
- Works on all ARM64 Android versions
- Simpler, fewer crash vectors
- Reversible (restore original vtable entry)

## SDK Function Usage
| Function | Purpose | Source |
|----------|---------|--------|
| GetBonePos(FName, FVector) | Get skeleton bone world position | ShadowTrackerExtra |
| K2_DrawLine(pos, pos, thick, color) | Draw line on screen | Engine.Canvas |
| K2_DrawText(font, text, pos, color...) | Draw text on screen | Engine.Canvas |
| K2_DrawBox(pos, size, thick) | Draw rectangle | Engine.Canvas |
| ProjectWorldLocationToScreen | 3D→2D projection | Engine.PlayerController |
| GetControlRotation | Get current aim direction | Engine.Controller |
| SetControlRotation | Set aim direction (aimbot) | Engine.Controller |

## Memory Safety Model
- Every pointer dereference checks `Tools::IsPtrValid()`
- Valid range: 0x1000 < ptr < 0x7FFFFFFFFFF
- SDK functions check `pFunc != nullptr` before ProcessEvent
- PostRender hook validates Canvas, World, PlayerController before use
- Lobby bypass: skip all logic when PlayerArray.Count < 10

## Build
```bash
./build.sh
# Outputs: libs/arm64-v8a/libbypass.so (ARM64 ELF shared object)
```
