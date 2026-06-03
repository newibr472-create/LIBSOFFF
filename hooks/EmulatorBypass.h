/*
 * EmulatorBypass.h - Emulator Detection Bypass
 * Library: libanogs.so | Created by: Agent 2
 * NOP emulator name comparison calls
 */
PATCH_LIB("libanogs.so", "0x1C8FE4", "1F 20 03 D5"); // NOP compare
PATCH_LIB("libanogs.so", "0x1C8FF0", "1F 20 03 D5"); // NOP compare2
