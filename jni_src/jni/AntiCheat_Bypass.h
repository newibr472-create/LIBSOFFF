#pragma once
// ============================================================
// AntiCheat_Bypass.h - COMPLETE Anti-Cheat Bypass Integration
// ALL 1762 bypasses + 8 hidden AC mechanisms
// BGMI 4.4.0 arm64-v8a
// ============================================================

#include "Settings.h"

// Bypass status tracking
static int g_bypassSuccess = 0;
static int g_bypassFailed = 0;
static int g_bypassSkipped = 0;

#define SAFE_PATCH_LIB(lib, offset, hex, name) \
    do { \
        if (isLibraryLoaded(lib)) { \
            patchOffset(oxorany(lib), string2Offset(oxorany(offset)), oxorany(hex), true); \
            g_bypassSuccess++; \
            if (g_Settings.bVerboseLog) LOGI("[@OWNERHUBEE] PASS: %s @ %s+%s", name, lib, offset); \
            logStatus("[OK] %s @ %s+%s", name, lib, offset); \
        } else { \
            g_bypassSkipped++; \
            logStatus("[SKIP] %s - lib not loaded: %s", name, lib); \
        } \
    } while(0)

// ============================================================
// HOOK FUNCTION DEFINITIONS (for DobbyHook-based bypasses)
// ============================================================

