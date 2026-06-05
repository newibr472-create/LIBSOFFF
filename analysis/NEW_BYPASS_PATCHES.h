// AUTO-GENERATED: BGMI 4.4.0 Deep Analysis Bypass Patches
// Generated from full disassembly of all AC libraries
// Format: PATCH_LIB(lib, offset, hex)

#pragma once

void applyDeepAnalysisBypass() {

    // === libanogs.so (10 targets) ===
    PATCH_LIB("libanogs.so", 0x1d3814, "C0035FD6");  // AnoSDKInit@@ANO
    PATCH_LIB("libanogs.so", 0x1d7938, "C0035FD6");  // AnoSDKGetReportData3@@ANO
    PATCH_LIB("libanogs.so", 0x1d551c, "C0035FD6");  // AnoSDKGetReportData@@ANO
    PATCH_LIB("libanogs.so", 0x1d79a4, "C0035FD6");  // AnoSDKDelReportData3@@ANO
    PATCH_LIB("libanogs.so", 0x1d7fc4, "C0035FD6");  // AnoSDKGetReportData4@@ANO
    PATCH_LIB("libanogs.so", 0x1d88ec, "C0035FD6");  // AnoSDKOnRecvSignature@@ANO
    PATCH_LIB("libanogs.so", 0x1d3b40, "C0035FD6");  // AnoSDKInitEx@@ANO
    PATCH_LIB("libanogs.so", 0x1d5a88, "C0035FD6");  // AnoSDKDelReportData@@ANO
    PATCH_LIB("libanogs.so", 0x1d82cc, "C0035FD6");  // AnoSDKDelReportData4@@ANO
    PATCH_LIB("libanogs.so", 0x1d78cc, "C0035FD6");  // AnoSDKGetReportData2@@ANO

    // === libhdmpvecore.so (1918 targets) ===
    PATCH_LIB("libhdmpvecore.so", 0x779d0, "C0035FD6");  // _ZN6HDmpve16RemoteGameConfigD2Ev
    PATCH_LIB("libhdmpvecore.so", 0x66e28, "C0035FD6");  // _ZN5ABase19OperationTargetBaseD0Ev
    PATCH_LIB("libhdmpvecore.so", 0x7cc84, "C0035FD6");  // _ZN6HDmpve9Configure11GetInstanceEv
    PATCH_LIB("libhdmpvecore.so", 0x71e4c, "C0035FD6");  // Java_com_gcore_abase_URLRequest_nativeTaskProgress
    PATCH_LIB("libhdmpvecore.so", 0x5c170, "C0035FD6");  // _ZN5ABase12CIniFileImpl11WriteStringEPKcS2_S2_
    PATCH_LIB("libhdmpvecore.so", 0x835e0, "C0035FD6");  // _ZTv0_n24_N5ABase22ValueConstIteratorImplD0Ev
    PATCH_LIB("libhdmpvecore.so", 0x5efc0, "C0035FD6");  // _ZN5ABase10BundleImpl3GetEPKcS2_
    PATCH_LIB("libhdmpvecore.so", 0x7e818, "C0035FD6");  // _ZN5ABase15IPlatformObjectD2Ev
    PATCH_LIB("libhdmpvecore.so", 0x93788, "C0035FD6");  // _ZN6HDmpve6Plugin13PluginManager15UninstallByNameEPKc
    PATCH_LIB("libhdmpvecore.so", 0x99cec, "C0035FD6");  // _ZN6HDmpve6Plugin19RemoteConfigSerivceC2Ev
    PATCH_LIB("libhdmpvecore.so", 0x8e1c0, "C0035FD6");  // _ZN10ABase_Json13ValueIteratorC2Ev
    PATCH_LIB("libhdmpvecore.so", 0x5838c, "C0035FD6");  // _ZN5ABase14uECC_secp224r1Ev
    PATCH_LIB("libhdmpvecore.so", 0x61fc8, "C0035FD6");  // Java_com_gcore_abase_config_ConfigBundle_setLong
    PATCH_LIB("libhdmpvecore.so", 0x903f0, "C0035FD6");  // _ZNSt4pairIKN10ABase_Json5Value8CZStringES1_ED1Ev
    PATCH_LIB("libhdmpvecore.so", 0x96e38, "C0035FD6");  // _ZN6HDmpve6Plugin11SpanContextC1EPKc
    PATCH_LIB("libhdmpvecore.so", 0x8e19c, "C0035FD6");  // _ZN10ABase_Json18ValueConstIteratoraSERKNS_17ValueIteratorBaseE
    PATCH_LIB("libhdmpvecore.so", 0x68058, "C0035FD6");  // _ZN5ABase10ThreadPool12stopAllTasksEv
    PATCH_LIB("libhdmpvecore.so", 0x5193c, "C0035FD6");  // _ZN5ABase19CApolloBufferWriter5WriteERNS_20_tagApolloBufferBaseE
    PATCH_LIB("libhdmpvecore.so", 0x8e244, "C0035FD6");  // _ZN10ABase_Json5Value11CommentInfoD1Ev
    PATCH_LIB("libhdmpvecore.so", 0x9abd8, "C0035FD6");  // _ZN6HDmpve6Plugin14ServiceManager15ReleaseInstanceEv
    PATCH_LIB("libhdmpvecore.so", 0x49f0c, "C0035FD6");  // _ZN5ABase13base64_encodeEPKhj
    PATCH_LIB("libhdmpvecore.so", 0x82eb4, "C0035FD6");  // _ZN5ABase21ValueIteratorBaseImplC1ERKSt17_Rb_tree_iteratorISt4pairIKNS_11ValueStringENS_5ValueEEE
    PATCH_LIB("libhdmpvecore.so", 0x69770, "C0035FD6");  // _ZN5ABase17OperationQueueImp12createThreadEv
    PATCH_LIB("libhdmpvecore.so", 0x70534, "C0035FD6");  // _ZN5ABase15UrlResponseImpl9SetHeaderEPKcS2_
    PATCH_LIB("libhdmpvecore.so", 0x4d308, "C0035FD6");  // _ZN5ABase11TdrTypeUtil14compareTdrTimeEjj
    PATCH_LIB("libhdmpvecore.so", 0x5d524, "C0035FD6");  // _ZN5ABase20EncryptedIniFileImplC2EPKc
    PATCH_LIB("libhdmpvecore.so", 0x8e658, "C0035FD6");  // _ZNK10ABase_Json5Value8CZString14isStaticStringEv
    PATCH_LIB("libhdmpvecore.so", 0x92b88, "C0035FD6");  // _ZNSt6vectorIN5ABase18CAFunctionSelectorESaIS1_EE8_M_eraseEN9__gnu_cxx17__normal_iteratorIPS1_S3_EE
    PATCH_LIB("libhdmpvecore.so", 0x99138, "C0035FD6");  // _ZThn32_N6HDmpve6Plugin21HDmpveCoreInnerPlugin16GetServiceByNameEPKc
    PATCH_LIB("libhdmpvecore.so", 0x5e974, "C0035FD6");  // _ZN5ABase6BundleD0Ev
    PATCH_LIB("libhdmpvecore.so", 0x6b818, "C0035FD6");  // _ZN5ABase16CNetworkObserverC1Ev
    PATCH_LIB("libhdmpvecore.so", 0x642b4, "C0035FD6");  // _ZN5ABase11AFileLogger9LogToFileEPKc
    PATCH_LIB("libhdmpvecore.so", 0x6829c, "C0035FD6");  // _ZN5ABase10ThreadPool13tryShrinkPoolEv
    PATCH_LIB("libhdmpvecore.so", 0x76bf4, "C0035FD6");  // _ZN6HDmpve16RemoteGameConfig11AddObserverEPNS_20RemoteConfigObserverE
    PATCH_LIB("libhdmpvecore.so", 0x5126c, "C0035FD6");  // _ZN5ABase16_tagApolloActionD1Ev
    PATCH_LIB("libhdmpvecore.so", 0x50f64, "C0035FD6");  // _ZN5ABase10ITdrObject11fromXmlFileEPKcNS_12TdrXmlFormatEj
    PATCH_LIB("libhdmpvecore.so", 0x8a784, "C0035FD6");  // _ZN10ABase_Json12StyledWriter15writeArrayValueERKNS_5ValueE
    PATCH_LIB("libhdmpvecore.so", 0x66da4, "C0035FD6");  // _ZNK5ABase15ObjectOperation9DuplicateEv
    PATCH_LIB("libhdmpvecore.so", 0x93610, "C0035FD6");  // _ZNSt8_Rb_treeISsSt4pairIKSsN6HDmpve6Plugin20_tagPluginDescriptorEESt10_Select1stIS5_ESt4lessISsESaIS5_EE5eraseB5cxx11ESt17_Rb_tree_iteratorIS5_E
    PATCH_LIB("libhdmpvecore.so", 0x642e8, "C0035FD6");  // _ZN5ABase11AFileLogger17RemoveOldLogFilesEi

    // === libTBlueData.so (39 targets) ===
    PATCH_LIB("libTBlueData.so", 0x4d1cc, "C0035FD6");  // _ZN3TDM10ITBlueData11GetInstanceEv
    PATCH_LIB("libTBlueData.so", 0x8e854, "C0035FD6");  // Java_com_blue_dm_TBlueData_TDMEnableReport
    PATCH_LIB("libTBlueData.so", 0x8f5c4, "C0035FD6");  // Java_com_blue_dm_TBlueData_TDMReportBinary
    PATCH_LIB("libTBlueData.so", 0x8ff3c, "C0035FD6");  // Java_com_blue_dm_system_FileUtils_FileUtilsInit
    PATCH_LIB("libTBlueData.so", 0x4d258, "C0035FD6");  // _ZN3TDM10ITBlueData15ReleaseInstanceEv
    PATCH_LIB("libTBlueData.so", 0x8e81c, "C0035FD6");  // Java_com_blue_dm_TBlueData_TDMPause
    PATCH_LIB("libTBlueData.so", 0x8e8bc, "C0035FD6");  // Java_com_blue_dm_TBlueData_TDMGetUID
    PATCH_LIB("libTBlueData.so", 0x8e7e4, "C0035FD6");  // Java_com_blue_dm_TBlueData_TDMResume
    PATCH_LIB("libTBlueData.so", 0x8ea38, "C0035FD6");  // Java_com_blue_dm_TBlueData_TDMGetSDKVerision
    PATCH_LIB("libTBlueData.so", 0x92328, "C0035FD6");  // _ZN6HDmpve6Plugin12PluginReportD0Ev
    PATCH_LIB("libTBlueData.so", 0x92380, "C0035FD6");  // _ZThn8_NK6HDmpve6Plugin12PluginReport7GetNameEv
    PATCH_LIB("libTBlueData.so", 0x6e484, "C0035FD6");  // _Z20RegisterHDmpvePluginv
    PATCH_LIB("libTBlueData.so", 0x923c0, "C0035FD6");  // _ZThn8_N6HDmpve6Plugin12PluginReport13OnPreShutdownEv
    PATCH_LIB("libTBlueData.so", 0x8e020, "C0035FD6");  // Java_com_blue_dm_database_TDMDataBase_DataBaseInit
    PATCH_LIB("libTBlueData.so", 0x922e4, "C0035FD6");  // _ZThn8_N6HDmpve6Plugin12PluginReport16GetServiceByNameEPKc
    PATCH_LIB("libTBlueData.so", 0x8ebf8, "C0035FD6");  // Java_com_blue_dm_TBlueData_TDMReportEvent
    PATCH_LIB("libTBlueData.so", 0x71e48, "C0035FD6");  // Java_com_blue_dm_TBlueData_setAppDir
    PATCH_LIB("libTBlueData.so", 0x92370, "C0035FD6");  // _ZN6HDmpve6Plugin12PluginReport10OnShutdownEv
    PATCH_LIB("libTBlueData.so", 0x92344, "C0035FD6");  // _ZN6HDmpve6Plugin12PluginReport9OnStartupEPNS0_16IServiceRegisterE
    PATCH_LIB("libTBlueData.so", 0x92398, "C0035FD6");  // _ZThn8_N6HDmpve6Plugin12PluginReport9OnStartupEPNS0_16IServiceRegisterE
    PATCH_LIB("libTBlueData.so", 0x91ff0, "C0035FD6");  // TDM_Report_Binary
    PATCH_LIB("libTBlueData.so", 0x8e7ac, "C0035FD6");  // Java_com_blue_dm_TBlueData_TDMInit
    PATCH_LIB("libTBlueData.so", 0x923c4, "C0035FD6");  // _ZThn8_N6HDmpve6Plugin12PluginReport10OnShutdownEv
    PATCH_LIB("libTBlueData.so", 0x9238c, "C0035FD6");  // _ZThn8_NK6HDmpve6Plugin12PluginReport10GetVersionEv
    PATCH_LIB("libTBlueData.so", 0x8ebb4, "C0035FD6");  // Java_com_blue_dm_TBlueData_TDMSetLogLevel
    PATCH_LIB("libTBlueData.so", 0x8f7ac, "C0035FD6");  // Java_com_blue_dm_TBlueData_TDMReportLogin
    PATCH_LIB("libTBlueData.so", 0x9232c, "C0035FD6");  // _ZNK6HDmpve6Plugin12PluginReport7GetNameEv
    PATCH_LIB("libTBlueData.so", 0x8e084, "C0035FD6");  // Java_com_blue_dm_system_TDMUtils_Init
    PATCH_LIB("libTBlueData.so", 0x9236c, "C0035FD6");  // _ZN6HDmpve6Plugin12PluginReport13OnPreShutdownEv
    PATCH_LIB("libTBlueData.so", 0x91d74, "C0035FD6");  // TDM_Initialize
    PATCH_LIB("libTBlueData.so", 0x92368, "C0035FD6");  // _ZN6HDmpve6Plugin12PluginReport13OnPostStartupEv
    PATCH_LIB("libTBlueData.so", 0x923bc, "C0035FD6");  // _ZThn8_N6HDmpve6Plugin12PluginReport13OnPostStartupEv
    PATCH_LIB("libTBlueData.so", 0x8f914, "C0035FD6");  // Java_com_blue_dm_TBlueData_TDMEnableDeviceInfo
    PATCH_LIB("libTBlueData.so", 0x92378, "C0035FD6");  // _ZThn8_N6HDmpve6Plugin12PluginReportD0Ev
    PATCH_LIB("libTBlueData.so", 0x91e28, "C0035FD6");  // TDM_Enable_Report
    PATCH_LIB("libTBlueData.so", 0x92324, "C0035FD6");  // _ZN6HDmpve6Plugin12PluginReportD2Ev
    PATCH_LIB("libTBlueData.so", 0x922a4, "C0035FD6");  // _ZN6HDmpve6Plugin12PluginReport16GetServiceByNameEPKc
    PATCH_LIB("libTBlueData.so", 0x92374, "C0035FD6");  // _ZThn8_N6HDmpve6Plugin12PluginReportD1Ev
    PATCH_LIB("libTBlueData.so", 0x92338, "C0035FD6");  // _ZNK6HDmpve6Plugin12PluginReport10GetVersionEv

    // === libhdmpve.so (231 targets) ===
    PATCH_LIB("libhdmpve.so", 0xc8898, "C0035FD6");  // hdmpve_custom_account_adapter_install
    PATCH_LIB("libhdmpve.so", 0xb95a0, "C0035FD6");  // hdmpve_get_connector_factory_instance
    PATCH_LIB("libhdmpve.so", 0x3d94cc, "C0035FD6");  // hdmpve_gromelink_getVersion
    PATCH_LIB("libhdmpve.so", 0x13d18c, "C0035FD6");  // Java_com_gcore_hdmpve_netinterface_MsgWorker_dnsQueryCallback
    PATCH_LIB("libhdmpve.so", 0xb9514, "C0035FD6");  // hdmpve_pay_Dipose
    PATCH_LIB("libhdmpve.so", 0xc1724, "C0035FD6");  // _ZN6HDmpve15IAccountServiceD0Ev
    PATCH_LIB("libhdmpve.so", 0xc149c, "C0035FD6");  // hdmpve_none_account_initialize
    PATCH_LIB("libhdmpve.so", 0x12fb5c, "C0035FD6");  // _ZN6HDmpve4Conn13NameRouteInfoD0Ev
    PATCH_LIB("libhdmpve.so", 0x2e9a98, "C0035FD6");  // InitVersionMgr
    PATCH_LIB("libhdmpve.so", 0x10e074, "C0035FD6");  // hdmpve_releaseInstance
    PATCH_LIB("libhdmpve.so", 0xc7e80, "C0035FD6");  // _ZN6HDmpve5TokenD0Ev
    PATCH_LIB("libhdmpve.so", 0xbbf5c, "C0035FD6");  // _ZNSt12_Vector_baseIPN6HDmpve16IServiceObserverESaIS2_EED1Ev
    PATCH_LIB("libhdmpve.so", 0x12d2ec, "C0035FD6");  // _ZNSt8_Rb_treeISsSt4pairIKSsPN6HDmpve4Conn10IConnectorEESt10_Select1stIS6_ESt4lessISsESaIS6_EE8_M_eraseEPSt13_Rb_tree_nodeIS6_E
    PATCH_LIB("libhdmpve.so", 0x1b3ac0, "C0035FD6");  // _ZN15ChannelInfoUtil13isV2SignatureEPKc
    PATCH_LIB("libhdmpve.so", 0x179acc, "C0035FD6");  // _ZN6HDmpve13ZoneRouteInfoD1Ev
    PATCH_LIB("libhdmpve.so", 0xb7cf8, "C0035FD6");  // _ZN6HDmpve14IPluginManager15ReleaseInstanceEv
    PATCH_LIB("libhdmpve.so", 0x3c6e98, "C0035FD6");  // _ZN6HDmpve5ITDirC1Ev
    PATCH_LIB("libhdmpve.so", 0xb92e8, "C0035FD6");  // hdmpve_pay_action
    PATCH_LIB("libhdmpve.so", 0x179a64, "C0035FD6");  // _ZN6HDmpve11WaitingInfoD2Ev
    PATCH_LIB("libhdmpve.so", 0x1934a4, "C0035FD6");  // InitPufferManager
    PATCH_LIB("libhdmpve.so", 0x3d94d8, "C0035FD6");  // hdmpve_gromelink_createSocket
    PATCH_LIB("libhdmpve.so", 0xb83bc, "C0035FD6");  // _ZN6HDmpve15CSdkFactoryBaseC2Ev
    PATCH_LIB("libhdmpve.so", 0x17cb0c, "C0035FD6");  // _ZN6HDmpve12RealNameInfoD2Ev
    PATCH_LIB("libhdmpve.so", 0x1799f4, "C0035FD6");  // _ZNK6HDmpve15ServerRouteInfo5CloneEv
    PATCH_LIB("libhdmpve.so", 0x12f38c, "C0035FD6");  // hdmpve_connector_set_protocol_version
    PATCH_LIB("libhdmpve.so", 0x12f6cc, "C0035FD6");  // hdmpve_connector_set_connectExtInt
    PATCH_LIB("libhdmpve.so", 0xb8718, "C0035FD6");  // _ZN6HDmpve12CServiceBase11AddObserverEPNS_16IServiceObserverE
    PATCH_LIB("libhdmpve.so", 0x17ce04, "C0035FD6");  // _ZNK6HDmpve13ConnectedInfo5CloneEv
    PATCH_LIB("libhdmpve.so", 0x13b534, "C0035FD6");  // Java_com_gcore_hdmpve_netinterface_MsgWorker_bindCallback
    PATCH_LIB("libhdmpve.so", 0xba3c4, "C0035FD6");  // hdmpve_account_realname_auth
    PATCH_LIB("libhdmpve.so", 0x2eaa8c, "C0035FD6");  // UnInitDataPreDownloader
    PATCH_LIB("libhdmpve.so", 0xb8340, "C0035FD6");  // _ZN6HDmpve11CPluginBase17OnApplicationQuitEv
    PATCH_LIB("libhdmpve.so", 0x179a98, "C0035FD6");  // _ZN6HDmpve15ServerRouteInfoD2Ev
    PATCH_LIB("libhdmpve.so", 0x3d9534, "C0035FD6");  // hdmpve_gromelink_destorySocket
    PATCH_LIB("libhdmpve.so", 0x12e85c, "C0035FD6");  // _ZN6HDmpve4Conn13ZoneRouteInfoD0Ev
    PATCH_LIB("libhdmpve.so", 0xba9cc, "C0035FD6");  // hdmpve_account_IsPlatformSupportApi
    PATCH_LIB("libhdmpve.so", 0xba224, "C0035FD6");  // hdmpve_account_initialize
    PATCH_LIB("libhdmpve.so", 0x3d9484, "C0035FD6");  // hdmpve_gromelink_getFactory
    PATCH_LIB("libhdmpve.so", 0x179ae0, "C0035FD6");  // _ZN6HDmpve13ZoneRouteInfoD0Ev
    PATCH_LIB("libhdmpve.so", 0x3c680c, "C0035FD6");  // hdmpve_tdir_queryall

    // === libsigner.so (1 targets) ===
    PATCH_LIB("libsigner.so", 0x9afc0, "C0035FD6");  // Java_com_adjust_sdk_sig_NativeLibHelper_nSign

    // Total patches: 130
}
