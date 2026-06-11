/*
 * TRUE_BYPASS.h - BGMI 4.4.0 ARM64 Anti-Cheat Bypass (CRASH-SAFE)
 * 
 * Deep analyzed with LIEF + Capstone. Only SAFE patches included.
 * Dangerous init patches REMOVED — those cause startup crashes.
 *
 * Architecture: arm64-v8a
 * Target: com.pubg.imobile (BGMI 4.4.0)
 * Requires: Macros.h (PATCH_LIB macro using KittyMemory)
 *
 * C0 03 5F D6 = RET
 * 00 00 80 52 C0 03 5F D6 = MOV W0,#0; RET (return 0)
 * 20 00 80 52 C0 03 5F D6 = MOV W0,#1; RET (return 1)
 * 1F 20 03 D5 = NOP
 */

#ifndef TRUE_BYPASS_H
#define TRUE_BYPASS_H

// ============================================================================
// libanogs.so — AnoSDK Reporting & Scanning (SAFE patches only)
// ============================================================================
// NOTE: AnoSDKInit/InitEx are NOT patched. They MUST run or game crashes.
// We only block REPORTING and SCANNING functions — AC runs but can't report.
// ============================================================================

PATCH_LIB("libanogs.so", "0x1d551c", "00 00 80 52 C0 03 5F D6");  // AnoSDKGetReportData → return 0 (no report data)
PATCH_LIB("libanogs.so", "0x1d78cc", "00 00 80 52 C0 03 5F D6");  // AnoSDKGetReportData2 → return 0
PATCH_LIB("libanogs.so", "0x1d7938", "00 00 80 52 C0 03 5F D6");  // AnoSDKGetReportData3 → return 0
PATCH_LIB("libanogs.so", "0x1d7fc4", "00 00 80 52 C0 03 5F D6");  // AnoSDKGetReportData4 → return 0
PATCH_LIB("libanogs.so", "0x1d6ea8", "00 00 80 52 C0 03 5F D6");  // AnoSDKIoctl → return 0 (scan commands disabled)
PATCH_LIB("libanogs.so", "0x1d6598", "00 00 80 52 C0 03 5F D6");  // AnoSDKIoctlOld → return 0
PATCH_LIB("libanogs.so", "0x1d5a88", "C0 03 5F D6");  // AnoSDKDelReportData → RET
PATCH_LIB("libanogs.so", "0x1d79a4", "C0 03 5F D6");  // AnoSDKDelReportData3 → RET
PATCH_LIB("libanogs.so", "0x1d82cc", "C0 03 5F D6");  // AnoSDKDelReportData4 → RET
PATCH_LIB("libanogs.so", "0x1d417c", "C0 03 5F D6");  // AnoSDKSetUserInfo → RET (don't send user info)
PATCH_LIB("libanogs.so", "0x1d4580", "C0 03 5F D6");  // AnoSDKSetUserInfoWithLicense → RET
PATCH_LIB("libanogs.so", "0x1d4c0c", "C0 03 5F D6");  // AnoSDKOnPause → RET
PATCH_LIB("libanogs.so", "0x1d5030", "C0 03 5F D6");  // AnoSDKOnResume → RET
PATCH_LIB("libanogs.so", "0x1d624c", "C0 03 5F D6");  // AnoSDKOnRecvData → RET (ignore server commands)
PATCH_LIB("libanogs.so", "0x1d88ec", "C0 03 5F D6");  // AnoSDKOnRecvSignature → RET
PATCH_LIB("libanogs.so", "0x1d7398", "C0 03 5F D6");  // AnoSDKFree → RET
PATCH_LIB("libanogs.so", "0x1d9024", "C0 03 5F D6");  // AnoSDKForExport → RET
PATCH_LIB("libanogs.so", "0x1d8c74", "C0 03 5F D6");  // AnoSDKRegistInfoListener → RET

// ============================================================================
// libhdmpvecore.so — Root Detection + Upload Blocking
// ============================================================================
// Root checks return 0 (not rooted). Upload functions blocked.
// Plugin startup NOT touched — game needs plugin chain.
// ============================================================================