// --- Xynnacore macro compatibility ---
#define Xynnacore(ret, name, params) \
    static ret (*o##name)params = nullptr; \
    static ret h##name params

// --- AnoGS Hooks ---
Xynnacore(int64_t, ScanTrigger, (int64_t a1)) {
    if (a1) *(_BYTE *)(a1 + 0x38) = 0;
    return 0;
}

Xynnacore(int64_t, IntegrityDispatcher, (int64_t a1)) {
    if (a1) *(_DWORD *)(a1 + 0xC) = 0x0000FFFF;
    return oIntegrityDispatcher(a1);
}

Xynnacore(void, PhantomTrace, (void *obj)) {
    if (obj != nullptr) {
        *(uint8_t*)((uint64_t)obj + 0x50) = 1;
    }
    return;
}

Xynnacore(int64_t, WisdomSub_2F4F08, (int64_t a1, int64_t a2)) {
    if (a1) return 0;
    return oWisdomSub_2F4F08(a1, a2);
}

Xynnacore(int64_t, WisdomSub_330498, (int64_t a1, int a2)) {
    return 0;
}

Xynnacore(int64_t, WisdomSub_335D20, (int64_t a1, int64_t a2, int64_t a3, int64_t a4)) {
    return 0;
}

Xynnacore(int64_t, WisdomSub_39F570, (int64_t a1, int a2)) {
    return 0;
}

Xynnacore(int64_t, WisdomSub_3A5650, (int64_t a1, int a2)) {
    return 0;
}

Xynnacore(int64_t, WisdomSub_436950, (int64_t a1, int a2, int64_t a3)) {
    return 0;
}

Xynnacore(int, WisdomSub_4D4C98, (int a1, int a2)) {
    return 0;
}

Xynnacore(int, WisdomSub_4DCBE0, (int64_t a1, int64_t a2, int64_t a3, int64_t a4, int a5)) {
    return 0;
}

// --- UE4 AC Hooks ---
Xynnacore(int, UE4_749F538, (int64_t a1, int64_t a2)) {
    return 0;
}

Xynnacore(int64_t, UE4_7AD8950, (int64_t a1, int64_t a2)) {
    return 0;
}

Xynnacore(int, UE4_A4FBA10, (int64_t a1, int64_t a2, int a3, int64_t a4)) {
    return 0;
}

// --- TDataMaster Hooks ---
Xynnacore(void*, TDM_GetInstance, ()) {
    return nullptr;
}

Xynnacore(void, TDM_ReportSend, (void *a1)) {
    // Block all reports
}

Xynnacore(void, TDM_ACConfigInit, (void *a1)) {
    // Skip AC config
}

// --- UE4 Emulator/Report Hooks ---
Xynnacore(bool, IsEmulator, ()) {
    return false;
}

Xynnacore(bool, IsEmulatorWhenInit, ()) {
    return false;
}

Xynnacore(void*, GetEmulatorName, ()) {
    return nullptr;
}

Xynnacore(void, ReportAntiCheatInfo, ()) {
    // Blocked
}

Xynnacore(void, ReportAntiCheatDetailData, ()) {
    // Blocked
}

Xynnacore(bool, CheckNeedReport, (void *sacData)) {
    return false;
}

Xynnacore(void, ServerHandleHitDataArray, (void *weapon, void *data)) {
    // Blocked
}

Xynnacore(void, HandleHitDataArray, (void *weapon, void *data)) {
    // Blocked
}

// ============================================================
// PHASE 1: libanogs.so PATCHES (Anti-Cheat Core)
// ============================================================
void applyAnogsPatches() {
    
    logStatus("=== PHASE 1: libanogs.so PATCHES ===");


    
    // --- AnoSDK Report Functions (RET) ---
    SAFE_PATCH_LIB("libanogs.so", "0x1d7938", "C0035FD6", "AnoSDKGetReportData3");
    SAFE_PATCH_LIB("libanogs.so", "0x1d551c", "C0035FD6", "AnoSDKGetReportData");
    SAFE_PATCH_LIB("libanogs.so", "0x1d79a4", "C0035FD6", "AnoSDKDelReportData3");
    SAFE_PATCH_LIB("libanogs.so", "0x1d7fc4", "C0035FD6", "AnoSDKGetReportData4");
    SAFE_PATCH_LIB("libanogs.so", "0x1d88ec", "C0035FD6", "AnoSDKOnRecvSignature");
    SAFE_PATCH_LIB("libanogs.so", "0x1d5a88", "C0035FD6", "AnoSDKDelReportData");
    SAFE_PATCH_LIB("libanogs.so", "0x1d82cc", "C0035FD6", "AnoSDKDelReportData4");
    SAFE_PATCH_LIB("libanogs.so", "0x1d78cc", "C0035FD6", "AnoSDKGetReportData2");
    
    // --- Guard Flags (BSS zero) ---
    SAFE_PATCH_LIB("libanogs.so", "0x54A3C8", "00", "MainDispatchGuard");
    SAFE_PATCH_LIB("libanogs.so", "0x54ABBC", "00000000000000000000000000000000000000000000000000", "24DetectionGuards");
    
    // --- Memory Scan Bypass ---
    SAFE_PATCH_LIB("libanogs.so", "0x37A0E0", "09008052", "MemScan_SkipGuard1");
    SAFE_PATCH_LIB("libanogs.so", "0x37A130", "09008052", "MemScan_SkipGuard2");
    SAFE_PATCH_LIB("libanogs.so", "0x37A0BC", "1F2003D5", "MemScan_NOPExec1");
    SAFE_PATCH_LIB("libanogs.so", "0x37A10C", "1F2003D5", "MemScan_NOPExec2");
    
    // --- Hook Detection Bypass ---
    SAFE_PATCH_LIB("libanogs.so", "0x379CEC", "1F2003D5", "HookDetect_Verify");
    SAFE_PATCH_LIB("libanogs.so", "0x379D0C", "1F2003D5", "HookDetect_ExtVerify");
    SAFE_PATCH_LIB("libanogs.so", "0x379D20", "1F2003D5", "HookDetect_Mutex");
    SAFE_PATCH_LIB("libanogs.so", "0x379D38", "1F2003D5", "HookDetect_Flag1");
    SAFE_PATCH_LIB("libanogs.so", "0x379D3C", "1F2003D5", "HookDetect_Flag2");
    
    // --- Report Blocker ---
    SAFE_PATCH_LIB("libanogs.so", "0x1D0F68", "1F2003D5", "ReportBuilder_NOP1");
    SAFE_PATCH_LIB("libanogs.so", "0x1D0FA8", "1F2003D5", "ReportBuilder_NOP2");
    SAFE_PATCH_LIB("libanogs.so", "0x1D0FEC", "1F2003D5", "ReportSubmit_NOP");
    
    // --- Emulator Detection ---
    SAFE_PATCH_LIB("libanogs.so", "0x1C8FE4", "1F2003D5", "EmuDetect_Compare1");
    SAFE_PATCH_LIB("libanogs.so", "0x1C8FF0", "1F2003D5", "EmuDetect_Compare2");
    
    // --- Report Construction Block ---
    SAFE_PATCH_LIB("libanogs.so", "0x4D4818", "1F2003D5", "TDM_ReportInit_NOP");
    SAFE_PATCH_LIB("libanogs.so", "0x4D4830", "1F2003D5", "TDM_ReportSubmit_NOP");
    SAFE_PATCH_LIB("libanogs.so", "0x4D4838", "08008052", "TDM_ReportFlag_Zero");
    SAFE_PATCH_LIB("libanogs.so", "0x4D48A4", "1F2003D5", "CoreReport_VtblNOP1");
    SAFE_PATCH_LIB("libanogs.so", "0x4D48BC", "1F2003D5", "CoreReport_VtblNOP2");
    SAFE_PATCH_LIB("libanogs.so", "0x4D48D8", "1F2003D5", "CoreReport_DataSend");
    
    // --- Server Communication Block ---
    SAFE_PATCH_LIB("libanogs.so", "0x4F7238", "1F2003D5", "SendToSvr_NOP");
    SAFE_PATCH_LIB("libanogs.so", "0x4F7074", "000080D2C0035FD6", "SendFunc_RetZero");
    
    // --- HIDDEN AC #1: Anti-Debug Detection ---
    SAFE_PATCH_LIB("libanogs.so", "0x2BFB34", "C0035FD6", "HIDDEN_AntiDebug1");
    SAFE_PATCH_LIB("libanogs.so", "0x2BFB50", "C0035FD6", "HIDDEN_AntiDebug2");
    SAFE_PATCH_LIB("libanogs.so", "0x2BFB70", "C0035FD6", "HIDDEN_AntiDebug3");
    
    // --- Install Hooks ---
    logStatus("--- Installing AnoGS Function Hooks ---");


    HOOK_LIB("libanogs.so", "0x369658", hScanTrigger, oScanTrigger);
    logStatus("[HOOK] ScanTrigger @ 0x369658");
    
    HOOK_LIB("libanogs.so", "0x378FF4", hIntegrityDispatcher, oIntegrityDispatcher);
    logStatus("[HOOK] IntegrityDispatcher @ 0x378FF4");
    
    HOOK_LIB("libanogs.so", "0x2BFCF0", hPhantomTrace, oPhantomTrace);
    logStatus("[HOOK] PhantomTrace @ 0x2BFCF0");
    
    HOOK_LIB("libanogs.so", "0x2F4F08", hWisdomSub_2F4F08, oWisdomSub_2F4F08);
    HOOK_LIB("libanogs.so", "0x330498", hWisdomSub_330498, oWisdomSub_330498);
    HOOK_LIB("libanogs.so", "0x335D20", hWisdomSub_335D20, oWisdomSub_335D20);
    HOOK_LIB("libanogs.so", "0x39F570", hWisdomSub_39F570, oWisdomSub_39F570);
    HOOK_LIB("libanogs.so", "0x3A5650", hWisdomSub_3A5650, oWisdomSub_3A5650);
    HOOK_LIB("libanogs.so", "0x436950", hWisdomSub_436950, oWisdomSub_436950);
    HOOK_LIB("libanogs.so", "0x4D4C98", hWisdomSub_4D4C98, oWisdomSub_4D4C98);
    HOOK_LIB("libanogs.so", "0x4DCBE0", hWisdomSub_4DCBE0, oWisdomSub_4DCBE0);
    logStatus("[HOOK] WisdomBypass x8 hooks installed");
    
    LOGI("[@OWNERHUBEE] AnoGS patches complete: %d success", g_bypassSuccess);
}


// ============================================================
// PHASE 2: libhdmpvecore.so PATCHES
// ============================================================
void applyHdmpvecorePatches() {
    if (!g_Settings.bHdmpvecorePatches) {
        logStatus("[DISABLED] HDmpveCore patches skipped by settings");
        return;
    }
    
    logStatus("\n=== PHASE 2: libhdmpvecore.so PATCHES ===");
    
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x779d0", "C0035FD6", "_ZN6HDmpve16RemoteGameConfigD2Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7cc84", "C0035FD6", "_ZN6HDmpve9Configure11GetInstanceEv");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x93788", "C0035FD6", "_ZN6HDmpve6Plugin13PluginManager15Uninst");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x99cec", "C0035FD6", "_ZN6HDmpve6Plugin19RemoteConfigSerivceC2");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x96e38", "C0035FD6", "_ZN6HDmpve6Plugin11SpanContextC1EPKc");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x5193c", "C0035FD6", "_ZN5ABase19CApolloBufferWriter5WriteERNS");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9abd8", "C0035FD6", "_ZN6HDmpve6Plugin14ServiceManager15Relea");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x99138", "C0035FD6", "_ZThn32_N6HDmpve6Plugin21HDmpveCoreInner");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x76bf4", "C0035FD6", "_ZN6HDmpve16RemoteGameConfig11AddObserve");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x5126c", "C0035FD6", "_ZN5ABase16_tagApolloActionD1Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x93610", "C0035FD6", "_ZNSt8_Rb_treeISsSt4pairIKSsN6HDmpve6Plu");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x77228", "C0035FD6", "_ZN6HDmpve16RemoteGameConfigC2EPKc");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7a8b0", "C0035FD6", "_ZN6HDmpve12RemoteConfig9GetStringEPKcPc");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x50fe4", "C0035FD6", "_ZNK5ABase22_tagApolloBufferBuffer6encod");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x763dc", "C0035FD6", "_ZThn16_N5ABase15ANetworkChecker4PingEPK");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6fc88", "C0035FD6", "_ZN5ABase14UploadTaskImpl6UploadEiiiPKc");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x72d3c", "C0035FD6", "_ZN5ABase7CSystem15CheckPermissionEi");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x75b7c", "C0035FD6", "_ZN5ABase16TraceRouteResult8ReadFromERNS");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x85970", "C0035FD6", "_ZN7AString6assignEPKci");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7d134", "C0035FD6", "_ZN6HDmpve22HDmpveCoreRemoteConfig9GetSt");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7ecbc", "C0035FD6", "_ZN5ABase15CPlatformObject15SendUnityBuf");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x930e8", "C0035FD6", "_ZN6HDmpve6Plugin20_tagPluginDescriptorD");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7aa40", "C0035FD6", "_ZN6HDmpve12RemoteConfigD0Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7b080", "C0035FD6", "_ZN6HDmpve13ConfigureImpl13_RetryRequest");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x513f8", "C0035FD6", "_ZNK5ABase20_tagApolloBufferBase6encodeE");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x52410", "C0035FD6", "_ZN6HDmpve5ABase13TeaEncryptECBEPKcS2_Pc");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6e514", "C0035FD6", "_ZN5ABase14UploadTaskImplD2Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x942b4", "C0035FD6", "_ZN6HDmpve6Plugin12TraceService9genSpanI");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x94c50", "C0035FD6", "_ZN6HDmpve6Plugin12TraceService10GetTrac");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x709c8", "C0035FD6", "Java_com_gcore_abase_cos_CUploadTask_nat");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x74d30", "C0035FD6", "_ZN5ABase15ANetworkChecker15ReleaseInsta");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x73150", "000080D2C0035FD6", "ABase_system_IsDeviceRooted");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x94178", "C0035FD6", "_ZNK6HDmpve6Plugin12TraceService7GetName");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x51a1c", "C0035FD6", "_ZN5ABase19CApolloBufferWriter5WriteEPK7");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7aa00", "C0035FD6", "_ZN6HDmpve12RemoteConfigD1Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x65298", "C0035FD6", "_ZN5ABase23PrintLogToConsoleFormatEN6HDm");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x97420", "C0035FD6", "_ZN6HDmpve6Plugin14TraceIdManager9GenSpa");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x765e4", "C0035FD6", "_ZThn16_N5ABase15ANetworkChecker10TraceR");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x97b08", "C0035FD6", "_ZN6HDmpve6Plugin14TraceIdManager16parse");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x77548", "C0035FD6", "_ZN6HDmpve16RemoteGameConfig6GetIntEPKci");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x8015c", "C0035FD6", "_ZNKSt6vectorIP7AObjectSaIS1_EE12_M_chec");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7a004", "C0035FD6", "_ZN6HDmpve14StringIteratorC2Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6d660", "C0035FD6", "_ZN5ABase3WWW14UploadFromDataEPKcPKvi");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9b054", "C0035FD6", "Java_com_gcore_hdmpve_plugin_PluginUtils");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7b6e4", "C0035FD6", "_ZNSt8_Rb_treeISsSt4pairIKSsPN6HDmpve17C");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9a460", "C0035FD6", "_ZN6HDmpve6Plugin20HDmpveCoreLogSerivce9");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9703c", "C0035FD6", "_ZNK6HDmpve6Plugin14TraceIdManager17Dump");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9627c", "C0035FD6", "_ZN6HDmpve6Plugin11SpanContextD1Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9aa30", "C0035FD6", "_ZN6HDmpve6Plugin14ServiceManager16GetSe");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6b1ac", "C0035FD6", "_ZN5ABase19CApolloBufferReader4ReadER7AS");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6d900", "C0035FD6", "_ZN5ABase14UploadTaskImpl6FinishEv");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9a330", "C0035FD6", "_ZNSt8_Rb_treeISsSt4pairIKSsPN6HDmpve7IL");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x98804", "C0035FD6", "_ZN6HDmpve6Plugin14TraceIdManager18OnDat");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x79944", "C0035FD6", "_ZN6HDmpve15ConfigureReport22ReportPullC");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9a1e8", "C0035FD6", "_ZN6HDmpve6Plugin20HDmpveCoreLogSerivceD");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6b398", "C0035FD6", "_ZNK5ABase22_tagADetailNetworkInfo7Write");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x72cac", "000080D2C0035FD6", "_ZN5ABase7CSystem14IsDeviceRootedEv");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x78628", "C0035FD6", "_ZN6HDmpve12FileUploaderC2Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x97300", "C0035FD6", "_ZN6HDmpve6Plugin16_tagTraceIdStoreC2Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x92658", "C0035FD6", "_Z27hdmpvecore_get_version_hashv");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x99bfc", "C0035FD6", "_ZNK6HDmpve6Plugin19RemoteConfigSerivce1");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7d100", "C0035FD6", "_ZN6HDmpve22HDmpveCoreRemoteConfig7GetBo");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7a02c", "C0035FD6", "_ZN6HDmpve14StringIterator4PushEPKc");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7cd34", "C0035FD6", "hdmpvecore_configure_clear");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x52694", "C0035FD6", "_ZN6HDmpve5ABase14TeaDecryptECB3EPKcS2_P");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x98fbc", "C0035FD6", "_ZN6HDmpve6Plugin21HDmpveCoreInnerPlugin");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7a1e4", "C0035FD6", "_ZN6HDmpve14ConfigObserverC1EPNS_12Remot");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x934d0", "C0035FD6", "_ZN6HDmpve6Plugin13PluginManager15GetPlu");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x8740c", "C0035FD6", "_ZNSt8_Rb_treeISsSt4pairIKSsN6HDmpve6Plu");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x76af0", "C0035FD6", "_ZN6HDmpve22ConfigObserverDispatch11AddO");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x687fc", "C0035FD6", "_ZNSt11_Deque_baseIN5ABase10ThreadPool4T");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7c8b4", "C0035FD6", "_ZThn32_N6HDmpve13ConfigureImpl18OnDataT");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7a45c", "C0035FD6", "_ZN6HDmpve12RemoteConfig11AddObserverEPN");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6d72c", "C0035FD6", "_ZN5ABase3WWW7DestroyEPNS_10UploadTaskE");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6fc40", "C0035FD6", "_ZN5ABase14UploadTaskImpl5PauseEv");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x93368", "C0035FD6", "_ZN6HDmpve6Plugin13PluginManagerC2Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x93450", "C0035FD6", "_ZNSt8_Rb_treeISsSt4pairIKSsN6HDmpve6Plu");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7ed04", "C0035FD6", "_ZN5ABase15CPlatformObject15SendUnityBuf");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x932d4", "C0035FD6", "_ZN6HDmpve6Plugin13PluginManagerD1Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x75db8", "C0035FD6", "_ZN5ABase14NSLookupResult8ReadFromERNS_1");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6fea8", "C0035FD6", "_ZN5ABase14UploadTaskImpl14onTaskFinishe");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x98fe0", "C0035FD6", "_ZN6HDmpve6Plugin21HDmpveCoreInnerPlugin");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x785e4", "C0035FD6", "_ZN6HDmpve12FileUploaderD1Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7eb1c", "C0035FD6", "setApolloSendBufferCallback");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x980b8", "C0035FD6", "_ZN6HDmpve6Plugin14TraceIdManager14genFa");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6db80", "C0035FD6", "_ZN5ABase14UploadTaskImpl11SetListenerEP");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x958dc", "C0035FD6", "_ZN6HDmpve6Plugin12TraceService12FlushCo");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x65680", "C0035FD6", "_ZN5ABase14ACheckLogLevelEN6HDmpve12ALog");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x752a8", "C0035FD6", "_ZN5ABase15ANetworkChecker22NotifyTraceR");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7eb04", "C0035FD6", "setApolloSendResultCallback");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x96a50", "C0035FD6", "_ZN6HDmpve6Plugin11SpanContext6SetTagEPK");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9a120", "C0035FD6", "_ZN6HDmpve6Plugin20HDmpveCoreLogSerivce1");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x52f64", "C0035FD6", "_ZN6HDmpve5ABase20oi_symmetry_decrypt2EP");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6f304", "C0035FD6", "_ZN17COSUploadTaskImpl17SetCredentialInf");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7d078", "C0035FD6", "_ZN6HDmpve22HDmpveCoreRemoteConfig6GetIn");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6fb40", "C0035FD6", "_ZN5ABase14UploadTaskImpl5_initEv");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x94bb4", "C0035FD6", "_ZN6HDmpve6Plugin12TraceServiceD1Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x50fe0", "C0035FD6", "_ZN5ABase16_tagApolloAction8ReadFromERNS");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x99004", "C0035FD6", "_ZThn24_N6HDmpve6Plugin21HDmpveCoreInner");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7f918", "C0035FD6", "_ZN8ABaseJVM19GetCUploadTaskClassEv");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x98fd8", "C0035FD6", "_ZThn32_N6HDmpve6Plugin21HDmpveCoreInner");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x76ac8", "C0035FD6", "_ZN6HDmpve22ConfigObserverDispatchC2EPNS");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x51020", "C0035FD6", "_ZNK5ABase22_tagApolloBufferBuffer6encod");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x52bf4", "C0035FD6", "_ZN6HDmpve5ABase24oi_symmetry_encrypt2_l");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x48fbc", "C0035FD6", "_ZN5ABase13Md5HashBufferEPcPKci");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6e580", "C0035FD6", "_ZThn88_N5ABase14UploadTaskImplD0Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9a990", "C0035FD6", "_ZN6HDmpve6Plugin23HDmpveCoreReportServi");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x93050", "C0035FD6", "_ZN6HDmpve6Plugin13PluginManager11PreShu");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x77228", "C0035FD6", "_ZN6HDmpve16RemoteGameConfigC1EPKc");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9264c", "C0035FD6", "_Z22hdmpvecore_get_versionv");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x78488", "C0035FD6", "_ZN6HDmpve12FileUploader10OnFinishedEP14");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x967e0", "C0035FD6", "_ZN6HDmpve6Plugin11SpanContext6GetTagEPK");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x75f08", "C0035FD6", "_ZN5ABase15ANetworkChecker30HandleNSLook");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x976d8", "C0035FD6", "_ZN5ABase19CApolloBufferReader4ReadI7ASt");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6de10", "C0035FD6", "_ZN5ABase10UploadTaskD2Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x93040", "C0035FD6", "_ZN6HDmpve6Plugin13PluginManager17GetAll");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x768a4", "C0035FD6", "Java_com_gcore_abase_TenX_nativeNSLookup");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x512a4", "C0035FD6", "_ZN5ABase22_tagApolloBufferBufferD2Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x99cc8", "C0035FD6", "_ZN6HDmpve6Plugin19RemoteConfigSerivceD0");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7b738", "C0035FD6", "_ZNSt8_Rb_treeISsSt4pairIKSsPN6HDmpve17C");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x64bf4", "C0035FD6", "_ZN5ABase6Logger3LogEN6HDmpve12ALogPrior");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x76eb0", "C0035FD6", "_ZN6HDmpve16RemoteGameConfig9SetValuesEN");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x71484", "C0035FD6", "_ZN5ABase10UrlRequest10UploadFileEPKciii");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9a2dc", "C0035FD6", "_ZN6HDmpve6Plugin20HDmpveCoreLogSerivce1");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7eb10", "C0035FD6", "setApolloSendResultStructCallback");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7bd24", "C0035FD6", "_ZN6HDmpve13ConfigureImpl7RequestEv");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x51384", "C0035FD6", "_ZNK5ABase22_tagApolloBufferBuffer5Clone");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x94714", "C0035FD6", "_ZNSt8_Rb_treeISsSt4pairIKSsPN6HDmpve6Pl");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7a004", "C0035FD6", "_ZN6HDmpve14StringIteratorC1Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x52764", "C0035FD6", "_ZN6HDmpve5ABase19oi_symmetry_encryptEPK");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9a15c", "C0035FD6", "_ZNSt8_Rb_treeISsSt4pairIKSsPN6HDmpve7IL");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x96e38", "C0035FD6", "_ZN6HDmpve6Plugin11SpanContextC2EPKc");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x95f48", "C0035FD6", "_ZN6HDmpve6Plugin11SpanContext18GetEncod");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x76a6c", "C0035FD6", "_ZN6HDmpve22ConfigObserverDispatch20OnCo");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x944c0", "C0035FD6", "_ZN6HDmpve6Plugin12TraceService15Release");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7cd80", "C0035FD6", "_ZN6HDmpve13ConfigureImplD1Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x98f40", "C0035FD6", "_ZNK6HDmpve6Plugin21HDmpveCoreInnerPlugi");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7eb28", "C0035FD6", "setApolloSendResultBufferCallback");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9a1b0", "C0035FD6", "_ZN6HDmpve6Plugin20HDmpveCoreLogSerivceD");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7c9a8", "C0035FD6", "_ZN6HDmpve13ConfigureImplC2Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x763e4", "C0035FD6", "_ZN5ABase15ANetworkChecker10TraceRouteEP");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x751d0", "C0035FD6", "_ZN5ABase15INetworkChecker11GetInstanceE");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x94198", "C0035FD6", "_ZN6HDmpve6Plugin12TraceService15_destro");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9a5fc", "C0035FD6", "_ZNK6HDmpve6Plugin23HDmpveCoreReportServ");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7f910", "C0035FD6", "_ZN8ABaseJVM19SetCUploadTaskClassEP7_jcl");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6fdf4", "C0035FD6", "_ZThn96_N5ABase14UploadTaskImpl11onTaskB");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x50fd8", "C0035FD6", "_ZN5ABase22_tagApolloBufferBuffer8ReadFr");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x798a0", "C0035FD6", "_ZN6HDmpve6Plugin9SingletonINS0_21HDmpve");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x98f8c", "C0035FD6", "_ZN6HDmpve6Plugin21HDmpveCoreInnerPlugin");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9a614", "C0035FD6", "_ZN6HDmpve6Plugin23HDmpveCoreReportServi");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x98fbc", "C0035FD6", "_ZN6HDmpve6Plugin21HDmpveCoreInnerPlugin");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6d854", "C0035FD6", "_ZN5ABase14UploadTaskImpl6ResumeEv");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7a200", "C0035FD6", "_ZNSt8_Rb_treeIPN6HDmpve20RemoteConfigOb");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9900c", "C0035FD6", "_ZThn32_N6HDmpve6Plugin21HDmpveCoreInner");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x94300", "C0035FD6", "_ZN6HDmpve6Plugin12TraceService7IsReadyE");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x78628", "C0035FD6", "_ZN6HDmpve12FileUploaderC1Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9627c", "C0035FD6", "_ZN6HDmpve6Plugin11SpanContextD2Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x50fdc", "C0035FD6", "_ZNK5ABase16_tagApolloAction7WriteToERNS");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x77618", "C0035FD6", "_ZN6HDmpve16RemoteGameConfig7GetBoolEPKc");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6de5c", "C0035FD6", "_ZTv0_n24_N5ABase10UploadTaskD0Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7ce84", "C0035FD6", "_ZN6HDmpve22HDmpveCoreRemoteConfigC2Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x5183c", "C0035FD6", "_ZN5ABase19CApolloBufferReader4ReadEPNS_");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x79d58", "C0035FD6", "_ZN6HDmpve14StringIterator5IsEndEv");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7ab1c", "C0035FD6", "_ZN6HDmpve13ConfigureImpl9GetStringEPKcS");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9a5f8", "C0035FD6", "_ZN6HDmpve6Plugin23HDmpveCoreReportServi");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x51888", "C0035FD6", "_ZN5ABase19CApolloBufferReader4ReadERNS_");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x5126c", "C0035FD6", "_ZN5ABase16_tagApolloActionD2Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x75474", "C0035FD6", "_ZN5ABase15ANetworkChecker26HandlePingRe");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7c250", "C0035FD6", "_ZN6HDmpve13ConfigureImpl18OnDataTaskFin");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x79f74", "C0035FD6", "_ZN6HDmpve12RemoteConfig9GetDoubleEPKcd");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x99f40", "C0035FD6", "_ZN6HDmpve6Plugin19RemoteConfigSerivce15");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6e53c", "C0035FD6", "_ZThn88_N5ABase14UploadTaskImplD1Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x930e8", "C0035FD6", "_ZN6HDmpve6Plugin20_tagPluginDescriptorD");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x99e10", "C0035FD6", "_ZNSt8_Rb_treeISsSt4pairIKSsPN6HDmpve13I");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x74f98", "C0035FD6", "_ZNK5ABase17TraceRouteMessage7WriteToERN");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6e544", "C0035FD6", "_ZThn24_N5ABase14UploadTaskImplD1Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x966fc", "C0035FD6", "_ZN6HDmpve6Plugin11SpanContext3GetEPKc");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x932d4", "C0035FD6", "_ZN6HDmpve6Plugin13PluginManagerD2Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x75158", "C0035FD6", "_ZN5ABase15ANetworkCheckerD0Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7c8bc", "C0035FD6", "_ZN6HDmpve13ConfigureImpl9GetDoubleEPKcS");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x98a30", "C0035FD6", "_ZN6HDmpve6Plugin14TraceIdManager22appen");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x767d8", "C0035FD6", "_ZThn16_N5ABase15ANetworkChecker8NSLooku");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x94190", "C0035FD6", "_ZNK6HDmpve6Plugin12TraceService9IsEnabl");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x750e0", "C0035FD6", "_ZN5ABase15ANetworkCheckerC1Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6f0b4", "C0035FD6", "_ZN5ABase14UploadTaskImpl15onWorkingThre");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7a1e4", "C0035FD6", "_ZN6HDmpve14ConfigObserverC2EPNS_12Remot");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7ce50", "C0035FD6", "_ZThn8_N6HDmpve13ConfigureImplD1Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9a974", "C0035FD6", "_ZN6HDmpve6Plugin23HDmpveCoreReportServi");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6d7d0", "C0035FD6", "_ZN5ABase14UploadTaskImpl17SaveUploading");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7a2a4", "C0035FD6", "_ZN6HDmpve12RemoteConfig14RemoveObserver");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x79d84", "C0035FD6", "_ZN6HDmpve14ConfigObserver20OnConfigureR");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6b288", "C0035FD6", "_ZN5ABase22_tagADetailNetworkInfo8ReadFr");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x51984", "C0035FD6", "_ZN5ABase19CApolloBufferWriter5WriteEPKN");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7314c", "C0035FD6", "ABase_system_CheckPermission");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6f2ec", "C0035FD6", "_ZN17COSUploadTaskImpl11SetListenerEPN14");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6d92c", "C0035FD6", "_ZThn96_N5ABase14UploadTaskImpl6FinishEv");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x86e30", "C0035FD6", "_ZN5AData6AssignEPKhi");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7eb34", "C0035FD6", "setApolloSendResultStructBufferCallback");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x95b44", "C0035FD6", "_ZN6HDmpve6Plugin12TraceService13ReportC");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x98f58", "C0035FD6", "_ZThn32_NK6HDmpve6Plugin21HDmpveCoreInne");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x94184", "C0035FD6", "_ZNK6HDmpve6Plugin12TraceService13GetPlu");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x98510", "C0035FD6", "_ZN6HDmpve6Plugin14TraceIdManagerC2Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x71f1c", "C0035FD6", "Java_com_gcore_abase_URLRequest_nativeSa");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9b09c", "C0035FD6", "Java_com_gcore_hdmpve_plugin_PluginUtils");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7aafc", "C0035FD6", "_ZN6HDmpve13ConfigureImpl7GetBoolEPKcS2_");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9ad68", "C0035FD6", "_ZN6HDmpve6Plugin14ServiceManager11GetIn");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x512a4", "C0035FD6", "_ZN5ABase22_tagApolloBufferBufferD1Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6f370", "C0035FD6", "_ZN14COSCUploadTaskD2Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x519d0", "C0035FD6", "_ZN5ABase19CApolloBufferWriter5WriteEPNS");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7aabc", "C0035FD6", "_ZN6HDmpve13ConfigureImpl6GetIntEPKcS2_i");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7baac", "C0035FD6", "_ZN6HDmpve13ConfigureImpl11AddObserverEP");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x79db8", "C0035FD6", "_ZN6HDmpve12RemoteConfig7GetBoolEPKcb");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x98f84", "C0035FD6", "_ZThn32_N6HDmpve6Plugin21HDmpveCoreInner");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7513c", "C0035FD6", "_ZN5ABase15ANetworkCheckerD2Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6b32c", "C0035FD6", "_ZN5ABase19CApolloBufferWriter5WriteERK7");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7ec98", "C0035FD6", "_ZN5ABase15CPlatformObject15SendUnityRes");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x52c20", "C0035FD6", "_ZN6HDmpve5ABase20oi_symmetry_encrypt2EP");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x79e4c", "C0035FD6", "_ZN6HDmpve12RemoteConfig6GetIntEPKci");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x75950", "C0035FD6", "_ZN5ABase15ANetworkChecker32HandleTraceR");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7ce84", "C0035FD6", "_ZN6HDmpve22HDmpveCoreRemoteConfigC1Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x93850", "C0035FD6", "_ZNSt8_Rb_treeISsSt4pairIKSsN6HDmpve6Plu");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6d84c", "C0035FD6", "_ZThn96_N5ABase14UploadTaskImpl17SaveUpl");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7ce58", "C0035FD6", "_ZN6HDmpve13ConfigureImplD0Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6dbe8", "C0035FD6", "_ZThn88_N5ABase14UploadTaskImpl11SetList");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x93368", "C0035FD6", "_ZN6HDmpve6Plugin13PluginManagerC1Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x78588", "C0035FD6", "_ZThn8_N6HDmpve12FileUploader10OnFinishe");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7c9a8", "C0035FD6", "_ZN6HDmpve13ConfigureImplC1Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x99068", "C0035FD6", "_ZN6HDmpve6Plugin21HDmpveCoreInnerPlugin");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x51188", "C0035FD6", "_ZN5ABase22_tagApolloBufferBuffer6decode");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7eaf8", "C0035FD6", "setApolloSendStructCallback");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6f374", "C0035FD6", "_ZN17COSUploadTaskImplD1Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7d0a4", "C0035FD6", "_ZN6HDmpve22HDmpveCoreRemoteConfig7GetLo");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x511f4", "C0035FD6", "_ZN5ABase22_tagApolloBufferBuffer6decode");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6d668", "C0035FD6", "_ZN5ABase3WWW14UploadFromFileEPKcS2_");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6e54c", "C0035FD6", "_ZN5ABase14UploadTaskImplD0Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x96bdc", "C0035FD6", "_ZN6HDmpve6Plugin11SpanContext11_FromStr");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9a20c", "C0035FD6", "_ZN6HDmpve6Plugin20HDmpveCoreLogSerivceC");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7503c", "C0035FD6", "_ZNK5ABase16TraceRouteResult7WriteToERNS");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6fd48", "C0035FD6", "_ZTv0_n56_N5ABase14UploadTaskImpl6Cancel");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6de28", "C0035FD6", "_ZTv0_n24_N5ABase10UploadTaskD1Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7a340", "C0035FD6", "_ZNSt8_Rb_treeIPN6HDmpve20RemoteConfigOb");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6fc80", "C0035FD6", "_ZThn88_N5ABase14UploadTaskImpl5PauseEv");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x70730", "C0035FD6", "_Z12COSUploadJNIPKcS0_xS0_PvS1_S0_");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7eb74", "C0035FD6", "setApolloSendVoidMethodCallback");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7ed28", "C0035FD6", "_ZN5ABase15CPlatformObject15SendUnityMet");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x94c2c", "C0035FD6", "_ZN6HDmpve6Plugin12TraceServiceD0Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x94768", "C0035FD6", "_ZNSt8_Rb_treeISsSt4pairIKSsPN6HDmpve6Pl");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x79ee0", "C0035FD6", "_ZN6HDmpve12RemoteConfig7GetLongEPKcl");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7a59c", "C0035FD6", "_ZN6HDmpve12RemoteConfigC2EPKc");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6b108", "C0035FD6", "_ZN5ABase19CApolloBufferReader4ReadERi");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x765ec", "C0035FD6", "_ZN5ABase15ANetworkChecker8NSLookupEPKci");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7cd80", "C0035FD6", "_ZN6HDmpve13ConfigureImplD2Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x93330", "C0035FD6", "_ZN6HDmpve6Plugin13PluginManager15Releas");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7ae3c", "C0035FD6", "_ZN6HDmpve13ConfigureImpl6SetUrlEPKc");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x77a14", "C0035FD6", "_ZN6HDmpve16RemoteGameConfigD0Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7ec50", "C0035FD6", "_ZN5ABase15CPlatformObject15SendUnityStr");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x79d24", "C0035FD6", "_ZN6HDmpve14StringIterator4NextEv");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9536c", "C0035FD6", "_ZN6HDmpve6Plugin12TraceServiceC2Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x944f4", "C0035FD6", "_ZN6HDmpve6Plugin12TraceService14_report");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x943cc", "C0035FD6", "_ZN6HDmpve6Plugin12TraceService11isTarge");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6d6cc", "C0035FD6", "_ZN5ABase3WWW16GetCOSUploadTaskEv");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x777c8", "C0035FD6", "_ZN6HDmpve16RemoteGameConfig9GetDoubleEP");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x99dbc", "C0035FD6", "_ZN6HDmpve6Plugin19RemoteConfigSerivce11");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6f2f4", "C0035FD6", "_ZN17COSUploadTaskImpl11GetUploadidEv");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x8b474", "C0035FD6", "_ZNSt11_Deque_baseIN10ABase_Json6Reader9");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7aadc", "C0035FD6", "_ZN6HDmpve13ConfigureImpl7GetLongEPKcS2_");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6f374", "C0035FD6", "_ZN17COSUploadTaskImplD2Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6fdfc", "C0035FD6", "_ZN5ABase14UploadTaskImpl14onTaskProgres");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x93d38", "C0035FD6", "_ZN6HDmpve6Plugin13PluginManager7Install");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x78bc8", "C0035FD6", "_ZN6HDmpve12FileUploader7_UploadEPKcS2_i");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x79820", "C0035FD6", "_ZN6HDmpve6Plugin9SingletonINS0_21HDmpve");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x778a4", "C0035FD6", "_ZN6HDmpve16RemoteGameConfig9GetStringEP");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x77434", "C0035FD6", "_ZN6HDmpve16RemoteGameConfig11GetInstanc");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6f400", "C0035FD6", "_ZN17COSUploadTaskImplC1Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7ece0", "C0035FD6", "_ZN5ABase15CPlatformObject15SendUnityBuf");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6e588", "C0035FD6", "_ZThn24_N5ABase14UploadTaskImplD0Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6dfb4", "C0035FD6", "_ZN5ABase14UploadTaskImpl7DestroyEv");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x5112c", "C0035FD6", "_ZNK5ABase26_tagApolloActionBufferBase12");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7536c", "C0035FD6", "_ZN5ABase15ANetworkChecker20NotifyNSLook");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9a638", "C0035FD6", "_ZN6HDmpve6Plugin23HDmpveCoreReportServi");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6e52c", "C0035FD6", "_ZTv0_n24_N5ABase14UploadTaskImplD1Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7ec74", "C0035FD6", "_ZN5ABase15CPlatformObject15SendUnityRes");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7aa00", "C0035FD6", "_ZN6HDmpve12RemoteConfigD2Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x878e8", "C0035FD6", "hdmpvecore_set_spid");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9accc", "C0035FD6", "_ZN6HDmpve6Plugin14ServiceManagerC2Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x947e8", "C0035FD6", "_ZN6HDmpve6Plugin12TraceService4_popEPKc");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x99bf0", "C0035FD6", "_ZNK6HDmpve6Plugin19RemoteConfigSerivce7");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9877c", "C0035FD6", "_ZN6HDmpve6Plugin14TraceIdManagerD2Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x95888", "C0035FD6", "_ZN6HDmpve6Plugin12TraceService11GetInst");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x77088", "C0035FD6", "_ZN6HDmpve16RemoteGameConfig10ReadString");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x51764", "C0035FD6", "_ZN5ABase20_tagApolloBufferBase6decodeEP");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7ce7c", "C0035FD6", "_ZThn8_N6HDmpve13ConfigureImplD0Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9a974", "C0035FD6", "_ZN6HDmpve6Plugin23HDmpveCoreReportServi");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7af78", "C0035FD6", "_ZN6HDmpve13ConfigureImpl5ClearEv");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6fc08", "C0035FD6", "_ZN5ABase14UploadTaskImpl7_uninitEv");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x98510", "C0035FD6", "_ZN6HDmpve6Plugin14TraceIdManagerC1Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x964b4", "C0035FD6", "_ZN6HDmpve6Plugin11SpanContextC1EPKcS3_S");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9a608", "C0035FD6", "_ZNK6HDmpve6Plugin23HDmpveCoreReportServ");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x524ec", "C0035FD6", "_ZN6HDmpve5ABase13TeaDecryptECBEPKcS2_Pc");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9a1b0", "C0035FD6", "_ZN6HDmpve6Plugin20HDmpveCoreLogSerivceD");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x50fd0", "C0035FD6", "_ZN5ABase20_tagApolloBufferBase12BeforeD");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6de38", "C0035FD6", "_ZN5ABase10UploadTaskD0Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x77544", "C0035FD6", "_ZN6HDmpve19GetGameRemoteConfigEv");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x964b4", "C0035FD6", "_ZN6HDmpve6Plugin11SpanContextC2EPKcS3_S");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7a59c", "C0035FD6", "_ZN6HDmpve12RemoteConfigC1EPKc");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7a6b4", "C0035FD6", "_ZN6HDmpve21GetHDmpveRemoteConfigEv");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x95c1c", "C0035FD6", "_ZN6HDmpve6Plugin12TraceService14Destroy");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x79bd0", "C0035FD6", "_ZN6HDmpve15ConfigureReport18ReportUploa");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x69d74", "C0035FD6", "_ZNKSt6vectorIPN5ABase9OperationESaIS2_E");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x51280", "C0035FD6", "_ZN5ABase16_tagApolloActionD0Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7d024", "C0035FD6", "_ZN6HDmpve22HDmpveCoreRemoteConfig11GetI");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x97f44", "C0035FD6", "_ZN6HDmpve6Plugin14TraceIdManager10GetTr");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x984cc", "C0035FD6", "_ZN6HDmpve6Plugin16_tagTraceIdStoreD1Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9ab7c", "C0035FD6", "_ZN6HDmpve6Plugin14ServiceManagerD1Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9a108", "C0035FD6", "_ZNK6HDmpve6Plugin20HDmpveCoreLogSerivce");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x76d90", "C0035FD6", "_ZN6HDmpve16RemoteGameConfig14RemoveObse");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7b0e4", "C0035FD6", "_ZN6HDmpve13ConfigureImpl16_ParseJsonObj");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6e520", "C0035FD6", "_ZN5ABase14UploadTaskImplD1Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x58db0", "C0035FD6", "_ZN5ABase11uECC_verifyEPKhS1_jS1_PKNS_12");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6d890", "C0035FD6", "_ZThn88_N5ABase14UploadTaskImpl6ResumeEv");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x78dcc", "C0035FD6", "_ZN6HDmpve12FileUploader20OnConfigureRef");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x50fd4", "C0035FD6", "_ZNK5ABase22_tagApolloBufferBuffer7Write");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x98fb4", "C0035FD6", "_ZThn32_N6HDmpve6Plugin21HDmpveCoreInner");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x751e8", "C0035FD6", "_ZN5ABase15ANetworkChecker16NotifyPingFi");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x907c4", "C0035FD6", "_ZNKSt6vectorIN10ABase_Json12PathArgumen");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x93684", "C0035FD6", "_ZN6HDmpve6Plugin13PluginManager9Uninsta");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7abe0", "C0035FD6", "_ZN6HDmpve13ConfigureImpl21OnNetworkStat");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9309c", "C0035FD6", "_ZN6HDmpve6Plugin13PluginManager8Shutdow");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6dc54", "C0035FD6", "_ZThn88_N5ABase14UploadTaskImpl14RemoveL");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x761dc", "C0035FD6", "_ZN5ABase15ANetworkChecker4PingEPKciPFvR");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x76d7c", "C0035FD6", "_ZN6HDmpve22ConfigObserverDispatch13Obse");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x50fcc", "C0035FD6", "_ZNK5ABase20_tagApolloBufferBase12Before");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x516a4", "C0035FD6", "_ZN5ABase20_tagApolloBufferBase6decodeER");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x99c90", "C0035FD6", "_ZN6HDmpve6Plugin19RemoteConfigSerivceD1");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x99cec", "C0035FD6", "_ZN6HDmpve6Plugin19RemoteConfigSerivceC1");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x96f98", "C0035FD6", "_ZNK6HDmpve6Plugin16_tagTraceIdStore7Wri");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x51088", "C0035FD6", "_ZN5ABase26_tagApolloActionBufferBase12B");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x65610", "C0035FD6", "_ZN5ABase6Logger11SetLogLevelEN6HDmpve12");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x779d0", "C0035FD6", "_ZN6HDmpve16RemoteGameConfigD1Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7513c", "C0035FD6", "_ZN5ABase15ANetworkCheckerD1Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6fd0c", "C0035FD6", "_ZN5ABase14UploadTaskImpl6CancelEv");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x78704", "C0035FD6", "_ZN6HDmpve12FileUploader11GetInstanceEv");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x74e54", "C0035FD6", "_ZN5ABase10PingResult8ReadFromERNS_19CAp");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6f3b8", "C0035FD6", "_ZN17COSUploadTaskImplD0Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x750e0", "C0035FD6", "_ZN5ABase15ANetworkCheckerC2Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6dbf0", "C0035FD6", "_ZN5ABase14UploadTaskImpl14RemoveListene");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x79820", "C0035FD6", "_ZN6HDmpve6Plugin9SingletonINS0_21HDmpve");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9a5f8", "C0035FD6", "_ZN6HDmpve6Plugin23HDmpveCoreReportServi");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6f0a0", "C0035FD6", "_ZN5ABase14UploadTaskImplC1EPKcS2_");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x97844", "C0035FD6", "_ZN6HDmpve6Plugin14TraceIdManager11genTr");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9877c", "C0035FD6", "_ZN6HDmpve6Plugin14TraceIdManagerD1Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x99c90", "C0035FD6", "_ZN6HDmpve6Plugin19RemoteConfigSerivceD2");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x98fd0", "C0035FD6", "_ZThn24_N6HDmpve6Plugin21HDmpveCoreInner");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x99c3c", "C0035FD6", "_ZNSt8_Rb_treeISsSt4pairIKSsPN6HDmpve13I");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x525bc", "C0035FD6", "_ZN6HDmpve5ABase14TeaEncryptECB3EPKcS2_P");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x51314", "C0035FD6", "_ZNK5ABase16_tagApolloAction5CloneEv");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7b8a0", "C0035FD6", "_ZN6HDmpve13ConfigureImpl20OnConfigureRe");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x529f0", "C0035FD6", "_ZN6HDmpve5ABase19oi_symmetry_decryptEPK");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7a24c", "C0035FD6", "_ZNSt8_Rb_treeIPN6HDmpve20RemoteConfigOb");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x99c08", "C0035FD6", "_ZN6HDmpve6Plugin19RemoteConfigSerivce15");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7b7b8", "C0035FD6", "_ZN6HDmpve13ConfigureImpl14RemoveObserve");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9a864", "C0035FD6", "_ZN6HDmpve6Plugin23HDmpveCoreReportServi");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9a154", "C0035FD6", "_ZN6HDmpve6Plugin20HDmpveCoreLogSerivce1");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x650d8", "C0035FD6", "_ZN5ABase23PrintLogToConsoleFormatEN6HDm");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x95cf8", "C0035FD6", "_ZN6HDmpve6Plugin11SpanContext8ToStringE");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x74fcc", "C0035FD6", "_ZN5ABase19CApolloBufferWriter5WriteERK6");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7b97c", "C0035FD6", "_ZNSt8_Rb_treeISsSt4pairIKSsPN6HDmpve17C");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x98f4c", "C0035FD6", "_ZThn32_NK6HDmpve6Plugin21HDmpveCoreInne");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x515e0", "C0035FD6", "_ZN5ABase7ConvertEPKNS_20_tagApolloBuffe");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9ab7c", "C0035FD6", "_ZN6HDmpve6Plugin14ServiceManagerD2Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x75094", "C0035FD6", "_ZNK5ABase14NSLookupResult7WriteToERNS_1");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x654a8", "C0035FD6", "_ZN5ABase16SetABaseLogLevelEN6HDmpve12AL");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x589f8", "C0035FD6", "_ZN5ABase9uECC_signEPKhS1_jPhPKNS_12uECC");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x653b0", "C0035FD6", "_ZN5ABase4XLogEN6HDmpve12ALogPriorityEPK");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x97834", "C0035FD6", "_ZN6HDmpve6Plugin16_tagTraceIdStore8Read");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6d764", "C0035FD6", "_ZN5ABase14UploadTaskImpl15RemoveCacheDa");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x74edc", "C0035FD6", "_ZN5ABase17TraceRouteMessage8ReadFromERN");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x97300", "C0035FD6", "_ZN6HDmpve6Plugin16_tagTraceIdStoreC1Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6fea0", "C0035FD6", "_ZThn96_N5ABase14UploadTaskImpl14onTaskP");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7d0d0", "C0035FD6", "_ZN6HDmpve22HDmpveCoreRemoteConfig9GetDo");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x98bfc", "C0035FD6", "_ZN6HDmpve6Plugin14TraceIdManager19pop1T");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9accc", "C0035FD6", "_ZN6HDmpve6Plugin14ServiceManagerC1Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x948e8", "C0035FD6", "_ZN6HDmpve6Plugin12TraceService7_insertE");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6f2fc", "C0035FD6", "_ZN17COSUploadTaskImpl17GetUploadFilePat");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x984cc", "C0035FD6", "_ZN6HDmpve6Plugin16_tagTraceIdStoreD2Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x97a14", "C0035FD6", "_ZN6HDmpve6Plugin14TraceIdManager18reque");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6fd58", "C0035FD6", "_ZN5ABase14UploadTaskImpl11onTaskBeganEl");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9536c", "C0035FD6", "_ZN6HDmpve6Plugin12TraceServiceC1Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x968c4", "C0035FD6", "_ZN6HDmpve6Plugin11SpanContext3SetEPKcS3");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9a940", "C0035FD6", "_ZN6HDmpve6Plugin23HDmpveCoreReportServi");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6ff4c", "C0035FD6", "_ZThn96_N5ABase14UploadTaskImpl14onTaskF");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x58ad0", "C0035FD6", "_ZN5ABase23uECC_sign_deterministicEPKhS1");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x87674", "C0035FD6", "_ZNSt8_Rb_treeISsSt4pairIKSsN6HDmpve6Plu");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x79a5c", "C0035FD6", "_ZN6HDmpve15ConfigureReport23ReportRecie");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6f3dc", "C0035FD6", "_ZN14COSCUploadTaskD0Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x518d0", "C0035FD6", "_ZN5ABase19CApolloBufferWriter5WriteEi");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6f370", "C0035FD6", "_ZN14COSCUploadTaskD1Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6f30c", "C0035FD6", "_ZN17COSUploadTaskImpl6UploadEPKcS1_xS1_");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6f084", "C0035FD6", "_ZN5ABase14UploadTaskImplC2EPKcS2_");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9a114", "C0035FD6", "_ZNK6HDmpve6Plugin20HDmpveCoreLogSerivce");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x76988", "C0035FD6", "Java_com_gcore_abase_TenX_nativePingFini");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x767e0", "C0035FD6", "Java_com_gcore_abase_TenX_nativeTraceRou");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x98758", "C0035FD6", "_ZN6HDmpve6Plugin16_tagTraceIdStoreD0Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x512f0", "C0035FD6", "_ZN5ABase22_tagApolloBufferBufferD0Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x51530", "C0035FD6", "_ZNK5ABase20_tagApolloBufferBase6encodeE");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x65938", "C0035FD6", "_ZN5ABase6Logger3LogEN6HDmpve12ALogPrior");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x776f8", "C0035FD6", "_ZN6HDmpve16RemoteGameConfig7GetLongEPKc");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x711c4", "C0035FD6", "_ZN5ABase10UrlRequest17SetUploadDelegate");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x93048", "C0035FD6", "_ZN6HDmpve6Plugin13PluginManager19GetPlu");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x87404", "C0035FD6", "hdmpvecore_set_gameid");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7ec2c", "C0035FD6", "_ZN5ABase15CPlatformObject16SendUnityMes");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x98f54", "C0035FD6", "_ZNK6HDmpve6Plugin21HDmpveCoreInnerPlugi");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6c574", "C0035FD6", "Java_com_gcore_abase_TenX_nativeNetworkS");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x76ac8", "C0035FD6", "_ZN6HDmpve22ConfigObserverDispatchC1EPNS");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x94bb4", "C0035FD6", "_ZN6HDmpve6Plugin12TraceServiceD2Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7517c", "C0035FD6", "_ZN5ABase15ANetworkChecker11GetInstanceE");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6d7a8", "C0035FD6", "_ZThn96_N5ABase14UploadTaskImpl15RemoveC");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x785e4", "C0035FD6", "_ZN6HDmpve12FileUploaderD2Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x76d14", "C0035FD6", "_ZN6HDmpve22ConfigObserverDispatch14Remo");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6de1c", "C0035FD6", "_ZN5ABase10UploadTaskD1Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6f400", "C0035FD6", "_ZN17COSUploadTaskImplC2Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x518f4", "C0035FD6", "_ZN5ABase19CApolloBufferWriter5WriteERKN");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x6e570", "C0035FD6", "_ZTv0_n24_N5ABase14UploadTaskImplD0Ev");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7ac4c", "C0035FD6", "_ZThn8_N6HDmpve13ConfigureImpl21OnNetwor");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x74f10", "C0035FD6", "_ZNK5ABase10PingResult7WriteToERNS_19CAp");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9a20c", "C0035FD6", "_ZN6HDmpve6Plugin20HDmpveCoreLogSerivceC");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x98f60", "C0035FD6", "_ZN6HDmpve6Plugin21HDmpveCoreInnerPlugin");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x933fc", "C0035FD6", "_ZN6HDmpve6Plugin13PluginManager11GetIns");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x79824", "C0035FD6", "_ZN6HDmpve6Plugin9SingletonINS0_21HDmpve");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x9735c", "C0035FD6", "_ZNK6HDmpve6Plugin16_tagTraceIdStore5Clo");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7eaec", "C0035FD6", "setApolloSendMessageCallback");
    SAFE_PATCH_LIB("libhdmpvecore.so", "0x7ccd8", "C0035FD6", "hdmpvecore_configure_seturl");

    LOGI("[@OWNERHUBEE] HDmpveCore patches complete");
}

