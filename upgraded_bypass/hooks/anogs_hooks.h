#pragma once
// ============================================================
// anogs_hooks.h — AnoGS Anti-Cheat Hooks (ARM64)
// WOLVES PVT BYPASS v4.0 — PUBG Mobile 4.4.0
// ============================================================

#include <cstdint>
#include <cstring>

// ARM64 pointer types (8 bytes)
extern uintptr_t libanogsBase;
extern uintptr_t libanogsAlloc;
extern uintptr_t libanogsSize;

// Original function pointers
static void *(*orig_AnoSDKGetReportData)(void *) = nullptr;
static void *(*orig_AnoSDKGetReportData3)(void) = nullptr;
static void *(*orig_AnoSDKGetReportData2)(void) = nullptr;

// ---- ARM64 Offsets (PUBG 4.4.0) ----
// libanogs.so
#define OFF_AnSDKGetReportData     0x1D551C  // Device info reporter (was 0x3766E0 ARM32)
#define OFF_AnSDKGetReportData3    0x1D7938  // Report data v3 trampoline (was 0xF1E3E)
#define OFF_AnSDKGetReportData3_REAL 0x1CC128 // Real body of GetReportData3
#define OFF_AnSDKGetReportData2    0x1D78CC  // Report data v2 trampoline
#define OFF_AnSDKGetReportData2_REAL 0x1C79D4 // Real body of GetReportData2
#define OFF_AnSDKInit              0x1D3814  // SDK init
#define OFF_AnSDKIoctl             0x1D6EA8  // IOCTL comm channel
#define OFF_AnSDKIoctlOld          0x1D6598  // IOCTL old channel
#define OFF_JNI_OnLoad             0x1DB894  // JNI entry

// ---- Hook: AnoSDKGetReportData ----
// Returns null to suppress device info telemetry
static void *hAnoSDKGetReportData(void *a1) {
    LOGI("AnoSDKGetReportData blocked");
    return nullptr;
}

// ---- Hook: AnoSDKGetReportData3 ----
// Returns null to kill AnoSDK report data collection
static void *hAnoSDKGetReportData3(void) {
    LOGI("AnoSDKGetReportData3 blocked");
    return nullptr;
}

// ---- Hook: AnoSDKGetReportData2 ----
// Returns null to suppress report data v2
static void *hAnoSDKGetReportData2(void) {
    LOGI("AnoSDKGetReportData2 blocked");
    return nullptr;
}