PATCH_LIB("libhdmpvecore.so", "0x73150", "00 00 80 52 C0 03 5F D6");  // ABase_system_IsDeviceRooted → return 0
PATCH_LIB("libhdmpvecore.so", "0x72cac", "00 00 80 52 C0 03 5F D6");  // CSystem::IsDeviceRooted → return 0
PATCH_LIB("libhdmpvecore.so", "0x7314c", "20 00 80 52 C0 03 5F D6");  // ABase_system_CheckPermission → return 1
PATCH_LIB("libhdmpvecore.so", "0x72d3c", "20 00 80 52 C0 03 5F D6");  // CSystem::CheckPermission → return 1
PATCH_LIB("libhdmpvecore.so", "0x6fc88", "C0 03 5F D6");  // UploadTaskImpl::Upload → RET (block upload)
PATCH_LIB("libhdmpvecore.so", "0x6d660", "C0 03 5F D6");  // WWW::UploadFromData → RET
PATCH_LIB("libhdmpvecore.so", "0x6d900", "C0 03 5F D6");  // UploadTaskImpl::Finish → RET
PATCH_LIB("libhdmpvecore.so", "0x79944", "C0 03 5F D6");  // ConfigureReport::ReportPullConfigResult → RET
PATCH_LIB("libhdmpvecore.so", "0x79bd0", "C0 03 5F D6");  // ConfigureReport::ReportUploadResult → RET
PATCH_LIB("libhdmpvecore.so", "0x95b44", "C0 03 5F D6");  // TraceService::ReportContext → RET

// ============================================================================
// libTBlueData.so — Telemetry Disabled (SAFE — telemetry is non-critical)
// ============================================================================
// TDM_Initialize is LEFT ALONE — let it init, just block all reporting.
// ============================================================================

PATCH_LIB("libTBlueData.so", "0x91ff0", "C0 03 5F D6");  // TDM_Report_Binary → RET (main report blocked)
PATCH_LIB("libTBlueData.so", "0x91e28", "C0 03 5F D6");  // TDM_Enable_Report → RET
PATCH_LIB("libTBlueData.so", "0x91ef8", "C0 03 5F D6");  // TDM_Create_Event → RET
PATCH_LIB("libTBlueData.so", "0x91fb8", "C0 03 5F D6");  // TDM_Destroy_Event → RET
PATCH_LIB("libTBlueData.so", "0x91e74", "C0 03 5F D6");  // TDM_Enable_Device_Info → RET
PATCH_LIB("libTBlueData.so", "0x8e854", "C0 03 5F D6");  // JNI: TDMEnableReport → RET
PATCH_LIB("libTBlueData.so", "0x8f5c4", "C0 03 5F D6");  // JNI: TDMReportBinary → RET
PATCH_LIB("libTBlueData.so", "0x8ebf8", "C0 03 5F D6");  // JNI: TDMReportEvent → RET
PATCH_LIB("libTBlueData.so", "0x8f7ac", "C0 03 5F D6");  // JNI: TDMReportLogin → RET
PATCH_LIB("libTBlueData.so", "0x8f914", "C0 03 5F D6");  // JNI: TDMEnableDeviceInfo → RET
PATCH_LIB("libTBlueData.so", "0x8e308", "C0 03 5F D6");  // TDMUtils::EncryptField → RET
PATCH_LIB("libTBlueData.so", "0x8e578", "C0 03 5F D6");  // SetDeviceInfoEncryptKey → RET
PATCH_LIB("libTBlueData.so", "0x92344", "C0 03 5F D6");  // PluginReport::OnStartup → RET
PATCH_LIB("libTBlueData.so", "0x92368", "C0 03 5F D6");  // PluginReport::OnPostStartup → RET
PATCH_LIB("libTBlueData.so", "0x92370", "C0 03 5F D6");  // PluginReport::OnShutdown → RET
PATCH_LIB("libTBlueData.so", "0x9236c", "C0 03 5F D6");  // PluginReport::OnPreShutdown → RET

// ============================================================================
// libhdmpve.so — Only SAFE patches (sniffer + verify)
// ============================================================================
// ALL hdmpve_*_initialize functions LEFT ALONE — game NEEDS them!
// Only patching: sniffer (spyware), API verification, pre-downloaders
// ============================================================================

PATCH_LIB("libhdmpve.so", "0x247a9c", "20 00 80 52 C0 03 5F D6");  // IFSDllVerifyApiCompatable → return 1
PATCH_LIB("libhdmpve.so", "0x3f4e5c", "C0 03 5F D6");  // apollo_net_sniffer_Init → RET (kill network sniffer)
PATCH_LIB("libhdmpve.so", "0x2eaa5c", "C0 03 5F D6");  // InitDataPreDownloader → RET
PATCH_LIB("libhdmpve.so", "0x2eaa8c", "C0 03 5F D6");  // UnInitDataPreDownloader → RET

// ============================================================================
// libsigner.so — APK Signature (SAFE — Adjust SDK, not game-critical)
// ============================================================================

PATCH_LIB("libsigner.so", "0x9a088", "C0 03 5F D6");  // nOnResume → RET
PATCH_LIB("libsigner.so", "0x9afc0", "C0 03 5F D6");  // nSign → RET

#endif // TRUE_BYPASS_H
