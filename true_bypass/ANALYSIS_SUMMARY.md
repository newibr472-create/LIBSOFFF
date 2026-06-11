# TRUE BYPASS - Deep Analysis Results
# BGMI 4.4.0 ARM64 Anti-Cheat Libraries

## libanogs.so (5.5MB) - Tencent AnoSDK

### All 21 Exports:
- `AnoSDKInit` @ 0x1d3814 [ANTI_CHEAT]
- `AnoSDKGetReportData3` @ 0x1d7938 [ANTI_CHEAT]
- `AnoSDKGetReportData` @ 0x1d551c [ANTI_CHEAT]
- `AnoSDKOnRecvData` @ 0x1d624c [ANTI_CHEAT]
- `AnoSDKIoctlOld` @ 0x1d6598 [ANTI_CHEAT]
- `AnoSDKIoctl` @ 0x1d6ea8 [ANTI_CHEAT]
- `AnoSDKDelReportData3` @ 0x1d79a4 [ANTI_CHEAT]
- `AnoSDKGetReportData4` @ 0x1d7fc4 [ANTI_CHEAT]
- `AnoSDKOnRecvSignature` @ 0x1d88ec [ANTI_CHEAT]
- `AnoSDKInitEx` @ 0x1d3b40 [ANTI_CHEAT]
- `AnoSDKSetUserInfo` @ 0x1d417c [ANTI_CHEAT]
- `AnoSDKDelReportData` @ 0x1d5a88 [ANTI_CHEAT]
- `AnoSDKDelReportData4` @ 0x1d82cc [ANTI_CHEAT]
- `AnoSDKSetUserInfoWithLicense` @ 0x1d4580 [ANTI_CHEAT]
- `AnoSDKFree` @ 0x1d7398 [ANTI_CHEAT]
- `AnoSDKForExport` @ 0x1d9024 [ANTI_CHEAT]
- `JNI_OnLoad` @ 0x1db894 [JNI]
- `AnoSDKOnPause` @ 0x1d4c0c [ANTI_CHEAT]
- `AnoSDKOnResume` @ 0x1d5030 [ANTI_CHEAT]
- `AnoSDKGetReportData2` @ 0x1d78cc [ANTI_CHEAT]
- `AnoSDKRegistInfoListener` @ 0x1d8c74 [ANTI_CHEAT]

