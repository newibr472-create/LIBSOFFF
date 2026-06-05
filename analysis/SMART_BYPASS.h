// KHATARNAK SMART BYPASS - Generated from ARM64 disassembly analysis
// Each patch is guided by actual instruction analysis
// BGMI 4.4.0 (com.pubg.imobile) arm64-v8a
// Total patches: 86

#pragma once

void applySmartBypass() {

    // ======= libanogs.so =======
    PATCH_LIB("libanogs.so", 0x1d3814, "C0035FD6");  // RET (skip init)
    PATCH_LIB("libanogs.so", 0x1d7938, "C0035FD6");  // RET (skip reporting)
    PATCH_LIB("libanogs.so", 0x1d551c, "C0035FD6");  // RET (skip reporting)
    PATCH_LIB("libanogs.so", 0x1d79a4, "C0035FD6");  // RET (skip reporting)
    PATCH_LIB("libanogs.so", 0x1d7fc4, "C0035FD6");  // RET (skip reporting)
    PATCH_LIB("libanogs.so", 0x1d88ec, "C0035FD6");  // RET
    PATCH_LIB("libanogs.so", 0x1d3b40, "C0035FD6");  // RET (skip init)
    PATCH_LIB("libanogs.so", 0x1d5a88, "C0035FD6");  // RET (skip reporting)
    PATCH_LIB("libanogs.so", 0x1d82cc, "C0035FD6");  // RET (skip reporting)
    PATCH_LIB("libanogs.so", 0x1d78cc, "C0035FD6");  // RET (skip reporting)

    // ======= libhdmpvecore.so =======
    PATCH_LIB("libhdmpvecore.so", 0x779d0, "C0035FD6");  // RET
    PATCH_LIB("libhdmpvecore.so", 0x7cc84, "C0035FD6");  // RET
    PATCH_LIB("libhdmpvecore.so", 0x93788, "C0035FD6");  // RET
    PATCH_LIB("libhdmpvecore.so", 0x99cec, "C0035FD6");  // RET
    PATCH_LIB("libhdmpvecore.so", 0x96e38, "C0035FD6");  // RET
    PATCH_LIB("libhdmpvecore.so", 0x5193c, "C0035FD6");  // RET
    PATCH_LIB("libhdmpvecore.so", 0x9abd8, "C0035FD6");  // RET
    PATCH_LIB("libhdmpvecore.so", 0x99138, "C0035FD6");  // RET
    PATCH_LIB("libhdmpvecore.so", 0x76bf4, "C0035FD6");  // RET
    PATCH_LIB("libhdmpvecore.so", 0x5126c, "C0035FD6");  // RET
    PATCH_LIB("libhdmpvecore.so", 0x93610, "C0035FD6");  // RET
    PATCH_LIB("libhdmpvecore.so", 0x77228, "C0035FD6");  // RET
    PATCH_LIB("libhdmpvecore.so", 0x7a8b0, "C0035FD6");  // RET
    PATCH_LIB("libhdmpvecore.so", 0x50fe4, "C0035FD6");  // RET
    PATCH_LIB("libhdmpvecore.so", 0x763dc, "000080D2C0035FD6");  // MOV X0,#0; RET
    PATCH_LIB("libhdmpvecore.so", 0x6fc88, "C0035FD6");  // RET (skip reporting)
    PATCH_LIB("libhdmpvecore.so", 0x72d3c, "000080D2C0035FD6");  // MOV X0,#0; RET
    PATCH_LIB("libhdmpvecore.so", 0x75b7c, "C0035FD6");  // RET
    PATCH_LIB("libhdmpvecore.so", 0x85970, "C0035FD6");  // RET
    PATCH_LIB("libhdmpvecore.so", 0x9b00c, "C0035FD6");  // RET (skip init)
    PATCH_LIB("libhdmpvecore.so", 0x7d134, "C0035FD6");  // RET
    PATCH_LIB("libhdmpvecore.so", 0x7ecbc, "C0035FD6");  // RET (skip reporting)
    PATCH_LIB("libhdmpvecore.so", 0x7ab4c, "C0035FD6");  // RET (skip init)
    PATCH_LIB("libhdmpvecore.so", 0x70ff8, "C0035FD6");  // RET (skip init)
    PATCH_LIB("libhdmpvecore.so", 0x930e8, "C0035FD6");  // RET

    // ======= libTBlueData.so =======
    PATCH_LIB("libTBlueData.so", 0x4d1cc, "C0035FD6");  // RET
    PATCH_LIB("libTBlueData.so", 0x8e854, "C0035FD6");  // RET (skip reporting)
    PATCH_LIB("libTBlueData.so", 0x8f5c4, "C0035FD6");  // RET (skip reporting)
    PATCH_LIB("libTBlueData.so", 0x8ff3c, "C0035FD6");  // RET (skip init)
    PATCH_LIB("libTBlueData.so", 0x4d258, "C0035FD6");  // RET
    PATCH_LIB("libTBlueData.so", 0x8e81c, "C0035FD6");  // RET
    PATCH_LIB("libTBlueData.so", 0x8e8bc, "C0035FD6");  // RET
    PATCH_LIB("libTBlueData.so", 0x8e7e4, "C0035FD6");  // RET
    PATCH_LIB("libTBlueData.so", 0x8ea38, "C0035FD6");  // RET
    PATCH_LIB("libTBlueData.so", 0x92328, "C0035FD6");  // RET (skip reporting)
    PATCH_LIB("libTBlueData.so", 0x92380, "C0035FD6");  // RET (skip reporting)
    PATCH_LIB("libTBlueData.so", 0x6e484, "C0035FD6");  // RET
    PATCH_LIB("libTBlueData.so", 0x923c0, "C0035FD6");  // RET (skip reporting)
    PATCH_LIB("libTBlueData.so", 0x8e020, "C0035FD6");  // RET (skip init)
    PATCH_LIB("libTBlueData.so", 0x922e4, "C0035FD6");  // RET (skip reporting)
    PATCH_LIB("libTBlueData.so", 0x8ebf8, "C0035FD6");  // RET (skip reporting)
    PATCH_LIB("libTBlueData.so", 0x71e48, "C0035FD6");  // RET
    PATCH_LIB("libTBlueData.so", 0x92370, "C0035FD6");  // RET (skip reporting)
    PATCH_LIB("libTBlueData.so", 0x92344, "C0035FD6");  // RET (skip reporting)
    PATCH_LIB("libTBlueData.so", 0x92398, "C0035FD6");  // RET (skip reporting)
    PATCH_LIB("libTBlueData.so", 0x91ff0, "C0035FD6");  // RET (skip reporting)
    PATCH_LIB("libTBlueData.so", 0x8e7ac, "C0035FD6");  // RET (skip init)
    PATCH_LIB("libTBlueData.so", 0x923c4, "C0035FD6");  // RET (skip reporting)
    PATCH_LIB("libTBlueData.so", 0x9238c, "C0035FD6");  // RET (skip reporting)
    PATCH_LIB("libTBlueData.so", 0x8ebb4, "C0035FD6");  // RET (skip reporting)

    // ======= libhdmpve.so =======
    PATCH_LIB("libhdmpve.so", 0xc8898, "C0035FD6");  // RET
    PATCH_LIB("libhdmpve.so", 0xb95a0, "C0035FD6");  // RET
    PATCH_LIB("libhdmpve.so", 0x3d94cc, "C0035FD6");  // RET
    PATCH_LIB("libhdmpve.so", 0x13d18c, "C0035FD6");  // RET
    PATCH_LIB("libhdmpve.so", 0xb9514, "C0035FD6");  // RET
    PATCH_LIB("libhdmpve.so", 0xc1724, "C0035FD6");  // RET
    PATCH_LIB("libhdmpve.so", 0xc149c, "C0035FD6");  // RET (skip init)
    PATCH_LIB("libhdmpve.so", 0x12fb5c, "C0035FD6");  // RET
    PATCH_LIB("libhdmpve.so", 0x2e9a98, "C0035FD6");  // RET (skip init)
    PATCH_LIB("libhdmpve.so", 0x10e074, "C0035FD6");  // RET
    PATCH_LIB("libhdmpve.so", 0xc7e80, "C0035FD6");  // RET
    PATCH_LIB("libhdmpve.so", 0xbbf5c, "C0035FD6");  // RET
    PATCH_LIB("libhdmpve.so", 0x12d2ec, "C0035FD6");  // RET
    PATCH_LIB("libhdmpve.so", 0x1b3ac0, "C0035FD6");  // RET
    PATCH_LIB("libhdmpve.so", 0x179acc, "C0035FD6");  // RET
    PATCH_LIB("libhdmpve.so", 0xb7cf8, "C0035FD6");  // RET
    PATCH_LIB("libhdmpve.so", 0x3c6e98, "C0035FD6");  // RET
    PATCH_LIB("libhdmpve.so", 0xb92e8, "C0035FD6");  // RET
    PATCH_LIB("libhdmpve.so", 0x179a64, "C0035FD6");  // RET
    PATCH_LIB("libhdmpve.so", 0x1934a4, "C0035FD6");  // RET (skip init)
    PATCH_LIB("libhdmpve.so", 0x3d94d8, "C0035FD6");  // RET
    PATCH_LIB("libhdmpve.so", 0xb83bc, "C0035FD6");  // RET
    PATCH_LIB("libhdmpve.so", 0x17cb0c, "C0035FD6");  // RET
    PATCH_LIB("libhdmpve.so", 0x1799f4, "C0035FD6");  // RET
    PATCH_LIB("libhdmpve.so", 0x12f38c, "C0035FD6");  // RET

    // ======= libsigner.so =======
    PATCH_LIB("libsigner.so", 0x9afc0, "C0035FD6");  // RET
}
