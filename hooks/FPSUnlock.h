// ============================================================
//  FPS Unlock Hook - libUE4.so @ BGMI 4.4.0
//  Bypasses frame rate cap by patching nanosleep call
// ============================================================
//
//  Target:  FPlatformProcess::Sleep() equivalent
//  File:    libUE4.so
//  Offset:  0xBC2BF38 (function entry)
//
//  Analysis:
//  -----------
//  Function @ 0xBC2BF38 is UE4's frame timing sleep function.
//  It calls nanosleep() at offset 0xBC2C054 to delay the next frame.
//  The delay value is computed based on target FPS config.
//
//  Key instructions:
//    0xBC2C008: mov x0, #0xc9ff / movk x0, #0x3b9a, lsl #16
//               → x0 = 0x3B9AC9FF (999999999 nanoseconds = ~1 second)
//    0xBC2C010: cmp x1, x0       → compare remaining time
//    0xBC2C014: b.le #skip       → if time <= 1s, skip to nanosleep
//    0xBC2C054: bl nanosleep     → SLEEP HERE (this caps FPS!)
//    0xBC2C074: ret
//
//  Method 1: NOP the BL nanosleep → frame delay removed, game runs unlimited
//  Method 2: RET the function     → entire sleep function skipped
//
//  IMPORTANT:
//  - NOP (Method 1) is safer — only removes the sleep call
//  - RET (Method 2) is more aggressive — skips entire function
//  - For 120fps devices, use Method 1 (NOP)
//
// ============================================================

// --- Method 1: NOP the nanosleep call (RECOMMENDED) ---
// Original:  47 D8 22 94  (BL nanosleep)
// Patched:   1F 20 03 D5  (NOP)
PATCH_LIB("libUE4.so", "0xBC2C054", "1F 20 03 D5");

// --- Method 2: RET entire sleep function (ALTERNATIVE) ---
// Original:  FD 7B 41 A9  (STP x29, x30, [sp, #-0x50]!)
// Patched:   C0 03 5F D6  (RET)
// PATCH_LIB("libUE4.so", "0xBC2BF38", "C0 03 5F D6");
