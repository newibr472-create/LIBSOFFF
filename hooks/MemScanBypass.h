/*
 * MemScanBypass.h - Memory Scan Bypass
 * Library: libanogs.so | Created by: Agent 2
 * Forces guard=0 to skip scans + NOPs scan executor calls
 */
PATCH_LIB("libanogs.so", "0x37A0E0", "09 00 80 52"); // MOV W9, #0 (skip scan1)
PATCH_LIB("libanogs.so", "0x37A130", "09 00 80 52"); // MOV W9, #0 (skip scan2)
PATCH_LIB("libanogs.so", "0x37A0BC", "1F 20 03 D5"); // NOP scan executor
PATCH_LIB("libanogs.so", "0x37A10C", "1F 20 03 D5"); // NOP scan executor2
