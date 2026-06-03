/*
 * GuardFlagKiller.h - Universal Anti-Cheat Guard Flag Bypass
 * Library: libanogs.so | Created by: Agent 2
 * 
 * Disables ALL 25 BSS guard flags that control scan execution.
 * When flag=0, check is SKIPPED. Zeroing all = disable all at once.
 * 
 * BSS region: 0x54A000, flags at +0x3C8 and +0xBBC to +0xBD3
 */
PATCH_LIB("libanogs.so", "0x54A3C8", "00");
PATCH_LIB("libanogs.so", "0x54ABBC", "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00");
