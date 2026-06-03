#pragma once
// ============================================================
// tdm_hooks.h — TDataMaster + libgcloud Hooks (ARM64)
// WOLVES PVT BYPASS v4.0 — PUBG Mobile 4.4.0
// ============================================================

#include <cstdint>
#include <cstring>

// Original function pointers
static void *(*orig_TDM_GetInstance)() = nullptr;
static void (*orig_ReportSend)(void *) = nullptr;
static void (*orig_ACConfigInit)(void *) = nullptr;

// ---- ARM64 Offsets (PUBG 4.4.0) ----

// libTDataMaster.so
#define OFF_TDM_GetInstance         0x4D47C   // TDM singleton (was 0x1B4CC ARM32)
#define OFF_TDM_GetInstance_Impl    0x4D480   // GetInstance actual entry
#define OFF_SIGPIPE_Handler         0xA74A0   // SIGPIPE handler setup function
#define OFF_SIGPIPE_Sigaction       0xA7528   // sigaction(SIGPIPE, SIG_IGN) call
#define OFF_SIGPIPE_Restore         0xA7564   // sigaction(SIGPIPE, &old) restore
#define OFF_ReportSend              0x4E33C   // Core report send dispatch
#define OFF_Report_Binary_Vtbl      0x53138   // Report_Binary vtable impl
#define OFF_ACConfigInit            0x569C0   // AC config init (was 0x248E0)
#define OFF_ACConfigCheck           0x653EC   // AC config check function
#define OFF_TDM_Init                0x92534   // TDM_Initialize C API

// PluginReport vtable entries (all in libTDataMaster.so on ARM64!)
#define OFF_PluginReport_D2         0x92AE4   // D2 destructor (was 0x1B4D8 in libgcloud ARM32)
#define OFF_PluginReport_D0         0x92AE8   // D0 destructor (was 0x1B4E4)
#define OFF_PluginReport_OnStartup  0x92B04   // OnStartup (36 bytes, calls register)
#define OFF_PluginReport_OnPostStartup 0x92B28 // OnPostStartup — already ret! (was 0x1B514)
#define OFF_PluginReport_OnPreShutdown 0x92B2C // OnPreShutdown — already ret! (was 0x1B520)
#define OFF_PluginReport_OnShutdown 0x92B30   // OnShutdown — already ret! (was 0x1B52C)

// libgcloud.so
#define OFF_GCloud_GetInstance      0x097D18  // PluginManager::GetInstance
#define OFF_GCloud_Init             0xF4AEC   // gcloud_init C API
#define OFF_GCloud_ReleaseInstance  0x097DFC  // PluginManager::ReleaseInstance

// ---- Hook: TDM GetInstance ----
// Returns null — prevents TDM singleton creation
static void *hTDM_GetInstance() {
    LOGI("TDM_GetInstance → null");
    return nullptr;
}

// ---- Hook: Report Send ----
// Blocks all TDM report sending
static void hReportSend(void *a1) {
    LOGI("TDM ReportSend blocked");
    // Don't call original — suppress all reports
}

// ---- Hook: AC Config Init ----
// Returns early — disables AC config initialization
static void hACConfigInit(void *a1) {
    LOGI("AC ConfigInit → early return");
    // Don't call original — skip AC config setup
}

// ---- Hook: PluginReport OnStartup ----
// Returns early — prevents plugin registration
static void hPluginReportOnStartup() {
    LOGI("PluginReport OnStartup blocked");
    // Don't call original — prevent registration
}