// ============================================================
// PHASE 3: libhdmpve.so PATCHES
// ============================================================
void applyHdmpvePatches() {
    if (!g_Settings.bHdmpvePatches) {
        logStatus("[DISABLED] HDmpve patches skipped by settings");
        return;
    }
    
    logStatus("\n=== PHASE 3: libhdmpve.so PATCHES ===");
    
    SAFE_PATCH_LIB("libhdmpve.so", "0xc8898", "C0035FD6", "hdmpve_custom_account_adapter_install");
    SAFE_PATCH_LIB("libhdmpve.so", "0x3d94cc", "C0035FD6", "hdmpve_gromelink_getVersion");
    SAFE_PATCH_LIB("libhdmpve.so", "0x13d18c", "C0035FD6", "Java_com_gcore_hdmpve_netinterface_MsgWo");
    SAFE_PATCH_LIB("libhdmpve.so", "0xb9514", "C0035FD6", "hdmpve_pay_Dipose");
    SAFE_PATCH_LIB("libhdmpve.so", "0xc1724", "C0035FD6", "_ZN6HDmpve15IAccountServiceD0Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x12fb5c", "C0035FD6", "_ZN6HDmpve4Conn13NameRouteInfoD0Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x10e074", "C0035FD6", "hdmpve_releaseInstance");
    SAFE_PATCH_LIB("libhdmpve.so", "0xc7e80", "C0035FD6", "_ZN6HDmpve5TokenD0Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0xbbf5c", "C0035FD6", "_ZNSt12_Vector_baseIPN6HDmpve16IServiceO");
    SAFE_PATCH_LIB("libhdmpve.so", "0x1b3ac0", "C0035FD6", "_ZN15ChannelInfoUtil13isV2SignatureEPKc");
    SAFE_PATCH_LIB("libhdmpve.so", "0x179acc", "C0035FD6", "_ZN6HDmpve13ZoneRouteInfoD1Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0xb7cf8", "C0035FD6", "_ZN6HDmpve14IPluginManager15ReleaseInsta");
    SAFE_PATCH_LIB("libhdmpve.so", "0x3c6e98", "C0035FD6", "_ZN6HDmpve5ITDirC1Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0xb92e8", "C0035FD6", "hdmpve_pay_action");
    SAFE_PATCH_LIB("libhdmpve.so", "0x179a64", "C0035FD6", "_ZN6HDmpve11WaitingInfoD2Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0xb83bc", "C0035FD6", "_ZN6HDmpve15CSdkFactoryBaseC2Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x17cb0c", "C0035FD6", "_ZN6HDmpve12RealNameInfoD2Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x1799f4", "C0035FD6", "_ZNK6HDmpve15ServerRouteInfo5CloneEv");
    SAFE_PATCH_LIB("libhdmpve.so", "0xb8718", "C0035FD6", "_ZN6HDmpve12CServiceBase11AddObserverEPN");
    SAFE_PATCH_LIB("libhdmpve.so", "0x13b534", "C0035FD6", "Java_com_gcore_hdmpve_netinterface_MsgWo");
    SAFE_PATCH_LIB("libhdmpve.so", "0xba3c4", "C0035FD6", "hdmpve_account_realname_auth");
    SAFE_PATCH_LIB("libhdmpve.so", "0xb8340", "C0035FD6", "_ZN6HDmpve11CPluginBase17OnApplicationQu");
    SAFE_PATCH_LIB("libhdmpve.so", "0x179a98", "C0035FD6", "_ZN6HDmpve15ServerRouteInfoD2Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x3d9534", "C0035FD6", "hdmpve_gromelink_destorySocket");
    SAFE_PATCH_LIB("libhdmpve.so", "0x12e85c", "C0035FD6", "_ZN6HDmpve4Conn13ZoneRouteInfoD0Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0xba9cc", "C0035FD6", "hdmpve_account_IsPlatformSupportApi");
    SAFE_PATCH_LIB("libhdmpve.so", "0x3d9484", "C0035FD6", "hdmpve_gromelink_getFactory");
    SAFE_PATCH_LIB("libhdmpve.so", "0x179ae0", "C0035FD6", "_ZN6HDmpve13ZoneRouteInfoD0Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x3c680c", "C0035FD6", "hdmpve_tdir_queryall");
    SAFE_PATCH_LIB("libhdmpve.so", "0xb76f8", "C0035FD6", "GetHDmpvePlugin");
    SAFE_PATCH_LIB("libhdmpve.so", "0xbbec0", "C0035FD6", "_ZNSt6vectorIPN6HDmpve16IServiceObserver");
    SAFE_PATCH_LIB("libhdmpve.so", "0xc16fc", "C0035FD6", "_ZN6HDmpve15IAccountServiceD2Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0xb9e34", "C0035FD6", "_ZN6HDmpve11AccountInfoD0Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0xc7cf0", "C0035FD6", "_ZN6HDmpve5TokenD2Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x13098c", "C0035FD6", "_ZNSt11__copy_moveILb0ELb1ESt26random_ac");
    SAFE_PATCH_LIB("libhdmpve.so", "0xb87f8", "C0035FD6", "_ZTv0_n32_N6HDmpve12CServiceBase11AddObs");
    SAFE_PATCH_LIB("libhdmpve.so", "0x2e9b90", "C0035FD6", "CheckAppUpdate");
    SAFE_PATCH_LIB("libhdmpve.so", "0xc1714", "C0035FD6", "_ZTv0_n24_N6HDmpve15IAccountServiceD1Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0xc0d48", "C0035FD6", "_ZN6HDmpve8UserInfoD2Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x179dd0", "C0035FD6", "_ZNK6HDmpve13NameRouteInfo5CloneEv");
    SAFE_PATCH_LIB("libhdmpve.so", "0x3f4ea8", "C0035FD6", "apollo_net_sniffer_SetZSD");
    SAFE_PATCH_LIB("libhdmpve.so", "0x10ebec", "C0035FD6", "Java_com_gcore_hdmpve_qr_QRCodeAPI_genQR");
    SAFE_PATCH_LIB("libhdmpve.so", "0x1b4814", "C0035FD6", "_ZN15ChannelInfoUtil21getV2SignBlockOffs");
    SAFE_PATCH_LIB("libhdmpve.so", "0x1238a4", "C0035FD6", "_ZN6HDmpve13NameRouteInfoD0Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0xb85bc", "C0035FD6", "_ZTv0_n40_N6HDmpve12CServiceBase14Remove");
    SAFE_PATCH_LIB("libhdmpve.so", "0x179acc", "C0035FD6", "_ZN6HDmpve13ZoneRouteInfoD2Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0xb8508", "C0035FD6", "_ZN6HDmpve12CServiceBase14RemoveObserver");
    SAFE_PATCH_LIB("libhdmpve.so", "0x10e0c8", "C0035FD6", "hdmpve_setApolloLogger");
    SAFE_PATCH_LIB("libhdmpve.so", "0xc7cf0", "C0035FD6", "_ZN6HDmpve5TokenD1Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x247a9c", "C0035FD6", "IFSDllVerifyApiCompatable");
    SAFE_PATCH_LIB("libhdmpve.so", "0x12e814", "C0035FD6", "_ZN6HDmpve4Conn15ServerRouteInfoD1Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0xb8330", "C0035FD6", "_ZN6HDmpve11CPluginBase10OnUnistallEv");
    SAFE_PATCH_LIB("libhdmpve.so", "0xb8328", "C0035FD6", "_ZN6HDmpve11CPluginBase9OnInstallEv");
    SAFE_PATCH_LIB("libhdmpve.so", "0x13b084", "C0035FD6", "Java_com_gcore_hdmpve_netinterface_MsgWo");
    SAFE_PATCH_LIB("libhdmpve.so", "0xb7c14", "C0035FD6", "_ZN6HDmpve14IPluginManager11GetInstanceE");
    SAFE_PATCH_LIB("libhdmpve.so", "0x1798ec", "C0035FD6", "_ZNK6HDmpve11WaitingInfo5CloneEv");
    SAFE_PATCH_LIB("libhdmpve.so", "0x10eaec", "C0035FD6", "Java_com_gcore_hdmpve_qr_QRCodeAPI_launc");
    SAFE_PATCH_LIB("libhdmpve.so", "0x3d95f0", "C0035FD6", "hdmpve_gromelink_recv");
    SAFE_PATCH_LIB("libhdmpve.so", "0xb91b4", "C0035FD6", "hdmpve_pay_Pay");
    SAFE_PATCH_LIB("libhdmpve.so", "0xc0d94", "C0035FD6", "_ZN6HDmpve8UserInfoD0Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x3c6e98", "C0035FD6", "_ZN6HDmpve5ITDirC2Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x179a78", "C0035FD6", "_ZN6HDmpve11WaitingInfoD0Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x10e070", "C0035FD6", "hdmpve_getInstance");
    SAFE_PATCH_LIB("libhdmpve.so", "0x3c6eac", "C0035FD6", "_ZN6HDmpve5ITDirD1Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x1ed040", "C0035FD6", "_ZN6HDmpve13DolphinHelper10InstallAPKEPK");
    SAFE_PATCH_LIB("libhdmpve.so", "0x12383c", "C0035FD6", "_ZN6HDmpve13NameRouteInfoD2Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x3d95b8", "C0035FD6", "hdmpve_gromelink_send");
    SAFE_PATCH_LIB("libhdmpve.so", "0xb83bc", "C0035FD6", "_ZN6HDmpve15CSdkFactoryBaseC1Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x10e078", "C0035FD6", "hdmpve_setLogLevel");
    SAFE_PATCH_LIB("libhdmpve.so", "0xb959c", "C0035FD6", "hdmpve_access_getInstance");
    SAFE_PATCH_LIB("libhdmpve.so", "0xb9a38", "C0035FD6", "_ZN6HDmpve6ResultD2Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x10f8c0", "C0035FD6", "_ZN6HDmpve14IQRCodeServiceD1Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0xba764", "C0035FD6", "hdmpve_account_reset");
    SAFE_PATCH_LIB("libhdmpve.so", "0x179b00", "C0035FD6", "_ZNK6HDmpve6Result5CloneEv");
    SAFE_PATCH_LIB("libhdmpve.so", "0xbaa44", "C0035FD6", "hdmpve_account_refreshAtk");
    SAFE_PATCH_LIB("libhdmpve.so", "0xb95c4", "C0035FD6", "hdmpve_access_releaseInstance");
    SAFE_PATCH_LIB("libhdmpve.so", "0xc1eb8", "C0035FD6", "_ZN6HDmpve14SetAccountInfoEPKcmS1_");
    SAFE_PATCH_LIB("libhdmpve.so", "0x17caa0", "C0035FD6", "_ZN6HDmpve18RealNameAuthResultD2Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x10e20c", "C0035FD6", "hdmpve_setUserInfo");
    SAFE_PATCH_LIB("libhdmpve.so", "0xba954", "C0035FD6", "hdmpve_account_IsPlatformInstalled");
    SAFE_PATCH_LIB("libhdmpve.so", "0x10f8ac", "C0035FD6", "_ZN6HDmpve14IQRCodeServiceC1Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0xb8380", "C0035FD6", "_ZN6HDmpve11CPluginBase8UnistallEv");
    SAFE_PATCH_LIB("libhdmpve.so", "0x179978", "C0035FD6", "_ZNK6HDmpve13ZoneRouteInfo5CloneEv");
    SAFE_PATCH_LIB("libhdmpve.so", "0x17caec", "C0035FD6", "_ZN6HDmpve18RealNameAuthResultD0Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x12e828", "C0035FD6", "_ZN6HDmpve4Conn15ServerRouteInfoD0Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x1b3bdc", "C0035FD6", "_ZN15ChannelInfoUtil20getV2SignBlockOffs");
    SAFE_PATCH_LIB("libhdmpve.so", "0x13aee4", "C0035FD6", "Java_com_gcore_hdmpve_netinterface_MsgWo");
    SAFE_PATCH_LIB("libhdmpve.so", "0xba684", "C0035FD6", "hdmpve_account_logout");
    SAFE_PATCH_LIB("libhdmpve.so", "0xb7d64", "C0035FD6", "_ZNSt8_Rb_treeISsSt4pairIKSsPN6HDmpve7IP");
    SAFE_PATCH_LIB("libhdmpve.so", "0xc09a4", "C0035FD6", "_ZN6HDmpve6Access15ReleaseInstanceEv");
    SAFE_PATCH_LIB("libhdmpve.so", "0x10f8c0", "C0035FD6", "_ZN6HDmpve14IQRCodeServiceD2Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x179d5c", "C0035FD6", "_ZNK6HDmpve5Token5CloneEv");
    SAFE_PATCH_LIB("libhdmpve.so", "0x3c68ac", "C0035FD6", "hdmpve_tdir_queryleaf");
    SAFE_PATCH_LIB("libhdmpve.so", "0xbbec0", "C0035FD6", "_ZNSt6vectorIPN6HDmpve16IServiceObserver");
    SAFE_PATCH_LIB("libhdmpve.so", "0x17caa0", "C0035FD6", "_ZN6HDmpve18RealNameAuthResultD1Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x3c6eac", "C0035FD6", "_ZN6HDmpve5ITDirD2Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x1ecdac", "C0035FD6", "_ZN6HDmpve13DolphinHelper13GetCurApkPath");
    SAFE_PATCH_LIB("libhdmpve.so", "0x3d9590", "C0035FD6", "hdmpve_gromelink_updateCookie");
    SAFE_PATCH_LIB("libhdmpve.so", "0x17cd78", "C0035FD6", "_ZNK6HDmpve11AccountInfo5CloneEv");
    SAFE_PATCH_LIB("libhdmpve.so", "0xbbb70", "C0035FD6", "_ZN6HDmpve4Conn13RouteInfoBaseD0Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0xb83d8", "C0035FD6", "_ZN6HDmpve15CSdkFactoryBaseD1Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0xbbb5c", "C0035FD6", "_ZN6HDmpve4Conn13RouteInfoBaseD2Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x12fb10", "C0035FD6", "_ZN6HDmpve4Conn13NameRouteInfoD2Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x158b3c", "C0035FD6", "_ZNKSt6vectorISsSaISsEE12_M_check_lenEmP");
    SAFE_PATCH_LIB("libhdmpve.so", "0xb9a38", "C0035FD6", "_ZN6HDmpve6ResultD1Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x17d38c", "C0035FD6", "_ZN6HDmpve7IHDmpve11GetInstanceEv");
    SAFE_PATCH_LIB("libhdmpve.so", "0x17cbb8", "C0035FD6", "_ZNK6HDmpve12RealNameInfo5CloneEv");
    SAFE_PATCH_LIB("libhdmpve.so", "0x3c6854", "C0035FD6", "hdmpve_tdir_querytree");
    SAFE_PATCH_LIB("libhdmpve.so", "0xc1edc", "C0035FD6", "hdmpve_set_account_info");
    SAFE_PATCH_LIB("libhdmpve.so", "0x179aac", "C0035FD6", "_ZN6HDmpve15ServerRouteInfoD0Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x179a64", "C0035FD6", "_ZN6HDmpve11WaitingInfoD1Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0xb9418", "C0035FD6", "hdmpve_pay_Pay4Mounth");
    SAFE_PATCH_LIB("libhdmpve.so", "0x3c67e8", "C0035FD6", "hdmpve_tdir_update");
    SAFE_PATCH_LIB("libhdmpve.so", "0x13b038", "C0035FD6", "Java_com_gcore_hdmpve_netinterface_MsgWo");
    SAFE_PATCH_LIB("libhdmpve.so", "0x10facc", "C0035FD6", "_ZN6HDmpve14IQRCodeService11GetInstanceE");
    SAFE_PATCH_LIB("libhdmpve.so", "0x15123c", "C0035FD6", "_ZNSt3tr117_Function_handlerIFN6HDmpve6R");
    SAFE_PATCH_LIB("libhdmpve.so", "0x3c6eb0", "C0035FD6", "_ZN6HDmpve5ITDirD0Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0xb7c2c", "C0035FD6", "_ZNSt8_Rb_treeISsSt4pairIKSsPN6HDmpve7IP");
    SAFE_PATCH_LIB("libhdmpve.so", "0xb85cc", "C0035FD6", "_ZNSt6vectorIPN6HDmpve16IServiceObserver");
    SAFE_PATCH_LIB("libhdmpve.so", "0xb9d8c", "C0035FD6", "_ZN6HDmpve11AccountInfoD2Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x12e848", "C0035FD6", "_ZN6HDmpve4Conn13ZoneRouteInfoD1Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x3c67c0", "C0035FD6", "hdmpve_tdir_getinstance");
    SAFE_PATCH_LIB("libhdmpve.so", "0x179bb4", "C0035FD6", "_ZNK6HDmpve18RealNameAuthResult5CloneEv");
    SAFE_PATCH_LIB("libhdmpve.so", "0xbab28", "C0035FD6", "hdmpve_account_getRecord");
    SAFE_PATCH_LIB("libhdmpve.so", "0x10fb10", "C0035FD6", "_ZNSt6vectorIPN6HDmpve15IQRCodeObserverE");
    SAFE_PATCH_LIB("libhdmpve.so", "0x10e11c", "C0035FD6", "hdmpve_quit");
    SAFE_PATCH_LIB("libhdmpve.so", "0x17cb0c", "C0035FD6", "_ZN6HDmpve12RealNameInfoD1Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x12fb10", "C0035FD6", "_ZN6HDmpve4Conn13NameRouteInfoD1Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x3c7694", "C0035FD6", "_ZNSt6vectorIPN6HDmpve12TDirObserverESaI");
    SAFE_PATCH_LIB("libhdmpve.so", "0xb8344", "C0035FD6", "_ZN6HDmpve11CPluginBase7InstallEv");
    SAFE_PATCH_LIB("libhdmpve.so", "0xb95c8", "C0035FD6", "hdmpve_switchplugin");
    SAFE_PATCH_LIB("libhdmpve.so", "0x179a98", "C0035FD6", "_ZN6HDmpve15ServerRouteInfoD1Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0xb8004", "C0035FD6", "_ZNSt8_Rb_treeISsSt4pairIKSsPN6HDmpve7IP");
    SAFE_PATCH_LIB("libhdmpve.so", "0x3f521c", "C0035FD6", "apollo_net_sniffer_PingCluster");
    SAFE_PATCH_LIB("libhdmpve.so", "0x13b1a0", "C0035FD6", "Java_com_gcore_hdmpve_netinterface_MsgWo");
    SAFE_PATCH_LIB("libhdmpve.so", "0x179c68", "C0035FD6", "_ZNK6HDmpve8UserInfo5CloneEv");
    SAFE_PATCH_LIB("libhdmpve.so", "0x13ae94", "C0035FD6", "Java_com_gcore_hdmpve_netinterface_MsgWo");
    SAFE_PATCH_LIB("libhdmpve.so", "0xba558", "C0035FD6", "hdmpve_account_login");
    SAFE_PATCH_LIB("libhdmpve.so", "0xc1708", "C0035FD6", "_ZN6HDmpve15IAccountServiceD1Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0xb9d8c", "C0035FD6", "_ZN6HDmpve11AccountInfoD1Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0xc0a18", "C0035FD6", "_ZN6HDmpve6Access11GetInstanceEv");
    SAFE_PATCH_LIB("libhdmpve.so", "0xc1744", "C0035FD6", "_ZTv0_n24_N6HDmpve15IAccountServiceD0Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x3c7a74", "C0035FD6", "_ZN6HDmpve5ITDir11GetInstanceEv");
    SAFE_PATCH_LIB("libhdmpve.so", "0xbbf5c", "C0035FD6", "_ZNSt12_Vector_baseIPN6HDmpve16IServiceO");
    SAFE_PATCH_LIB("libhdmpve.so", "0x12e848", "C0035FD6", "_ZN6HDmpve4Conn13ZoneRouteInfoD2Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0xbbb5c", "C0035FD6", "_ZN6HDmpve4Conn13RouteInfoBaseD1Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x1511b0", "C0035FD6", "_ZNSt3tr114_Function_base13_Base_manager");
    SAFE_PATCH_LIB("libhdmpve.so", "0x1ed284", "C0035FD6", "_ZN6HDmpve13DolphinHelper18CopyResFileFr");
    SAFE_PATCH_LIB("libhdmpve.so", "0xc0d48", "C0035FD6", "_ZN6HDmpve8UserInfoD1Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0xb83d8", "C0035FD6", "_ZN6HDmpve15CSdkFactoryBaseD2Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0xb9aa0", "C0035FD6", "_ZN6HDmpve6ResultD0Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x10f8ac", "C0035FD6", "_ZN6HDmpve14IQRCodeServiceC2Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x17cb98", "C0035FD6", "_ZN6HDmpve12RealNameInfoD0Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x12383c", "C0035FD6", "_ZN6HDmpve13NameRouteInfoD1Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x1b411c", "C0035FD6", "_ZN15ChannelInfoUtil14isV2Signature2EPKc");
    SAFE_PATCH_LIB("libhdmpve.so", "0x10f9b4", "C0035FD6", "_ZN6HDmpve14IQRCodeServiceD0Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0x12e814", "C0035FD6", "_ZN6HDmpve4Conn15ServerRouteInfoD2Ev");
    SAFE_PATCH_LIB("libhdmpve.so", "0xba820", "C0035FD6", "hdmpve_account_getAppId");

    LOGI("[@OWNERHUBEE] HDmpve patches complete");
}

