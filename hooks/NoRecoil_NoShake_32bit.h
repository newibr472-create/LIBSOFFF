/*
 * BGMI 4.4.0 Game Patches (32-bit libUE4.so)
 * Source: User-provided, architecture verified
 * 
 * These offsets are for 32-BIT (armeabi-v7a) libUE4.so!
 * Patch values "78 00 A0 E3" = ARM32 MOV instruction
 *     
 * For 64-BIT (arm64-v8a), equivalent patches need:
 * - Different offsets (approximately 1.5-2x higher)
 * - ARM64 instructions instead of ARM32
 * - 120 FPS: "80 0F 80 52 C0 03 5F D6" (MOV W0,#120; RET)
 * - HDR:     "A0 00 80 52 C0 03 5F D6" (MOV W0,#5; RET)
 *
 * STRING LOCATIONS IN 64-BIT libUE4.so (CONFIRMED via Ghidra):
 * - "RecoilKickADS" @ 0x03C7B763
 * - "CameraShakeInnerRadius" @ 0x03E1D961
 * - "AccessoriesVRecoilFactor" @ 0x03C90BE6
 * - "AnimationKick" @ 0x03CB527E
 * - "CameraShakeScale" @ 0x03C89337
 *
 * Ghidra analysis completed on Codespace (fuzzy-space-xylophone).
 * FName indirection prevents direct string xrefs in UE4.
 * Need UE4Dumper on rooted device for exact 64-bit property offsets.
 */

// 32-BIT PATCHES (armeabi-v7a)
PATCH_LIB("libUE4.so", "0x2EE2E34", "00 00 00 00"); // No Shake (float->0)
PATCH_LIB("libUE4.so", "0x61278E0", "00 00 00 00"); // No Shake (float->0)
PATCH_LIB("libUE4.so", "0x2A90ED8", "00 00 00 00"); // No Recoil (float->0)
PATCH_LIB("libUE4.so", "0x67CF028", "00 00 85 43"); // iPad View (FOV=266.0)
PATCH_LIB("libUE4.so", "0x4EFD5B0", "00 00 00 00"); // Aimbot (float->0)
PATCH_LIB("libUE4.so", "0x322C9C0", "78 00 A0 E3 1E FF 2F E1"); // 120 FPS
PATCH_LIB("libUE4.so", "0x322C5C8", "05 00 A0 E3 1E FF 2F E1"); // HDR Unlock
PATCH_LIB("libUE4.so", "0x6D9EE28", "00 00 00 00"); // No Grass/Tree
PATCH_LIB("libUE4.so", "0x6F3CB00", "00 00 00 00"); // Flash Speed
