/*
 * ============================================================
 *  PHANTOM TRACE - Anti-Debug & Emulator Detection Bypass
 *  Target: libanogs.so (BGMI 4.4.0) @ offset 0x2BFCF0
 *  Method: Independent Ghidra-less discovery via ADRP cross-reference analysis
 * ============================================================
 *
 *  DISCOVERY METHOD:
 *  -----------------
 *  This hook was discovered through INDEPENDENT analysis of libanogs.so
 *  using a custom Python script (fast_hook_hunter.py) that:
 *    1. Extracted 41 detection-related strings from .rodata section
 *    2. Scanned 3.5MB .text for ADRP instructions targeting those string pages
 *    3. Found 2950 ADRP candidates, resolved 44 string cross-references
 *    4. Grouped references into 43 function regions via prologue detection
 *    5. Scored and ranked all functions by anti-cheat relevance
 *    6. Identified 0x2BFCF0 as the #1 anti-debug/emulator detection function
 *
 *  NO prior offset lists, WISDOM SRC values, or previously known addresses
 *  were used in this discovery. Pure ELF + Capstone analysis.
 *
 *  FUNCTION ANALYSIS:
 *  ------------------
 *  Address:    0x2BFCF0
 *  Size:       ~944 bytes (236 instructions)
 *  Category:   ANTI_DEBUG + EMULATOR DETECTION
 *  Obfuscation: Control Flow Flattening (CFF) with XOR-based dispatch
 *
 *  Prologue:
 *    0x2BFCEC  SUB  SP, SP, #0x80
 *    0x2BFCF0  STP  X29, X30, [SP, #0x50]   <-- HOOK TARGET
 *    0x2BFCF4  STP  X22, X21, [SP, #0x60]
 *    0x2BFCF8  STP  X20, X19, [SP, #0x70]
 *    0x2BFCFC  ADD  X29, SP, #0x50
 *
 *  One-Time Guard Flag:
 *    LDRB W9, [X0, #0x50]     -- Read guard byte from object+0x50
 *    CMP  W9, #0              -- Check if already executed
 *    CSEL W8, W8, W11, EQ     -- CFF: select branch (run vs skip)
 *    ...
 *    STRB W8, [X19, #0x50]    -- Set flag = 1 (one-time guard)
 *
 *  Detection Logic (3 checks):
 *    Check 1: /proc/self/status read via vtable [obj+0x48]
 *             -> Searches for TracerPid pattern
 *             -> Formats result as "debugger=%s" and "debugger:%s"
 *
 *    Check 2: Additional file read (key 0x2292)
 *             -> Cross-references search function at 0x2C028C
 *
 *    Check 3: Emulator detection
 *             -> Searches for "|emulator_name=" in device info
 *             -> 5-iteration loop (cmp w22, #5)
 *
 *  CFF Jump Table:
 *    ADRP X22, #0x548000
 *    ADD  X22, X22, #0x500     -- Jump table at 0x548500
 *    LDR  X8, [X22, W8, UXTW #3]
 *    BR   X8                     -- Obfuscated dispatch
 *
 *  Detection Strings Referenced:
 *    0xA17FA  "debugger"
 *    0xA0C80  "debugger:%s"
 *    0x9FD05  "debugger=%s"
 *    0xA5381  "|emulator_name="
 *
 *  WHAT THIS HOOK BLOCKS:
 *  ----------------------
 *    1. TracerPid detection (debugger/frida/gdb attachment)
 *    2. Emulator detection (QEMU, BlueStacks, etc.)
 *    3. /proc/self/status file reading and analysis
 *    4. Detection result formatting and logging
 *    5. The one-time execution guard (prevents re-running)
 *
 *  BYPASS STRATEGY:
 *  ---------------
 *    Set the guard flag byte at [this + 0x50] = 1 immediately.
 *    This makes the anti-cheat think it already performed the
 *    debugger/emulator check and skips all detection logic.
 *    Then return void without executing any checks.
 *
 *  IMPACT:
 *  -------
 *    - Frida can attach without being detected
 *    - Root (Magisk) won't trigger debugger-related flags
 *    - Emulator environments won't be flagged
 *    - /proc/self/status won't be read by this function
 *    - Detection logs won't be generated
 *
 *  NOTES:
 *  ------
 *    - This is a member function of the AnoSDK anti-cheat object
 *    - Parameter x0 = this pointer (anti-cheat object)
 *    - Return type: void
 *    - The function uses CFF (Control Flow Flattening) obfuscation
 *    - Jump table at 0x548500 controls the obfuscated dispatch
 *    - Offset 0x50 in the object is the one-time execution guard
 *    - This function is called from AnoSDK initialization flow
 *
 * ============================================================
 */

