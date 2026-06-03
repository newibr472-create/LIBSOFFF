/*
 * HookDetectBypass.h - Inline Hook Detection Bypass
 * Library: libanogs.so | Created by: Agent 2
 * NOP verification calls in set_inline_hook_error block
 */
PATCH_LIB("libanogs.so", "0x379CEC", "1F 20 03 D5"); // NOP verify
PATCH_LIB("libanogs.so", "0x379D0C", "1F 20 03 D5"); // NOP extended_verify
PATCH_LIB("libanogs.so", "0x379D20", "1F 20 03 D5"); // NOP mutex check
PATCH_LIB("libanogs.so", "0x379D38", "1F 20 03 D5"); // NOP flag op
PATCH_LIB("libanogs.so", "0x379D3C", "1F 20 03 D5"); // NOP flag op2