// ============================================================
// PHASE 4: libTDataMaster.so PATCHES + HOOKS
// ============================================================
void applyTDataMasterPatches() {
    if (!g_Settings.bTDataMasterPatches) {
        logStatus("[DISABLED] TDataMaster patches skipped by settings");
        return;
    }
    
    logStatus("\n=== PHASE 4: libTDataMaster.so PATCHES ===");
    
    SAFE_PATCH_LIB("libTDataMaster.so", "0x4D47C", "C0035FD6", "TDM_GetInstance_RET");
    SAFE_PATCH_LIB("libTDataMaster.so", "0xA74A0", "C0035FD6", "TDM_SIGPIPE_Handler_RET");
    SAFE_PATCH_LIB("libTDataMaster.so", "0x4E33C", "C0035FD6", "TDM_ReportSend_RET");
    SAFE_PATCH_LIB("libTDataMaster.so", "0x569C0", "C0035FD6", "TDM_ACConfigInit_RET");
    SAFE_PATCH_LIB("libTDataMaster.so", "0x653EC", "C0035FD6", "TDM_ACConfigCheck_RET");
    SAFE_PATCH_LIB("libTDataMaster.so", "0x92B04", "C0035FD6", "TDM_PluginReport_OnStartup_RET");
    SAFE_PATCH_LIB("libTDataMaster.so", "0x92534", "C0035FD6", "TDM_Initialize_RET");
    
    // TDM Hooks
    logStatus("\n--- Installing TDataMaster Hooks ---");
    HOOK_LIB("libTDataMaster.so", "0x4D47C", hTDM_GetInstance, oTDM_GetInstance);
    logStatus("[HOOK] TDM_GetInstance @ 0x4D47C");
    HOOK_LIB("libTDataMaster.so", "0x4E33C", hTDM_ReportSend, oTDM_ReportSend);
    logStatus("[HOOK] TDM_ReportSend @ 0x4E33C");
    HOOK_LIB("libTDataMaster.so", "0x569C0", hTDM_ACConfigInit, oTDM_ACConfigInit);
    logStatus("[HOOK] TDM_ACConfigInit @ 0x569C0");
    
    LOGI("[@OWNERHUBEE] TDataMaster patches complete");
}