#ifndef PHANTOM_TRACE_H
#define PHANTOM_TRACE_H

#include <stdint.h>

// Hook Name: PhantomTrace
// Target:    libanogs.so @ 0x2BFCF0
// Type:      HOOK_LIB (function hook)
// Category:  Anti-Debug / Anti-Emulator Bypass

/*
 *  Original Function Signature:
 *    void AntiCheat_DebugEmulatorCheck(void* this_ptr)
 *
 *  Parameters:
 *    X0 = this_ptr  -> Pointer to AnoSDK anti-cheat object
 *                      Offset +0x48 = vtable (file read function)
 *                      Offset +0x50 = one-time guard flag (byte)
 */

Xynnacore(void, PhantomTrace, (void *obj)) {
    /*
     * PHANTOM TRACE - Make the debugger/emulator detection invisible.
     *
     * Strategy: Set the one-time execution guard flag at [obj + 0x50] = 1.
     * This tells the anti-cheat "I already ran this check, everything is clean."
     * Then return immediately without reading /proc/self/status or checking
     * for TracerPid or emulator signatures.
     *
     * Why this works:
     *   The function at 0x2BFCF0 has a one-time guard pattern:
     *     LDRB W9, [X0, #0x50]  -- if flag == 0, run checks
     *     STRB W8, [X19, #0x50]  -- set flag = 1 after running
     *   By pre-setting the flag, we skip ALL three detection checks:
     *     1. TracerPid detection (debugger attachment)
     *     2. /proc/self/status reading
     *     3. Emulator name detection
     *
     * Safety: Setting the flag byte does not affect any other anti-cheat
     * functionality. It only prevents THIS specific check from running again.
     */

    if (obj != nullptr) {
        // Set the one-time guard flag
        // The anti-cheat will think this check already ran successfully
        *(uint8_t*)((uint64_t)obj + 0x50) = 1;
    }

    return;
}

HOOK_LIB("libanogs.so", 0x2BFCF0, hFunc, oFunc);

/*
 * ============================================================
 *  ADDITIONAL DISCOVERY DATA (for reference)
 * ============================================================
 *
 *  Top 5 Hook Candidates Found (independent analysis):
 *
 *  #  Offset    Score  Category            What it blocks
 *  -  -------   -----  -----------------  ------------------
 *  1  0x37A450   117  MEMORY_PROTECT     mmap failure handler
 *  2  0x37A524   117  MEMORY_PROTECT     ms_mmap handler
 *  3  0x379CA4   112  HOOK_DETECT        set_inline_hook_error
 *  4  0x379D78   112  MEMORY_SCAN        ms_set_inline_hook
 *  5  0x379DEC   112  HOOK_DETECT        inline_hook_opcode_dismatch
 *  7  0x1D0F60   102  REPORT             ReportComplaint
 *  12 0x2BFB34    91  ANTI_DEBUG         debugger detection  <-- HOOKED
 *  15 0x1C8FE0    87  EMULATOR           emulator_name check
 *
 *  All 44 Detection String Cross-References Found:
 *    ms_scan_start, ms_mmap, ms_set_inline_hook,
 *    inline_hook_opcode_dismatch, set_inline_hook_error,
 *    debugger, debugger=%s, debugger:%s,
 *    ReportComplaint, tdm_report, REPORT, COREREPORT,
 *    CertMD5, mmap_failed, emulator_name=,
 *    ChkSetGameStatus{, ChkLogout{, status_info->game_status_:%d,
 *    and 27 more...
 *
 *  Toolchain Used:
 *    Python 3 + LIEF (ELF parsing) + Capstone 5 (ARM64 disassembly)
 *    No Ghidra, no IDA - pure script-based analysis
 *
 * ============================================================
 */

#endif // PHANTOM_TRACE_H
