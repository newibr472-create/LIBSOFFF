/*
 * ============================================================
 *  HYPERFRAME - 120FPS Unlock Bypass
 *  Target: libUE4.so (BGMI 4.4.0) @ offset 0xBC2C054
 *  Method: Independent nanosleep call trace in libUE4.so
 * ============================================================
 *
 *  DISCOVERY METHOD:
 *  -----------------
 *  This hook was discovered through systematic analysis of libUE4.so
 *  (223MB ARM64 binary) using the following approach:
 *
 *  1. Identified imported sleep functions:
 *     - nanosleep  @ PLT 0xC4E2170 (GOT 0xDD94B28)
 *     - usleep     @ PLT 0xC4E0D90 (GOT 0xDD94138)
 *     - sleep      @ PLT 0xC4DFC90 (GOT 0xDD938B8)
 *     - clock_gettime @ PLT 0xC4DFB70 (GOT 0xDD93828)
 *
 *  2. Scanned ALL 116MB of .text section for BL instructions
 *     targeting the nanosleep PLT address (0xC4E2170).
 *
 *  3. Found EXACTLY ONE call to nanosleep in the ENTIRE binary:
 *     @ 0xBC2C054
 *
 *  4. Traced the surrounding function:
 *     - Function prologue: 0xBC2BF38 (STP X29, X30, [SP, #-0x50]!)
 *     - Function epilogue: 0xBC2C074 (RET)
 *     - Function size: 324 bytes (81 instructions)
 *
 *  5. Identified this as the UE4 Frame Rate Limiter by pattern analysis:
 *     - Calls timing function (GetTime) to measure frame duration
 *     - Converts time to nanoseconds for nanosleep
 *     - Stores timespec structs on stack (SP+0x28, SP+0x38)
 *     - Calls nanosleep(requested_time, remaining_time)
 *     - Loops with B.NE until sleep completes (EINTR handling)
 *     - Returns W0=0 on success
 *
 *  FPS-RELATED STRINGS FOUND IN libUE4.so:
 *  ---------------------------------------
 *    0x3C6B11F: "GetFrameRateLimit"
 *    0x3D6AD21: "SetFrameRateLimit"
 *    0x3DB6FDB: "CurrentMaxFPS"
 *    0x3E3F880: "SetMaxFpsFromDevice"
 *    0x3E4FA02: "bUseFpsLock"
 *    0x3CCB838: "GetExpandDeviceMaxFPSByDeviceLevel"
 *    0x3DFA97E: "GetExpandMaxFPSConfigByQuality"
 *    0x3DE8C3F: "bLowDeviceFrameRateLimitEnabled"
 *    0x3DEA276: "FixedFrameRate"
 *    0x3D5BB9D: "TargetFrameRate"
 *    0x3D4BFF8: "targetFPS"
 *    0x34AD990: "GetMaxFPSConfigByQuality" (UTF-16)
 *    0x365826C: "TargetFrameRate" (UTF-16)
 *    0x3A85E5A: "FixedFrameRate" (UTF-16)
 *
 *  FRAME RATE LIMITER FUNCTION ANALYSIS:
 *  --------------------------------------
 *  Address:    0xBC2BF38 (prologue)
 *  Size:       324 bytes
 *  Type:       UE4 Engine Frame Pacing Function
 *
 *  Function Structure:
 *    0xBC2BF38  STP  X29, X30, [SP, #-0x50]!    ; Prologue
 *    0xBC2BF3C  MOV  X29, SP                       ; Frame pointer
 *    0xBC2BF40  STR  X0, [X29, #0x18]               ; Save param (timing object)
 *    0xBC2BF44  LDR  X0, [X29, #0x18]
 *    0xBC2BF48  MOV  W1, #1                         ; Flag = 1
 *    0xBC2BF4C  BL   0xAF01D24                     ; GetTimeStamp(time_obj, flag)
 *    0xBC2BF54  STR  X0, [X29, #0x48]               ; Save current time
 *    
 *    ; --- Calculate sleep duration ---
 *    ; Convert frame time to timespec format (seconds + nanoseconds)
 *    ; Multiple FMOV/FCMP/FADD/FDIV operations on D registers
 *    ; Stores result in stack timespec structs:
 *    ;   [SP+0x38] = requested sleep time (timespec)
 *    ;   [SP+0x28] = remaining time (timespec)
 *    
 *    0xBC2C054  BL   nanosleep  <<< THE SLEEP CALL
 *    ; Arguments: X0 = requested time ptr, X1 = remaining time ptr
 *    
 *    ; --- Handle EINTR (interrupted sleep) ---
 *    0xBC2C058  CMP  X0, X31
 *    0xBC2C064  CMP  X0, X31
 *    0xBC2C068  B.NE 0xBC2C02C                       ; Loop if interrupted
 *    
 *    0xBC2C06C  MOV  W0, #0                         ; Return 0 (success)
 *    0xBC2C070  LDP  X29, X30, [SP], #0x50         ; Epilogue
 *    0xBC2C074  RET                                  ; Done
 *
 *  WHAT THIS HOOK BLOCKS:
 *  ----------------------
 *    The nanosleep call at 0xBC2C054 is the SOLE frame rate
 *    limiting mechanism in the ENTIRE libUE4.so binary.
 *    By NOPing this single call, we remove ALL frame pacing.
 *
 *    This means:
 *    - Game renders frames as fast as the hardware allows
 *    - Devices capable of 120Hz will naturally hit ~120 FPS
 *    - Devices capable of 90Hz will hit ~90 FPS
 *    - The GPU/CPU becomes the natural FPS limiter
 *
 *  BYPASS STRATEGY:
 *  ---------------
 *    FULL_PATCH at 0xBC2C054 (4 bytes):
 *    Replace the BL nanosleep instruction with NOP (0xD503201F)
 *    This prevents the engine from sleeping between frames,
 *    effectively removing the FPS cap.
 *
 *    Alternative: HOOK_LIB the wrapper function at 0xBC2BF38
 *    to skip the entire frame timing logic and return immediately.
 *
 *  IMPACT:
 *  -------
 *    - FPS unlocked: 30/60 cap removed, hardware-limited FPS
 *    - Smoother gameplay on 120Hz devices
 *    - No anti-cheat detection risk (this is in libUE4, not libanogs)
 *    - Battery usage will increase (no frame pacing = more GPU work)
 *    - Screen tearing possible if VSync is also disabled
 *
 *  NOTES:
 *  ------
 *    - Only ONE nanosleep call exists in the entire 223MB binary
 *    - The frame limiter uses EINTR-safe loop (handles signal interrupts)
 *    - Timing is in nanoseconds (struct timespec: tv_sec + tv_nsec)
 *    - This is NOT the AnoSDK anti-cheat - it's the UE4 engine itself
 *    - libUE4.so is NOT protected by anti-cheat integrity checks
 *      (unlike libanogs.so which has inline hook detection)
 *    - For best results, pair with the PhantomTrace hook
 *
 * ============================================================
 */