// ============================================================
// PHASE 5: libsigner.so PATCHES
// ============================================================
void applySignerPatches() {
    if (!g_Settings.bSignerPatches) {
        logStatus("[DISABLED] Signer patches skipped by settings");
        return;
    }
    
    logStatus("\n=== PHASE 5: libsigner.so PATCHES ===");
    
    SAFE_PATCH_LIB("libsigner.so", "0x9afc0", "C0035FD6", "nSign_JNI_RET");
    
    LOGI("[@OWNERHUBEE] Signer patches complete");
}

// ============================================================
// PHASE 6: libUE4.so ANTI-CHEAT PATCHES + HOOKS
// ============================================================
void applyUE4ACPatches() {
    if (!g_Settings.bUE4ACPatches) {
        logStatus("[DISABLED] UE4 AC patches skipped by settings");
        return;
    }
    
    logStatus("\n=== PHASE 6: libUE4.so ANTI-CHEAT PATCHES ===");
    
    // Layer 1: NOP all BL callers of AC Dispatcher (0x5ACC184)
    SAFE_PATCH_LIB("libUE4.so", "0x5AB2F78", "1F2003D5", "NOP_BL_AC_Dispatch_01");
    SAFE_PATCH_LIB("libUE4.so", "0x5ACBFB4", "1F2003D5", "NOP_BL_AC_Dispatch_02");
    SAFE_PATCH_LIB("libUE4.so", "0x5C6AA88", "1F2003D5", "NOP_BL_AC_Dispatch_03");
    SAFE_PATCH_LIB("libUE4.so", "0x5D79504", "1F2003D5", "NOP_BL_AC_Dispatch_04");
    SAFE_PATCH_LIB("libUE4.so", "0x5D7A8A0", "1F2003D5", "NOP_BL_AC_Dispatch_05");
    SAFE_PATCH_LIB("libUE4.so", "0x6213140", "1F2003D5", "NOP_BL_AC_Dispatch_06");
    SAFE_PATCH_LIB("libUE4.so", "0x62131AC", "1F2003D5", "NOP_BL_AC_Dispatch_07");
    SAFE_PATCH_LIB("libUE4.so", "0x644B5DC", "1F2003D5", "NOP_BL_AC_Dispatch_08");
    SAFE_PATCH_LIB("libUE4.so", "0x67826F4", "1F2003D5", "NOP_BL_AC_Dispatch_09");
    SAFE_PATCH_LIB("libUE4.so", "0x68CD9C8", "1F2003D5", "NOP_BL_AC_Dispatch_10");
    SAFE_PATCH_LIB("libUE4.so", "0x6921EE8", "1F2003D5", "NOP_BL_AC_Dispatch_11");
    SAFE_PATCH_LIB("libUE4.so", "0x695DC08", "1F2003D5", "NOP_BL_AC_Dispatch_12");
    SAFE_PATCH_LIB("libUE4.so", "0x69A8570", "1F2003D5", "NOP_BL_AC_Dispatch_13");
    SAFE_PATCH_LIB("libUE4.so", "0x69A8604", "1F2003D5", "NOP_BL_AC_Dispatch_14");
    SAFE_PATCH_LIB("libUE4.so", "0x7BF8044", "1F2003D5", "NOP_BL_AC_Dispatch_15");
    SAFE_PATCH_LIB("libUE4.so", "0x7C3A808", "1F2003D5", "NOP_BL_AC_Dispatch_16");
    SAFE_PATCH_LIB("libUE4.so", "0x9FEFDE4", "1F2003D5", "NOP_BL_AC_Dispatch_17");
    SAFE_PATCH_LIB("libUE4.so", "0xAA73CD8", "1F2003D5", "NOP_BL_AC_Dispatch_18");
    SAFE_PATCH_LIB("libUE4.so", "0xBA6FC2C", "1F2003D5", "NOP_BL_AC_Dispatch_19");
    SAFE_PATCH_LIB("libUE4.so", "0xBA9D4AC", "1F2003D5", "NOP_BL_AC_Dispatch_20");
    SAFE_PATCH_LIB("libUE4.so", "0xC25D2D4", "1F2003D5", "NOP_BL_AC_Dispatch_21");
    
    // Layer 2: NOP caller of AC Enforcer
    SAFE_PATCH_LIB("libUE4.so", "0x7AD1B4C", "1F2003D5", "NOP_BL_AC_Enforce");
    
    // Layer 3: RET at AC Function Entries
    SAFE_PATCH_LIB("libUE4.so", "0x749F538", "C0035FD6", "AC_Callback1_RET");
    SAFE_PATCH_LIB("libUE4.so", "0x7AD8950", "C0035FD6", "AC_Callback2_RET");
    SAFE_PATCH_LIB("libUE4.so", "0xA4FBA10", "C0035FD6", "AC_Callback3_RET");
    SAFE_PATCH_LIB("libUE4.so", "0x5ACC184", "C0035FD6", "AC_Dispatcher_RET");
    SAFE_PATCH_LIB("libUE4.so", "0x62E286C", "C0035FD6", "AC_IntegrityCheck_RET");
    SAFE_PATCH_LIB("libUE4.so", "0x68CE7EC", "C0035FD6", "AC_MemMonitor1_RET");
    SAFE_PATCH_LIB("libUE4.so", "0x68CE7F4", "C0035FD6", "AC_MemMonitor2_RET");
    SAFE_PATCH_LIB("libUE4.so", "0x7A649A8", "C0035FD6", "AC_Enforcer_RET");
    
    // Layer 5: High-frequency AC dispatchers
    SAFE_PATCH_LIB("libUE4.so", "0x8394FEC", "C0035FD6", "AC_Reporter_58K_RET");
    SAFE_PATCH_LIB("libUE4.so", "0x8603830", "C0035FD6", "AC_Watcher1_30K_RET");
    SAFE_PATCH_LIB("libUE4.so", "0x86037F4", "C0035FD6", "AC_Watcher2_30K_RET");
    SAFE_PATCH_LIB("libUE4.so", "0x862A210", "C0035FD6", "AC_Dispatch2_24K_RET");
    
    // UE4 Emulator/Report patches from SDK
    SAFE_PATCH_LIB("libUE4.so", "0x7625FB0", "000080D2C0035FD6", "IsEmulator_RetFalse");
    SAFE_PATCH_LIB("libUE4.so", "0x7625FE8", "000080D2C0035FD6", "IsEmulatorWhenInit_RetFalse");
    SAFE_PATCH_LIB("libUE4.so", "0x761E918", "C0035FD6", "GetEmulatorName_RET");
    SAFE_PATCH_LIB("libUE4.so", "0x6FB028C", "C0035FD6", "ReportAntiCheatInfo_RET");
    SAFE_PATCH_LIB("libUE4.so", "0x6D95FEC", "C0035FD6", "ReportACDetailData_RET");
    SAFE_PATCH_LIB("libUE4.so", "0x6FA37C4", "000080D2C0035FD6", "CheckNeedReport_RetFalse");
    SAFE_PATCH_LIB("libUE4.so", "0x6FB78A0", "C0035FD6", "ServerHandleHitData_RET");
    SAFE_PATCH_LIB("libUE4.so", "0x6FAAA44", "C0035FD6", "HandleHitDataArray_RET");
    
    // Install UE4 AC Hooks
    logStatus("\n--- Installing UE4 AC Hooks ---");
    HOOK_LIB("libUE4.so", "0x7625FB0", hIsEmulator, oIsEmulator);
    HOOK_LIB("libUE4.so", "0x7625FE8", hIsEmulatorWhenInit, oIsEmulatorWhenInit);
    HOOK_LIB("libUE4.so", "0x761E918", hGetEmulatorName, oGetEmulatorName);
    HOOK_LIB("libUE4.so", "0x6FB028C", hReportAntiCheatInfo, oReportAntiCheatInfo);
    HOOK_LIB("libUE4.so", "0x6D95FEC", hReportAntiCheatDetailData, oReportAntiCheatDetailData);
    HOOK_LIB("libUE4.so", "0x6FA37C4", hCheckNeedReport, oCheckNeedReport);
    HOOK_LIB("libUE4.so", "0x6FB78A0", hServerHandleHitDataArray, oServerHandleHitDataArray);
    HOOK_LIB("libUE4.so", "0x6FAAA44", hHandleHitDataArray, oHandleHitDataArray);
    HOOK_LIB("libUE4.so", "0x749F538", hUE4_749F538, oUE4_749F538);
    HOOK_LIB("libUE4.so", "0x7AD8950", hUE4_7AD8950, oUE4_7AD8950);
    HOOK_LIB("libUE4.so", "0xA4FBA10", hUE4_A4FBA10, oUE4_A4FBA10);
    logStatus("[HOOK] UE4 AC hooks x11 installed");
    
    LOGI("[@OWNERHUBEE] UE4 AC patches complete");
}

