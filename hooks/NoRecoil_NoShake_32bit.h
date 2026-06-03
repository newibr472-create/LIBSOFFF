/*
 * User-provided offsets (32-bit libUE4.so, BGMI 4.4.0)
 * WARNING: These are ARM32! For arm64, need different offsets.
 * Ghidra analysis running on Codespace for 64-bit verification.
 */
PATCH_LIB("libUE4.so", "0x2EE2E34", "00 00 00 00"); // No Shake
PATCH_LIB("libUE4.so", "0x61278E0", "00 00 00 00"); // No Shake  
PATCH_LIB("libUE4.so", "0x2A90ED8", "00 00 00 00"); // No Recoil
PATCH_LIB("libUE4.so", "0x67CF028", "00 00 85 43"); // iPad View
PATCH_LIB("libUE4.so", "0x4EFD5B0", "00 00 00 00"); // Aimbot
PATCH_LIB("libUE4.so", "0x322C9C0", "78 00 A0 E3 1E FF 2F E1"); // 120 FPS (ARM32)
PATCH_LIB("libUE4.so", "0x322C5C8", "05 00 A0 E3 1E FF 2F E1"); // HDR (ARM32)
PATCH_LIB("libUE4.so", "0x6D9EE28", "00 00 00 00"); // No Grass
PATCH_LIB("libUE4.so", "0x6F3CB00", "00 00 00 00"); // Flash Speed
