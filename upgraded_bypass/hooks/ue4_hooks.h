#pragma once
// ============================================================
// ue4_hooks.h — UE4 Engine Anti-Cheat Hooks (ARM64)
// WOLVES PVT BYPASS v4.0 — PUBG Mobile 4.4.0
// ============================================================

#include <cstdint>
#include <cstring>

// ARM64 base address
extern uintptr_t libUE4Base;
extern uintptr_t libUE4Alloc;
extern uintptr_t libUE4Size;

// Original function pointers
static bool (*orig_IsEmulator)() = nullptr;
static bool (*orig_IsEmulatorWhenInit)() = nullptr;
static void *(*orig_GetEmulatorName)() = nullptr;
static void (*orig_ReportAntiCheatInfo)() = nullptr;
static void (*orig_ReportAntiCheatDetailData)() = nullptr;
static bool (*orig_CheckNeedReport)(void *) = nullptr;
static void (*orig_ServerHandleHitDataArray)(void *, void *) = nullptr;
static void (*orig_HandleHitDataArray)(void *, void *) = nullptr;

// ---- ARM64 Offsets (PUBG 4.4.0 from SDK dump) ----
// libUE4.so — STExtraBlueprintFunctionLibrary
#define OFF_IsEmulator              0x7625FB0  // Runtime emulator detect
#define OFF_IsEmulatorWhenInit      0x7625FE8  // Init-time emulator detect
#define OFF_GetEmulatorName         0x761E918  // Get emulator name string

// libUE4.so — STExtraPlayerController
#define OFF_ReportAntiCheatInfo     0x6FB028C  // Report AC info to server
#define OFF_CheckNeedReport         0x6FA37C4  // Check if SACData should report
#define OFF_ServerHandleHitDataArray 0x6FB78A0 // Server handle hit data
#define OFF_HandleHitDataArray      0x6FAAA44  // Handle hit data array

// libUE4.so — PlayerAntiCheatManager
#define OFF_ReportAntiCheatDetailData 0x6D95FEC // Report detailed AC data

// PlayerController member offsets (ARM64)
#define OFF_PC_AntiCheatManagerComp  0x23E0  // PlayerAntiCheatManager*
#define OFF_PC_bShouldReportAntiCheat 0x23F0 // bool flag
#define OFF_PC_bUseAntiDataReportFilter 0x3DD8 // bool flag

// SceneComponent AntiCheatRandValue offsets (ARM64)
#define OFF_SC_AntiCheatRandValue0  0x2D0
#define OFF_SC_AntiCheatRandValue1  0x2D8
#define OFF_SC_AntiCheatRandValue2  0x2E0
#define OFF_SC_AntiCheatRandValue3  0x2E8
#define OFF_SC_AntiCheatRandValue4  0x2F0
#define OFF_SC_AntiCheatRandValue5  0x2F8
#define OFF_SC_AntiCheatRandValue6  0x300

// ---- Hook: IsEmulator ----
// Returns false — claims we're NOT an emulator
static bool hIsEmulator() {
    LOGI("IsEmulator → false");
    return false;
}

// ---- Hook: IsEmulatorWhenInit ----
// Returns false — claims we're NOT an emulator at init
static bool hIsEmulatorWhenInit() {
    LOGI("IsEmulatorWhenInit → false");
    return false;
}

// ---- Hook: GetEmulatorName ----
// Returns empty string — no emulator name
static void *hGetEmulatorName() {
    LOGI("GetEmulatorName → empty");
    return nullptr;
}

// ---- Hook: ReportAntiCheatInfo ----
// Blocks AC info reporting — does nothing
static void hReportAntiCheatInfo() {
    LOGI("ReportAntiCheatInfo blocked");
    // Don't call original — suppress report
}

// ---- Hook: ReportAntiCheatDetailData ----
// Blocks detailed AC data reporting
static void hReportAntiCheatDetailData() {
    LOGI("ReportAntiCheatDetailData blocked");
    // Don't call original — suppress report
}

// ---- Hook: CheckNeedReport ----
// Returns false — no report needed
static bool hCheckNeedReport(void *sacData) {
    LOGI("CheckNeedReport → false");
    return false;
}

// ---- Hook: ServerHandleHitDataArray ----
// Blocks server hit data handling
static void hServerHandleHitDataArray(void *weapon, void *data) {
    LOGI("ServerHandleHitDataArray blocked");
    // Don't call original
}

// ---- Hook: HandleHitDataArray ----
// Blocks hit data array processing
static void hHandleHitDataArray(void *weapon, void *data) {
    LOGI("HandleHitDataArray blocked");
    // Don't call original
}