// ============================================================
// PHASE 7: GAME FEATURE PATCHES (Settings-controlled)
// ============================================================
void applyGameFeaturePatches() {
    logStatus("\n=== PHASE 7: GAME FEATURE PATCHES ===");
    
    if (g_Settings.bFPSUnlock) {
        SAFE_PATCH_LIB("libUE4.so", "0xBC2C054", "1F2003D5", "FPS_Unlock_NOP_nanosleep");
        logStatus("[FEATURE] 120FPS Unlock ENABLED");
    }
    
    if (g_Settings.bNoRecoil) {
        SAFE_PATCH_LIB("libUE4.so", "0xBB8DA0", "00000000", "NoRecoil_AnimKick");
        SAFE_PATCH_LIB("libUE4.so", "0xBB8DB8", "00000000", "NoRecoil_KickADS");
        SAFE_PATCH_LIB("libUE4.so", "0xBB9190", "00000000", "NoRecoil_Deviation");
        SAFE_PATCH_LIB("libUE4.so", "0xBB9208", "00000000", "NoRecoil_HRecoil");
        SAFE_PATCH_LIB("libUE4.so", "0xBB9238", "00000000", "NoRecoil_VRecoil");
        logStatus("[FEATURE] No Recoil ENABLED");
    }
    
    if (g_Settings.bNoShake) {
        SAFE_PATCH_LIB("libUE4.so", "0xBB73C0", "00000000", "NoShake_InnerRadius");
        SAFE_PATCH_LIB("libUE4.so", "0xF0F098", "00000000", "NoShake_Scale");
        logStatus("[FEATURE] No Shake ENABLED");
    }
    
    if (g_Settings.bNoGrass) {
        SAFE_PATCH_LIB("libUE4.so", "0x1C6C820", "00000000", "NoGrass_Density");
        logStatus("[FEATURE] No Grass ENABLED");
    }
    
    if (g_Settings.bFlashSpeed) {
        SAFE_PATCH_LIB("libUE4.so", "0x18AF088", "00000000", "FlashSpeed_Zero");
        logStatus("[FEATURE] Flash Speed ENABLED");
    }
    
    if (g_Settings.bAimAssist) {
        SAFE_PATCH_LIB("libUE4.so", "0x11F6BA0", "00000000", "AimAssist_Zero");
        logStatus("[FEATURE] Aim Assist ENABLED");
    }
    
    if (g_Settings.bHDRUnlock) {
        SAFE_PATCH_LIB("libUE4.so", "0x068CFD84", "C0035FD6", "HDR_Unlock_RET");
        logStatus("[FEATURE] HDR Unlock ENABLED");
    }
    
    if (g_Settings.bIPadFOV) {
        SAFE_PATCH_LIB("libUE4.so", "0xA9EEC38", "C0035FD6", "iPad_FOV266_RET");
        logStatus("[FEATURE] iPad FOV ENABLED");
    }
    
    LOGI("[@OWNERHUBEE] Game feature patches complete");
}