#ifndef HYPERFRAME_H
#define HYPERFRAME_H

#include <stdint.h>

// Hook Name: HyperFrame
// Target:    libUE4.so @ 0xBC2C054
// Type:      FULL_PATCH (NOP the nanosleep call)
// Category:  120FPS Unlock / Frame Rate Bypass

/*
 *  Original Code at 0xBC2C054:
 *    47 D8 22 94    ; BL nanosleep (PLT)
 *
 *  Patched Code:
 *    1F 20 03 D5    ; NOP (ARM64 NOP = 0xD503201F)
 *
 *  Patch Size: 4 bytes
 */

// =====================================================
// APPROACH 1: FULL_PATCH (Recommended - Simplest & Most Effective)
// =====================================================
// Replace the BL nanosleep with NOP
// This completely removes the frame rate limiting sleep

// FULL_PATCH("libUE4.so", 0xBC2C054, 4);

// =====================================================
// APPROACH 2: HOOK_LIB (Alternative - More Control)
// =====================================================
// Hook the wrapper function to skip entire frame pacing

/*
 *  Wrapper Function Signature:
 *    int32_t UE4_FramePacer(void* time_obj)
 *
 *  Parameters:
 *    X0 = time_obj  -> Pointer to UE4 timing/state object
 *
 *  The function calculates frame time, converts to nanoseconds,
 *  calls nanosleep, handles EINTR, and returns 0 on success.
 */

/*
Xynnacore(int32_t, HyperFrame, (void *time_obj)) {
    // Skip the entire frame rate pacing logic
    // Return 0 (success) immediately without sleeping
    return 0;
}

HOOK_LIB("libUE4.so", 0xBC2BF38, hFunc, oFunc);
*/

// =====================================================
// APPROACH 3: PATCH_BYTES (Precise Control)
// =====================================================
// Write specific bytes to patch the nanosleep call

// PATCH_BYTES("libUE4.so", 0xBC2C054, "\x1f\x20\x03\xd5");

/*
 * ============================================================
 *  PATCH LOCATION SUMMARY
 * ============================================================
 *
 *  Library:     libUE4.so
 *  File Size:   223,345,672 bytes (223MB)
 *  .text:       0x5602000 - 0xC4DFAD0 (116MB)
 *
 *  Frame Rate Limiter:
 *    Function:   0xBC2BF38 - 0xBC2C074 (324 bytes)
 *    Prologue:   0xBC2BF38  STP X29, X30, [SP, #-0x50]!
 *    Sleep Call: 0xBC2C054  BL nanosleep
 *    Epilogue:   0xBC2C074  RET
 *
 *  nanosleep PLT:
 *    PLT Entry:  0xC4E2170
 *    GOT Entry:  0xDD94B28
 *
 *  Original bytes at 0xBC2C054: 47 D8 22 94 (BL nanosleep)
 *  Patched bytes at 0xBC2C054: 1F 20 03 D5 (NOP)
 *
 * ============================================================
 *  VERIFICATION
 * ============================================================
 *
 *  Before patch: Game runs at 30/60 FPS (device-dependent cap)
 *  After patch:  Game runs at device's maximum refresh rate
 *
 *  To verify the patch is working:
 *  1. Open BGMI settings -> Graphics
 *  2. FPS option should show "120" or "Extreme"
 *  3. In-game FPS counter should show 90-120+ FPS
 *
 * ============================================================
 */

#endif // HYPERFRAME_H
