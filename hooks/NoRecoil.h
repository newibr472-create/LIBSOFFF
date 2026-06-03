/*
 * NoRecoil.h - Zero Recoil Hack for BGMI 4.4.0
 * Library: libUE4.so (arm64-v8a)
 * Created by: Agent 2
 * 
 * ═══════════════════════════════════════════════════════════════
 * HOW RECOIL WORKS IN BGMI (Unreal Engine 4):
 * ═══════════════════════════════════════════════════════════════
 * 
 * Object Chain:
 *   STExtraBaseCharacter (Player)
 *     → WeaponManagerComponent (+0x21B8..0x24D8 depending on version)
 *       → CurrentWeaponReplicated (+0x4F0..0x558)
 *         → ShootWeaponEntityComp (+0xFE8..0x11B8)
 *           → RecoilKickADS (float)
 *           → AccessoriesVRecoilFactor (float) 
 *           → AccessoriesHRecoilFactor (float)
 *           → RecoilInfo (struct)
 *
 * Writing 0.0f to these floats = ZERO recoil.
 *
 * ═══════════════════════════════════════════════════════════════
 * OFFSET TABLE (Multi-Version):
 * ═══════════════════════════════════════════════════════════════
 * 
 * ShootWeaponEntity offsets (relative to component base):
 * 
 * | Field                     | v2.x(2022) | v3.0(2024) | v3.6(2025) | v4.4(2026) EST |
 * |---------------------------|------------|------------|------------|----------------|
 * | RecoilInfo                | 0x9B0      | 0xA48      | 0xA48      | 0xA78~0xAA8    |
 * | AccessoriesVRecoilFactor  | 0xA20      | 0xAB8      | 0xAB8      | 0xAE8~0xB18    |
 * | AccessoriesHRecoilFactor  | 0xA24      | 0xABC      | 0xABC      | 0xAEC~0xB1C    |
 * | AccessoriesRecoveryFactor | -          | 0xAC0      | 0xAC0      | 0xAF0~0xB20    |
 * | GameDeviationFactor       | 0xA7C      | 0xB30      | 0xB34      | 0xB60~0xB90    |
 * | RecoilKickADS             | 0xB30      | 0xBE8      | 0xBF8      | 0xC28~0xC68    |
 * | AnimationKick             | 0xB4C      | 0xC04      | 0xC04      | 0xC34~0xC74    |
 *
 * NOTE: Exact 4.4.0 offsets need SDK dump verification.
 *       Use the GG script (scripts/no_recoil.lua) for dynamic finding.
 *
 * ═══════════════════════════════════════════════════════════════
 * PATCH METHOD (for Xynnacore/memory patcher):
 * ═══════════════════════════════════════════════════════════════
 *
 * Method A: Write 0.0f to recoil fields (runtime, per-weapon)
 * Method B: Patch the recoil APPLY function in libUE4.so (one-time)
 *
 * Method B is more reliable - patch the function that reads RecoilKickADS
 * and applies it to camera rotation. Make it return 0 always.
 * 
 * The recoil apply function signature in ARM64:
 *   LDR S0, [X0, #RecoilKickADS_offset]  ← reads recoil value
 *   FMUL S0, S0, S1                      ← multiplies by factor
 *   STR S0, [X8, #rotation_offset]        ← applies to camera
 *
 * Patch: Replace LDR with FMOV S0, #0.0 (0x2E00_001E in ARM64)
 */

// ═══════════════════════════════════════════════════════════════
// RUNTIME MEMORY PATCH (GameGuardian / Memory Editor approach)
// ═══════════════════════════════════════════════════════════════
// 
// Step 1: Get libUE4.so base
//   base = gg.getRangesList("libUE4.so")[Xa].start
//
// Step 2: Read object chain to find ShootWeaponEntityComponent
//   (chain described above)
//
// Step 3: Write 0.0f (bytes: 00 00 00 00) to:
//   ShootWeaponEntityComp + RecoilKickADS
//   ShootWeaponEntityComp + AccessoriesVRecoilFactor  
//   ShootWeaponEntityComp + AccessoriesHRecoilFactor
//   ShootWeaponEntityComp + AnimationKick
//   ShootWeaponEntityComp + GameDeviationFactor
//
// ═══════════════════════════════════════════════════════════════

// BGMI 4.4.0 Estimated offsets (need SDK dump to confirm exact values)
// These are best estimates based on version progression analysis:

#define RECOIL_KICK_ADS         0xC38   // float - main ADS recoil
#define ACCESSORIES_V_RECOIL    0xAF8   // float - vertical recoil factor
#define ACCESSORIES_H_RECOIL    0xAFC   // float - horizontal recoil factor  
#define ACCESSORIES_RECOVERY    0xB00   // float - recovery speed factor
#define ANIMATION_KICK          0xC54   // float - visual kick animation
#define GAME_DEVIATION_FACTOR   0xB70   // float - bullet deviation
#define RECOIL_INFO_BASE        0xA88   // struct - full recoil data
#define BULLET_FIRE_SPEED       0x4E0   // float - bullet velocity (set high for instahit)

// Object chain offsets (estimated for 4.4.0):
#define WEAPON_MANAGER_COMP     0x2558  // STExtraBaseCharacter → WeaponManagerComponent
#define CURRENT_WEAPON_REP      0x558   // WeaponManagerComponent → CurrentWeaponReplicated
#define SHOOT_WEAPON_ENTITY     0x1048  // STExtraShootWeapon → ShootWeaponEntityComp
#define SHOOT_WEAPON_COMP       0xF50   // STExtraShootWeapon → ShootWeaponComponent

/*
 * For confirmed offsets, run UE4Dumper on device:
 * ./ue4dumper --sdkw --gname <GNames_addr> --gworld <GWorld_addr> --package com.pubg.imobile
 * Then search dump for "ShootWeaponEntity" class → find field offsets
 */