// ============================================================
// MASTER INIT - Calls all bypass phases in order
// ============================================================
// Safe phase executor - catches crashes per-phase
#include <setjmp.h>

static sigjmp_buf g_phase_jmp;
static volatile int g_in_safe_phase = 0;
static const char* g_current_phase = "unknown";

void bypass_crash_handler(int sig, siginfo_t *info, void *ctx) {
    if (g_in_safe_phase) {
        LOGI("[@CRASH] Phase '%s' CRASHED! sig=%d addr=%p", g_current_phase, sig, info->si_addr);
        g_bypassFailed++;
        siglongjmp(g_phase_jmp, 1);
    }
    // Not in safe phase - re-raise
    signal(sig, SIG_DFL);
    raise(sig);
}

void install_phase_crash_handler() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = bypass_crash_handler;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGBUS, &sa, NULL);
}

// Run a bypass phase safely - if it crashes, skip it
#define RUN_SAFE_PHASE(phase_name, func) do { \
    g_current_phase = phase_name; \
    LOGI("[@OWNERHUBEE] >>> PHASE: %s", phase_name); \
    g_in_safe_phase = 1; \
    if (sigsetjmp(g_phase_jmp, 1) == 0) { \
        func(); \
        LOGI("[@OWNERHUBEE] <<< PHASE: %s OK", phase_name); \
    } else { \
        LOGI("[@OWNERHUBEE] <<< PHASE: %s CRASHED - SKIPPED", phase_name); \
    } \
    g_in_safe_phase = 0; \
} while(0)

