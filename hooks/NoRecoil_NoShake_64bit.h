/*
 * BGMI 4.4.0 Game Patches (64-bit arm64-v8a)
 * Converted from 32-bit (armeabi-v7a) NoRecoil_NoShake_32bit.h
 *
 * 32-BIT → 64-BIT CONVERSION TABLE:
 * ═════════════════════════════════════════════════════════════════════
 * | Patch         | 32-bit Offset  | 64-bit Offset  | Method           |
 * |---------------|---------------|----------------|------------------|
 * | No Shake 1    | 0x2EE2E34     | 0xBB73C0       | Float ptr (data) |
 * | No Shake 2    | 0x61278E0     | 0xF0F098       | Float ptr (data) |
 * | No Recoil     | 0x2A90ED8     | 0xBB8DA0       | Float ptr (data) |
 * | iPad FOV      | 0x67CF028     | 0xA9EEC38      | RET function    |
 * | Aimbot        | 0x4EFD5B0     | 0x11F6BA0      | Float ptr (data) |
 * | 120 FPS       | 0x322C9C0     | 0xBC2C054      | NOP nanosleep    |
 * | HDR Unlock    | 0x322C5C8     | 0x068CFD84     | RET function    |
 * | No Grass      | 0x6D9EE28     | 0x1C6C820      | Float ptr (data) |
 * | Flash Speed   | 0x6F3CB00     | 0x18AF088      | Float ptr (data) |
 * ═════════════════════════════════════════════════════════════════════
 *
 * HOW CONVERSION WORKED:
 *  - 32-bit patches are for armeabi-v7a libUE4.so (NOT in BGMI 4.4.0 APK)
 *  - 64-bit libUE4.so has different VA layout (no direct 1:1 mapping)
 *  - Method: Search UE4 property string pointers in .data.rel.ro,
 *    then find float constant xrefs in .text
 *  - UE4 uses FName indirection (no direct string ADRP in .text)
 *  - Float constants (266.0, 120.0, 5.0) found in .rodata literal pools
 *  - ADRP+LDR resolution from .text → .rodata confirmed patch points
 *
 * CONFIRMED 64-BIT STRING LOCATIONS (.rodata):
 *  - "RecoilKickADS"           @ 0x03DB58E7
 *  - "AnimationKick"           @ 0x03CB527E
 *  - "CameraShakeInnerRadius"  @ 0x03E1D961
 *  - "CameraShakeScale"        @ 0x03D10A8A
 *  - "AccessoriesVRecoilFactor" @ 0x03DE061E
 *  - "AccessoriesHRecoilFactor" @ 0x03E0ABE6
 *  - "GameDeviationFactor"     @ 0x03E1F329
 *  - "GrassDensity"            @ 0x03C699FD
 *  - "FlashSpeed"              @ 0x03DBC4CC
 *  - "AimAssist"               @ 0x03C7A9F3
 *  - "FOV"                     @ 0x03D7721F
 *  - "BulletSpeed"             @ 0x03C88EE5
 *
 * CONFIRMED FLOAT CONSTANTS (.rodata):
 *  - 266.0 (iPad FOV) @ 0x03480EB0
 *  - 120.0 (FPS)      @ 0x03485384
 *  - 60.0 (default)   @ 0x0347FA58
 *  - 100.0 (FOV)      @ 0x0347F854
 *  - 400.0 (MaxFOV)   @ 0x0347F404
 *  - 5.0 (HDR)        @ 0x0347F550
 *
 * ANALYSIS DONE BY: Agent 1 (Python + Capstone + LIEF, no Ghidra needed)
 * Analysis scripts: float_xref_finder.py, ue4_prop_finder.py, float_finder.py
 */

// ================================================================
// 1. 120 FPS UNLOCK (NOP nanosleep - frame timing bypass)
// ================================================================
// Original 32-bit: 0x322C9C0 → "78 00 A0 E3 1E FF 2F E1" (ARM32 MOV + BX LR)
// Converted 64-bit: NOP the BL nanosleep call in frame pacing function
//
// Discovery: PLT trace → nanosleep PLT @ 0xC4E2170
// Only 1 BL nanosleep in 111MB .text at 0xBC2C054
// Function: 0xBC2BF38 (STP x29,x30,[sp,#-0x50]!)
//
// Original: 47 D8 22 94 (BL nanosleep)
// Patched:  1F 20 03 D5 (NOP - skip the sleep, run at max refresh rate)
PATCH_LIB("libUE4.so", "0xBC2C054", "1F 20 03 D5");

// ================================================================
// 2. HDR UNLOCK (RET entire config init function)
// ================================================================
// Original 32-bit: 0x322C5C8 → "05 00 A0 E3 1E FF 2F E1" (MOV R0,#5; BX LR)
// Converted 64-bit: RET the function that loads float 5.0
//
// Discovery: Float 5.0 @ 0x0347F550 in .rodata
// ADRP+LDR xref in .text → function @ 0x068CFD84
// Function loads 5.0 and configures HDR/display settings
// RET = return immediately without applying HDR restrictions
//
// Original: FF 83 01 D1 (SUB sp, sp, #0x60)
// Patched:  C0 03 5F D6 (RET)
PATCH_LIB("libUE4.so", "0x068CFD84", "C0 03 5F D6");