### Hook Targets (AnoSDK functions):
- **AnoSDKInit** @ `0x1d3814`
  First instructions: stp x29, x30, [sp, #-0x50]!; stp x26, x25, [sp, #0x10]; stp x24, x23, [sp, #0x20]
- **AnoSDKGetReportData3** @ `0x1d7938`
  First instructions: sub sp, sp, #0x10; mov w8, #0x51bf; mov w9, #0x19e2
- **AnoSDKGetReportData** @ `0x1d551c`
  First instructions: stp x29, x30, [sp, #-0x60]!; stp x28, x27, [sp, #0x10]; stp x26, x25, [sp, #0x20]
- **AnoSDKOnRecvData** @ `0x1d624c`
  First instructions: stp x29, x30, [sp, #-0x60]!; str x27, [sp, #0x10]; stp x26, x25, [sp, #0x20]
- **AnoSDKIoctlOld** @ `0x1d6598`
  First instructions: stp x29, x30, [sp, #-0x60]!; stp x28, x27, [sp, #0x10]; stp x26, x25, [sp, #0x20]
- **AnoSDKIoctl** @ `0x1d6ea8`
  First instructions: stp x29, x30, [sp, #-0x10]!; mov x29, sp; sub sp, sp, #0x40
- **AnoSDKDelReportData3** @ `0x1d79a4`
  First instructions: stp x29, x30, [sp, #-0x10]!; mov x29, sp; sub sp, sp, #0x60
- **AnoSDKGetReportData4** @ `0x1d7fc4`
  First instructions: stp x29, x30, [sp, #-0x50]!; stp x26, x25, [sp, #0x10]; stp x24, x23, [sp, #0x20]
- **AnoSDKOnRecvSignature** @ `0x1d88ec`
  First instructions: stp x29, x30, [sp, #-0x60]!; stp x28, x27, [sp, #0x10]; stp x26, x25, [sp, #0x20]
- **AnoSDKInitEx** @ `0x1d3b40`
  First instructions: stp x29, x30, [sp, #-0x20]!; stp x20, x19, [sp, #0x10]; mov x29, sp
- **AnoSDKSetUserInfo** @ `0x1d417c`
  First instructions: stp x29, x30, [sp, #-0x60]!; stp x28, x27, [sp, #0x10]; stp x26, x25, [sp, #0x20]
- **AnoSDKDelReportData** @ `0x1d5a88`
  First instructions: stp x29, x30, [sp, #-0x60]!; stp x28, x27, [sp, #0x10]; stp x26, x25, [sp, #0x20]
- **AnoSDKDelReportData4** @ `0x1d82cc`
  First instructions: stp x29, x30, [sp, #-0x10]!; mov x29, sp; sub sp, sp, #0x60
- **AnoSDKSetUserInfoWithLicense** @ `0x1d4580`
  First instructions: stp x29, x30, [sp, #-0x60]!; stp x28, x27, [sp, #0x10]; stp x26, x25, [sp, #0x20]
- **AnoSDKFree** @ `0x1d7398`
  First instructions: stp x29, x30, [sp, #-0x10]!; mov x29, sp; sub sp, sp, #0x60
- **AnoSDKForExport** @ `0x1d9024`
  First instructions: sub sp, sp, #0x10; mov w8, #0xf4dc; mov w9, #0x7506
- **AnoSDKOnPause** @ `0x1d4c0c`
  First instructions: stp x29, x30, [sp, #-0x60]!; stp x28, x27, [sp, #0x10]; stp x26, x25, [sp, #0x20]
- **AnoSDKOnResume** @ `0x1d5030`
  First instructions: stp x29, x30, [sp, #-0x60]!; stp x28, x27, [sp, #0x10]; stp x26, x25, [sp, #0x20]
- **AnoSDKGetReportData2** @ `0x1d78cc`
  First instructions: sub sp, sp, #0x10; mov w8, #0x51bf; mov w9, #0x19e2
- **AnoSDKRegistInfoListener** @ `0x1d8c74`
  First instructions: stp x29, x30, [sp, #-0x60]!; str x27, [sp, #0x10]; stp x26, x25, [sp, #0x20]

### AC Strings with Cross-References:
- "ms_scan_start" → code refs: 0x1c5f48, 0x1c63e8, 0x1c6448, 0x1c6814, 0x1c687c
- "/data/data/%s/files" → code refs: 0x1c5f48, 0x1c63e8, 0x1c6448, 0x1c6814, 0x1c687c
- "output buffer too small for in-memory compression" → code refs: 0x1c5f48, 0x1c63e8, 0x1c6448, 0x1c6814, 0x1c687c
- "unsigned short" → code refs: 0x1c5f48, 0x1c63e8, 0x1c6448, 0x1c6814, 0x1c687c
- "unsigned long long" → code refs: 0x1c5f48, 0x1c63e8, 0x1c6448, 0x1c6814, 0x1c687c
- "%s;crc:%s" → code refs: 0x1c5f48, 0x1c63e8, 0x1c6448, 0x1c6814, 0x1c687c
- "/data/user/10/%s/files" → code refs: 0x1c5f48, 0x1c63e8, 0x1c6448, 0x1c6814, 0x1c687c
- "DW_EH_PE_funcrel pointer encoding not supported" → code refs: 0x1c5f48, 0x1c63e8, 0x1c6448, 0x1c6814, 0x1c687c
- "nsUyhU}uwja" → code refs: 0x1c5f48, 0x1c63e8, 0x1c6448, 0x1c6814, 0x1c687c
- "TotalMem" → code refs: 0x1c5f48, 0x1c63e8, 0x1c6448, 0x1c6814, 0x1c687c
- "tdm_report" → code refs: 0x1c5f48, 0x1c63e8, 0x1c6448, 0x1c6814, 0x1c687c
- "debugger=%s" → code refs: 0x1c5f48, 0x1c63e8, 0x1c6448, 0x1c6814, 0x1c687c
- "oversubscribed distance tree" → code refs: 0x1c5f48, 0x1c63e8, 0x1c6448, 0x1c6814, 0x1c687c
- "Smoothing not supported with nonstandard sampling ratios" → code refs: 0x1c5f48, 0x1c63e8, 0x1c6448, 0x1c6814, 0x1c687c
- "REPORT" → code refs: 0x1c5f48, 0x1c63e8, 0x1c6448, 0x1c6814, 0x1c687c
- "result" → code refs: 0x1c5f48, 0x1c63e8, 0x1c6448, 0x1c6814, 0x1c687c
- "inline_hook_opcode_dismatch" → code refs: 0x1c60b8, 0x1c612c, 0x1c6140, 0x1c6434, 0x1c67b0
- "mmap_fialed" → code refs: 0x1c60b8, 0x1c612c, 0x1c6140, 0x1c6434, 0x1c67b0
- "Suspension not allowed here" → code refs: 0x1c60b8, 0x1c612c, 0x1c6140, 0x1c6434, 0x1c67b0
- "ReportComplaint" → code refs: 0x1c60b8, 0x1c612c, 0x1c6140, 0x1c6434, 0x1c67b0
- "onResume" → code refs: 0x1c60b8, 0x1c612c, 0x1c6140, 0x1c6434, 0x1c67b0
- "Insufficient memory (case %d)" → code refs: 0x1c60b8, 0x1c612c, 0x1c6140, 0x1c6434, 0x1c67b0
- "_Unwind_Resume() can't return" → code refs: 0x1c60b8, 0x1c612c, 0x1c6140, 0x1c6434, 0x1c67b0
- "set_inline_hook_error" → code refs: 0x1c60b8, 0x1c612c, 0x1c6140, 0x1c6434, 0x1c67b0
- "init_info->tss_sdk_send_data_to_svr:%p" → code refs: 0x1c60b8, 0x1c612c, 0x1c6140, 0x1c6434, 0x1c67b0
- "debugger:%s" → code refs: 0x1c60b8, 0x1c612c, 0x1c6140, 0x1c6434, 0x1c67b0
- "Error when TiXmlDocument added to document, because TiXmlDoc" → code refs: 0x1c60b8, 0x1c612c, 0x1c6140, 0x1c6434, 0x1c67b0
- "Unsupported JPEG data precision %d" → code refs: 0x1c60b8, 0x1c612c, 0x1c6140, 0x1c6434, 0x1c67b0
- "hello getreportdata3" → code refs: 0x1c60b8, 0x1c612c, 0x1c6140, 0x1c6434, 0x1c67b0
- "unsigned " → code refs: 0x1c60b8, 0x1c612c, 0x1c6140, 0x1c6434, 0x1c67b0
- "mt2_no_root" → code refs: 0x1c60b8, 0x1c612c, 0x1c6140, 0x1c6434, 0x1c67b0
- "allocator<T>::allocate(size_t n) 'n' exceeds maximum support" → code refs: 0x1c67f4, 0x1c7728, 0x1d0ac4, 0x1dbe84, 0x1ed390
- "Unsupported color conversion request" → code refs: 0x1c67f4, 0x1c7728, 0x1d0ac4, 0x1dbe84, 0x1ed390
- "void __cxxabiv1::scan_eh_tab(scan_results &, _Unwind_Action," → code refs: 0x1c67f4, 0x1c7728, 0x1d0ac4, 0x1dbe84, 0x1ed390
- "libunwind: unsupported .eh_frame_hdr version: %u at %lx" → code refs: 0x1c67f4, 0x1c7728, 0x1d0ac4, 0x1dbe84, 0x1ed390
- "unsupported arm64 register" → code refs: 0x1c67f4, 0x1c7728, 0x1d0ac4, 0x1dbe84, 0x1ed390
- "unsupported restore location for register" → code refs: 0x1c67f4, 0x1c7728, 0x1d0ac4, 0x1dbe84, 0x1ed390
- "ScanCast" → code refs: 0x1c67f4, 0x1c7728, 0x1d0ac4, 0x1dbe84, 0x1ed390
- "debugger" → code refs: 0x1c67f4, 0x1c7728, 0x1d0ac4, 0x1dbe84, 0x1ed390
- "Scan script does not transmit all data" → code refs: 0x1c67f4, 0x1c7728, 0x1d0ac4, 0x1dbe84, 0x1ed390
- "check_state=%s" → code refs: 0x1c67f4, 0x1c7728, 0x1d0ac4, 0x1dbe84, 0x1ed390
- "wild scan" → code refs: 0x1c67f4, 0x1c7728, 0x1d0ac4, 0x1dbe84, 0x1ed390
- "DCT scaled block size %dx%d not supported" → code refs: 0x1c5f34, 0x1c645c, 0x1c6848, 0x1c69e0, 0x1c6a14
- "unsigned __int128" → code refs: 0x1c5f34, 0x1c645c, 0x1c6848, 0x1c69e0, 0x1c6a14
- "unsupported restore location for float register" → code refs: 0x1c5f34, 0x1c645c, 0x1c6848, 0x1c69e0, 0x1c6a14
- "DW_EH_PE_aligned pointer encoding not supported" → code refs: 0x1c5f34, 0x1c645c, 0x1c6848, 0x1c69e0, 0x1c6a14
- "mrpcs_data_crc_error" → code refs: 0x1c5f34, 0x1c645c, 0x1c6848, 0x1c69e0, 0x1c6a14
- "uint64_t AVM::BopMemoryOperation::PmemRead(paddr_t, int)" → code refs: 0x1c5f34, 0x1c645c, 0x1c6848, 0x1c69e0, 0x1c6a14
- "void AVM::BopMemoryOperation::PmemWrite(paddr_t, word_t, int" → code refs: 0x1c5f34, 0x1c645c, 0x1c6848, 0x1c69e0, 0x1c6a14
- "check_state:%s" → code refs: 0x1c5f34, 0x1c645c, 0x1c6848, 0x1c69e0, 0x1c6a14
- "insufficient lookahead" → code refs: 0x1c5f34, 0x1c645c, 0x1c6848, 0x1c69e0, 0x1c6a14
- "COREREPORT" → code refs: 0x1c5f34, 0x1c645c, 0x1c6848, 0x1c69e0, 0x1c6a14
- "unsigned char" → code refs: 0x1c5f34, 0x1c645c, 0x1c6848, 0x1c69e0, 0x1c6a14
- "Empty JPEG image (DNL not supported)" → code refs: 0x1c5f14, 0x1c64f0, 0x1c66fc, 0x1c6ac8, 0x1c6adc
- "Unsupported JPEG process: SOF type 0x%02x" → code refs: 0x1c5f14, 0x1c64f0, 0x1c66fc, 0x1c6ac8, 0x1c6adc
- "ChkSetGameStatus{" → code refs: 0x1c5f14, 0x1c64f0, 0x1c66fc, 0x1c6ac8, 0x1c6adc
- "ms_mmap" → code refs: 0x1c5f14, 0x1c64f0, 0x1c66fc, 0x1c6ac8, 0x1c6adc
- "Application transferred too many scanlines" → code refs: 0x1c5f14, 0x1c64f0, 0x1c66fc, 0x1c6ac8, 0x1c6adc
- "hash_cache" → code refs: 0x1c5f14, 0x1c64f0, 0x1c66fc, 0x1c6ac8, 0x1c6adc
- "/data/data" → code refs: 0x1c5f14, 0x1c64f0, 0x1c66fc, 0x1c6ac8, 0x1c6adc
- "/Users/bkdevops/tpmobile/workspace/p-74fc7d317ebc4e7c912d0c9" → code refs: 0x1c5f14, 0x1c64f0, 0x1c66fc, 0x1c6ac8, 0x1c6adc
- "Invalid memory pool code %d" → code refs: 0x1c5f14, 0x1c64f0, 0x1c66fc, 0x1c6ac8, 0x1c6adc
- "status_info->game_status_:%d" → code refs: 0x1c5f14, 0x1c64f0, 0x1c66fc, 0x1c6ac8, 0x1c6adc
- "libunwind: unsupported .eh_frame_hdr at %lx: need at least 4" → code refs: 0x1c5f14, 0x1c64f0, 0x1c66fc, 0x1c6ac8, 0x1c6adc
- "ms_data_crc" → code refs: 0x1c63c0, 0x1c6418, 0x1c6504, 0x1c654c, 0x1c658c
- "Unrecognized component IDs %d %d %d, assuming YCbCr" → code refs: 0x1c63c0, 0x1c6418, 0x1c6504, 0x1c654c, 0x1c658c
- "TotalMem:%d;FreeSpaceTDM:%d" → code refs: 0x1c63c0, 0x1c6418, 0x1c6504, 0x1c654c, 0x1c658c
- "DW_EH_PE_textrel pointer encoding not supported" → code refs: 0x1c63c0, 0x1c6418, 0x1c6504, 0x1c654c, 0x1c658c
- "oversubscribed literal/length tree" → code refs: 0x1c63c0, 0x1c6418, 0x1c6504, 0x1c654c, 0x1c658c
- "ms_set_inlie_hook" → code refs: 0x1c63c0, 0x1c6418, 0x1c6504, 0x1c654c, 0x1c658c
- "unsigned int" → code refs: 0x1c63c0, 0x1c6418, 0x1c6504, 0x1c654c, 0x1c658c
- "Maximum supported image dimension is %u pixels" → code refs: 0x1c63c0, 0x1c6418, 0x1c6504, 0x1c654c, 0x1c658c
- "check_state" → code refs: 0x1c63d4, 0x1c68b0, 0x1c8fdc, 0x1d11d8, 0x1daf34
- "incorrect header check" → code refs: 0x1c63d4, 0x1c68b0, 0x1c8fdc, 0x1d11d8, 0x1daf34
- "mrpcs_scan_thread_start_failed!" → code refs: 0x1c63d4, 0x1c68b0, 0x1c8fdc, 0x1d11d8, 0x1daf34
- "Invalid scan script at entry %d" → code refs: 0x1c63d4, 0x1c68b0, 0x1c8fdc, 0x1d11d8, 0x1daf34
- "Application transferred too few scanlines" → code refs: 0x1c63d4, 0x1c68b0, 0x1c8fdc, 0x1d11d8, 0x1daf34
- "Start Of Scan: %d components" → code refs: 0x1c63d4, 0x1c68b0, 0x1c8fdc, 0x1d11d8, 0x1daf34
- "signed char" → code refs: 0x1c63d4, 0x1c68b0, 0x1c8fdc, 0x1d11d8, 0x1daf34
- "|emulator_name=" → code refs: 0x1c63d4, 0x1c68b0, 0x1c8fdc, 0x1d11d8, 0x1daf34


## libhdmpvecore.so (1.4MB) - HDMP Core

### JNI Functions (38 total):
- `Java_com_gcore_abase_URLRequest_nativeTaskProgress` @ 0x71e4c
- `Java_com_gcore_abase_config_ConfigBundle_setLong` @ 0x61fc8
- `_ZN5ABase7JniTool22ConvertJStringToStringEP7_JNIEnvP8_jstringRSs` @ 0x5de24
- `Java_com_gcore_hdmpve_plugin_PluginUtils_nativePostStartup` @ 0x9b00c
- `Java_com_gcore_abase_config_ConfigBundle_setBoolean` @ 0x620a4
- `Java_com_gcore_abase_cos_CUploadTask_nativeOnUploadFinished` @ 0x709c8
- `_ZN8ABaseJVM11Jstring2StrEP7_JNIEnvP8_jstring` @ 0x7fb20
- `Java_com_gcore_abase_URLRequest_nativeResponse` @ 0x71fc0
- `Java_com_gcore_hdmpve_plugin_PluginUtils_nativePreShutdown` @ 0x9b054
- `Java_com_gcore_abase_config_ConfigBundle_setString` @ 0x61de4
- `_ZN8ABaseJVM12StrToJstringEP7_JNIEnvPKc` @ 0x7f9f0
- `Java_com_gcore_abase_log_XLog_nativeXLog` @ 0x66220
- `Java_com_gcore_abase_TenX_nativeResume` @ 0x7ed60
- `Java_com_gcore_abase_TenX_nativeNSLookupFinishNotify` @ 0x768a4
- `Java_com_gcore_abase_URLRequest_nativeTaskBegan` @ 0x71df8
- `_ZN8ABaseJVM14Jbytearray2StrEP7_JNIEnvP11_jbyteArray` @ 0x70a4c
- `Java_com_gcore_abase_URLRequest_nativeTaskFinished` @ 0x71ea4
- `Java_com_gcore_abase_TenX_nativePause` @ 0x7ed4c
- `JNI_OnLoad` @ 0x7f528
- `Java_com_gcore_abase_URLRequest_nativeSaveUploadPart` @ 0x71f1c
- `Java_com_gcore_hdmpve_plugin_PluginUtils_nativeShutdown` @ 0x9b09c
- `Java_com_gcore_abase_TenX_nativePerform` @ 0x7f2c4
- `Java_com_gcore_abase_config_ConfigBundle_getInt` @ 0x61b4c
- `_Z12COSUploadJNIPKcS0_xS0_PvS1_S0_` @ 0x70730
- `Java_com_gcore_abase_URLRequest_nativeInit` @ 0x71d9c
- `Java_com_gcore_abase_config_ConfigBundle_getBoolean` @ 0x61d04
- `Java_com_gcore_abase_config_ConfigBundle_getString` @ 0x62184
- `Java_com_gcore_abase_config_ConfigBundle_setInt` @ 0x61eec
- `_Z14ABase_setPathsP7_JNIEnvP8_jobject` @ 0x5deb4
- `Java_com_gcore_abase_TenX_setSPID` @ 0x7ed74
- `Java_com_gcore_abase_URLRequest_nativeRemoveCacheData` @ 0x71f70
- `Java_com_gcore_abase_config_ConfigBundle_getLong` @ 0x61c28
- `Java_com_gcore_hdmpve_plugin_PluginUtils_nativeStartup` @ 0x9afc4
- `Java_com_gcore_abase_TenX_nativeCreate` @ 0x7ef40
- `_ZN5ABase7JniTool22ConvertStringToJStringEP7_JNIEnvPKc` @ 0x760bc
- `Java_com_gcore_abase_TenX_nativePingFinishNotify` @ 0x76988
- `Java_com_gcore_abase_TenX_nativeTraceRouteFinishNotify` @ 0x767e0
- `Java_com_gcore_abase_TenX_nativeNetworkStateChangeNotify` @ 0x6c574

### Top 30 Hookable Targets:
- `_ZN6HDmpve16RemoteGameConfigD2Ev` @ 0x779d0
- `_ZN6HDmpve9Configure11GetInstanceEv` @ 0x7cc84
- `_ZN6HDmpve6Plugin19RemoteConfigSerivceC2Ev` @ 0x99cec
- `_ZN6HDmpve16RemoteGameConfig11AddObserverEPNS_20RemoteConfigObserverE` @ 0x76bf4
- `_ZN6HDmpve16RemoteGameConfigC2EPKc` @ 0x77228
- `_ZN6HDmpve12RemoteConfig9GetStringEPKcPcRiS2_` @ 0x7a8b0
- `_ZThn16_N5ABase15ANetworkChecker4PingEPKciPFvRNS_10PingResultEEhh` @ 0x763dc
- `_ZN5ABase14UploadTaskImpl6UploadEiiiPKc` @ 0x6fc88
- `_ZN5ABase7CSystem15CheckPermissionEi` @ 0x72d3c
- `_ZN6HDmpve22HDmpveCoreRemoteConfig9GetStringEPKcS2_` @ 0x7d134
- `_ZN6HDmpve13ConfigureImpl5StartEv` @ 0x7ab4c
- `_ZN5ABase10UrlRequest10InitializeEPKciPNS0_8ListenerE` @ 0x70ff8
- `_ZN6HDmpve12RemoteConfigD0Ev` @ 0x7aa40
- `_ZN6HDmpve13ConfigureImpl13_RetryRequestEv` @ 0x7b080
- `_ZN5ABase14UploadTaskImplD2Ev` @ 0x6e514
- `_ZN5ABase15ANetworkChecker15ReleaseInstanceEv` @ 0x74d30
- `abase_remoteconfig_GetString` @ 0x7a7f4
- `_ZN6HDmpve12RemoteConfigD1Ev` @ 0x7aa00
- `abase_remoteconfig_GetDouble` @ 0x7a880
- `_ZThn16_N5ABase15ANetworkChecker10TraceRouteEPKciPFvRNS_16TraceRouteResultEEh` @ 0x765e4
- `_ZN6HDmpve16RemoteGameConfig6GetIntEPKci` @ 0x77548
- `_ZNKSt6vectorIP7AObjectSaIS1_EE12_M_check_lenEmPKc` @ 0x8015c
- `_ZN5ABase8CNetwork4initEv` @ 0x6c594
- `_ZN5ABase3WWW14UploadFromDataEPKcPKvi` @ 0x6d660
- `_ZNSt8_Rb_treeISsSt4pairIKSsPN6HDmpve17ConfigureObserverEESt10_Select1stIS5_ESt4lessISsESaIS5_EE8_M_eraseEPSt13_Rb_tree_nodeIS5_E` @ 0x7b6e4
- `_ZN5ABase17SolidConfigReader10GetAllKeysEPKcP6AArray` @ 0x60860
- `_ZN5ABase14UploadTaskImpl6FinishEv` @ 0x6d900
- `_ZN6HDmpve15ConfigureReport22ReportPullConfigResultEiii` @ 0x79944
- `_ZN6HDmpve12FileUploaderC2Ev` @ 0x78628
- `_ZNK6HDmpve6Plugin19RemoteConfigSerivce13GetPluginNameEv` @ 0x99bfc


## libTBlueData.so (3.9MB) - Telemetry

### JNI Functions (24 total):
- `Java_com_blue_dm_TBlueData_TDMEnableReport` @ 0x8e854
- `Java_com_blue_dm_TBlueData_TDMReportBinary` @ 0x8f5c4
- `Java_com_blue_dm_system_FileUtils_FileUtilsInit` @ 0x8ff3c
- `Java_com_blue_dm_device_DeviceInfoHolder_SetNewLongDeviceInfo` @ 0x8fbd0
- `Java_com_blue_dm_system_TDMUtils_EncryptField` @ 0x8e308
- `Java_com_blue_dm_TBlueData_TDMPause` @ 0x8e81c
- `Java_com_blue_dm_TBlueData_TDMGetUID` @ 0x8e8bc
- `Java_com_blue_dm_TBlueData_TDMResume` @ 0x8e7e4
- `Java_com_blue_dm_TBlueData_TDMGetSDKVerision` @ 0x8ea38
- `Java_com_blue_dm_system_TDMUtils_SetDeviceInfoEncryptKey` @ 0x8e578
- `Java_com_blue_dm_database_TDMDataBase_DataBaseInit` @ 0x8e020
- `Java_com_blue_dm_system_FileUtils_GetDefaultPreferenceByKey` @ 0x8ffa0
- `Java_com_blue_dm_device_DeviceInfoHolder_SetNewBoolDeviceInfo` @ 0x8fd6c
- `Java_com_blue_dm_TBlueData_TDMReportEvent` @ 0x8ebf8
- `Java_com_blue_dm_TBlueData_setAppDir` @ 0x71e48
- `Java_com_blue_dm_device_DeviceInfoHolder_isEnableDeviceInfo` @ 0x8ff20
- `Java_com_blue_dm_device_DeviceInfoHolder_SetNewStringDeviceInfo` @ 0x8f97c
- `Java_com_blue_dm_system_TDMUtils_OnNetworkChanged` @ 0x8e0e8
- `Java_com_blue_dm_TBlueData_TDMInit` @ 0x8e7ac
- `JNI_OnLoad` @ 0x6e864
- `Java_com_blue_dm_TBlueData_TDMSetLogLevel` @ 0x8ebb4
- `Java_com_blue_dm_TBlueData_TDMReportLogin` @ 0x8f7ac
- `Java_com_blue_dm_system_TDMUtils_Init` @ 0x8e084
- `Java_com_blue_dm_TBlueData_TDMEnableDeviceInfo` @ 0x8f914

### Init Functions (auto-run on load):
- 0x3b2010 → target 0x4c78c
- 0x3b2018 → target 0x4c848
- 0x3b2020 → target 0x4c8c8
- 0x3b2028 → target 0x4c954
- 0x3b2030 → target 0x4c9e0
- 0x3b2038 → target 0x4caac
- 0x3b2040 → target 0x4cb38
- 0x3b2048 → target 0x4cbc4
- 0x3b2050 → target 0x4cbe8
- 0x3b2058 → target 0x4cc74

### Top Hookable:
- `_ZN6HDmpve6Plugin12PluginReportD0Ev` @ 0x92328
- `_ZThn8_NK6HDmpve6Plugin12PluginReport7GetNameEv` @ 0x92380
- `_ZThn8_N6HDmpve6Plugin12PluginReport13OnPreShutdownEv` @ 0x923c0
- `_ZThn8_N6HDmpve6Plugin12PluginReport16GetServiceByNameEPKc` @ 0x922e4
- `_ZN6HDmpve6Plugin12PluginReport10OnShutdownEv` @ 0x92370
- `_ZN6HDmpve6Plugin12PluginReport9OnStartupEPNS0_16IServiceRegisterE` @ 0x92344
- `_ZThn8_N6HDmpve6Plugin12PluginReport9OnStartupEPNS0_16IServiceRegisterE` @ 0x92398
- `TDM_Report_Binary` @ 0x91ff0
- `_ZThn8_N6HDmpve6Plugin12PluginReport10OnShutdownEv` @ 0x923c4
- `_ZThn8_NK6HDmpve6Plugin12PluginReport10GetVersionEv` @ 0x9238c
- `_ZNK6HDmpve6Plugin12PluginReport7GetNameEv` @ 0x9232c
- `_ZN6HDmpve6Plugin12PluginReport13OnPreShutdownEv` @ 0x9236c
- `TDM_Initialize` @ 0x91d74
- `_ZN6HDmpve6Plugin12PluginReport13OnPostStartupEv` @ 0x92368
- `_ZThn8_N6HDmpve6Plugin12PluginReport13OnPostStartupEv` @ 0x923bc
- `_ZThn8_N6HDmpve6Plugin12PluginReportD0Ev` @ 0x92378
- `TDM_Enable_Report` @ 0x91e28
- `_ZN6HDmpve6Plugin12PluginReportD2Ev` @ 0x92324
- `_ZN6HDmpve6Plugin12PluginReport16GetServiceByNameEPKc` @ 0x922a4
- `_ZThn8_N6HDmpve6Plugin12PluginReportD1Ev` @ 0x92374
- `_ZNK6HDmpve6Plugin12PluginReport10GetVersionEv` @ 0x92338