void initAllBypasses() {
    LOGI("[@OWNERHUBEE] ====================================");
    LOGI("[@OWNERHUBEE] INITIALIZING ALL BYPASSES (SAFE MODE)");
    LOGI("[@OWNERHUBEE] ====================================");
    
    if (!g_Settings.bAntiCheatBypass) {
        LOGI("[@OWNERHUBEE] AC bypass disabled in settings");
        return;
    }
    
    // Install per-phase crash handler
    install_phase_crash_handler();
    
    // Each phase is protected - if one crashes, we skip it and continue
    RUN_SAFE_PHASE("AnoGS_Patches", applyAnogsPatches);
    RUN_SAFE_PHASE("HdmpveCore_Patches", applyHdmpvecorePatches);
    RUN_SAFE_PHASE("Hdmpve_Patches", applyHdmpvePatches);
    RUN_SAFE_PHASE("TDataMaster_Patches", applyTDataMasterPatches);
    RUN_SAFE_PHASE("Signer_Patches", applySignerPatches);
    RUN_SAFE_PHASE("UE4_AC_Patches", applyUE4ACPatches);
    RUN_SAFE_PHASE("Game_Features", applyGameFeaturePatches);
    
    LOGI("[@OWNERHUBEE] ====================================");
    LOGI("[@OWNERHUBEE] BYPASS DONE: %d OK / %d FAIL / %d SKIP",
        g_bypassSuccess, g_bypassFailed, g_bypassSkipped);
    LOGI("[@OWNERHUBEE] ====================================");
}