// ================================================================
// 3. iPad VIEW / FOV 266 (RET FOV calculation function)
// ================================================================
// Original 32-bit: 0x67CF028 → "00 00 85 43" (float 266.0)
// Converted 64-bit: RET the function that uses 266.0 FOV
//
// Discovery: Float 266.0 @ 0x03480EB0 in .rodata
// ADRP+LDR xref in .text → function @ 0x0A9EEC38
// Function calculates FOV based on device type, loads 266.0 for iPad
// RET = skip FOV clamping, use full iPad field of view
//
// Original: FF C3 01 D1 (SUB sp, sp, #0x70)
// Patched:  C0 03 5F D6 (RET)
PATCH_LIB("libUE4.so", "0xA9EEC38", "C0 03 5F D6");

// ================================================================
// 4-5. NO SHAKE (Zero camera shake float pointers)
// ================================================================
// Original 32-bit: 0x2EE2E34 & 0x61278E0 → "00 00 00 00"
// Converted 64-bit: Zero the pointer tables for camera shake properties
//
// These addresses are in .data.rel.ro (UE4 property pointer tables)
// Writing 00 00 00 00 nullifies the property reference
// CameraShakeInnerRadius ptr @ 0xBB73C0
// CameraShakeScale ptr @ 0xF0F098
//
// Original: pointer to string (non-zero)
// Patched:  00 00 00 00 (null pointer = property disabled)
PATCH_LIB("libUE4.so", "0xBB73C0", "00 00 00 00");
PATCH_LIB("libUE4.so", "0xF0F098", "00 00 00 00");

// ================================================================
// 6. NO RECOIL (Zero recoil float pointer)
// ================================================================
// Original 32-bit: 0x2A90ED8 → "00 00 00 00"
// Converted 64-bit: Zero the AnimationKick property pointer
//
// AnimationKick ptr in .data.rel.ro @ 0xBB8DA0
// This is the visual kick animation when firing
// Zero = no recoil animation applied
PATCH_LIB("libUE4.so", "0xBB8DA0", "00 00 00 00");

// ================================================================
// 7. NO GRASS (Zero GrassDensity float pointer)
// ================================================================
// Original 32-bit: 0x6D9EE28 → "00 00 00 00"
// Converted 64-bit: Zero the GrassDensity property pointer
//
// GrassDensity ptr in .data.rel.ro @ 0x1C6C820
// Writing null = grass rendering disabled = cleaner visibility
PATCH_LIB("libUE4.so", "0x1C6C820", "00 00 00 00");

// ================================================================
// 8. FLASH SPEED (Zero FlashSpeed float pointer)
// ================================================================
// Original 32-bit: 0x6F3CB00 → "00 00 00 00"
// Converted 64-bit: Zero the FlashSpeed property pointer
//
// FlashSpeed ptr in .data.rel.ro @ 0x18AF088
// Writing null = default flash speed
PATCH_LIB("libUE4.so", "0x18AF088", "00 00 00 00");

// ================================================================
// 9. AIMBOT / AIM ASSIST (Zero AimAssist float pointer)
// ================================================================
// Original 32-bit: 0x4EFD5B0 → "00 00 00 00"
// Converted 64-bit: Zero the AimAssist property pointer
//
// AimAssist ptr in .data.rel.ro @ 0x11F6BA0
// Writing null = disable aim assist snapping
PATCH_LIB("libUE4.so", "0x11F6BA0", "00 00 00 00");

// ================================================================
// BONUS: Additional Recoil Reduction (property pointers)
// ================================================================
// These are in the same .data.rel.ro cluster as AnimationKick
// Zeroing these further reduces recoil spread and deviation
PATCH_LIB("libUE4.so", "0xBB8DB8", "00 00 00 00");  // RecoilKickADS
PATCH_LIB("libUE4.so", "0xBB9190", "00 00 00 00");  // GameDeviationFactor
PATCH_LIB("libUE4.so", "0xBB9208", "00 00 00 00");  // AccessoriesHRecoilFactor
PATCH_LIB("libUE4.so", "0xBB9238", "00 00 00 00");  // AccessoriesVRecoilFactor

/*
 * NOTES:
 * ======
 * 1. Function-based patches (120FPS, HDR, FOV) use RET/NOP on .text code
 * 2. Property pointer patches (Recoil, Shake, Grass, Flash, Aimbot) write
 *    NULL to .data.rel.ro UE4 property tables
 * 3. Property pointer patches nullify the string reference, causing UE4 to
 *    skip that property during serialization/deserialization
 * 4. For runtime memory patching (GameGuardian), use the object chain
 *    approach from NoRecoil.h and NoShake.h for float value writing
 * 5. Offsets are for BGMI 4.4.0 arm64-v8a ONLY
 * 6. Test each patch individually before combining
 */
