# BGMI 4.4.0 TRUE BYPASS - Complete Documentation

## Overview

This bypass neutralizes ALL anti-cheat protections in BGMI (Battlegrounds Mobile India) v4.4.0.
Every offset is verified from deep binary analysis using LIEF + Capstone on the ARM64 libraries.

---

## Target Libraries

| Library | Size | Purpose | Hooks |
|---------|------|---------|-------|
| `libanogs.so` | 5.5MB | Tencent AnoSDK - Main anti-cheat engine | 20 |
| `libhdmpvecore.so` | 1.4MB | HDMP Core - Root detection, uploads, network | 13 |
| `libTBlueData.so` | 4.0MB | TBlue - Telemetry & data reporting | 15 |
| `libhdmpve.so` | 6.8MB | HDMP Main - Initialization, verification | 12 |
| `libsigner.so` | 983KB | APK signature verification (Adjust SDK) | 2 |
| **TOTAL** | | | **62 hooks** |

---

## Files

| File | Description |
|------|-------------|
| `bypass.js` | Frida script - Full runtime hooking with system call interception |
| `bypass_gg.lua` | GameGuardian script - Direct memory patching at offsets |
| `analysis_libanogs.json` | Deep analysis of AnoSDK |
| `analysis_libhdmpvecore.json` | Deep analysis of HDMP Core |
| `analysis_libTBlueData.json` | Deep analysis of TBlue telemetry |
| `analysis_libhdmpve.json` | Deep analysis of HDMP main |
| `analysis_libsigner.json` | Deep analysis of signer |
| `ANALYSIS_SUMMARY.md` | Human-readable summary of all findings |

---

## Usage

### Frida (Recommended - Most Complete)

```bash
# Root + Frida on device
frida -U -f com.pubg.imobile -l bypass.js --no-pause

# Or attach to running process
frida -U com.pubg.imobile -l bypass.js
```

### GameGuardian

1. Open BGMI
2. Open GameGuardian → Select BGMI process
3. Load `bypass_gg.lua`
4. Wait for "RESULTS" popup
5. If some libraries not loaded → enter lobby → Menu → "Re-apply all patches"

---

## What Each Hook Does

### 1. libanogs.so (AnoSDK) — 20 Functions

AnoSDK is Tencent's proprietary anti-cheat. It performs:
- Memory scanning for known cheat patterns
- Process monitoring (TracerPid, /proc/self/maps)
- Inline hook detection
- Reporting cheat data to servers

| Function | Offset | What It Does | Bypass Action |
|----------|--------|--------------|---------------|
| `AnoSDKInit` | `0x1d3814` | Initialize AC engine | Return 0 (fail silently) |
| `AnoSDKInitEx` | `0x1d3b40` | Extended initialization | Return 0 |
| `AnoSDKSetUserInfo` | `0x1d417c` | Send user identity to AC server | Return 0 (don't send) |
| `AnoSDKSetUserInfoWithLicense` | `0x1d4580` | Licensed user tracking | Return 0 |
| `AnoSDKOnPause` | `0x1d4c0c` | Lifecycle event (app paused) | Return 0 |
| `AnoSDKOnResume` | `0x1d5030` | Lifecycle event (app resumed) | Return 0 |
| `AnoSDKGetReportData` | `0x1d551c` | **CRITICAL**: Get cheat detection report | Return 0 + set size=0 |
| `AnoSDKDelReportData` | `0x1d5a88` | Delete sent reports | Return 0 |
| `AnoSDKOnRecvData` | `0x1d624c` | Receive server commands (kick/ban) | Return 0 (ignore commands) |
| `AnoSDKIoctlOld` | `0x1d6598` | Old control interface | Return 0 |
| `AnoSDKIoctl` | `0x1d6ea8` | **CRITICAL**: Main control/scan commands | Return 0 (all commands succeed) |
| `AnoSDKFree` | `0x1d7398` | Free AC memory | Return 0 |
| `AnoSDKGetReportData2` | `0x1d78cc` | Report variant 2 | Return 0 |
| `AnoSDKGetReportData3` | `0x1d7938` | Report variant 3 | Return 0 |
| `AnoSDKDelReportData3` | `0x1d79a4` | Delete report 3 | Return 0 |
| `AnoSDKGetReportData4` | `0x1d7fc4` | Report variant 4 | Return 0 |
| `AnoSDKDelReportData4` | `0x1d82cc` | Delete report 4 | Return 0 |
| `AnoSDKOnRecvSignature` | `0x1d88ec` | Verify server signature | Return 0 |
| `AnoSDKRegistInfoListener` | `0x1d8c74` | Register info callback | Return 0 |
| `AnoSDKForExport` | `0x1d9024` | Export utility | Return 0 |

**Note**: `JNI_OnLoad` @ `0x1db894` is NOT patched — it's needed for JVM registration.

---

### 2. libhdmpvecore.so — 13 Functions

HDMP Core handles root detection, device fingerprinting, and data upload.

| Function | Offset | What It Does | Bypass Action |
|----------|--------|--------------|---------------|
| `CSystem::IsDeviceRooted` | `0x72cac` | C++ root check | Return 0 (not rooted) |
| `ABase_system_IsDeviceRooted` | `0x73150` | C API root check | Return 0 |
| `CSystem::CheckPermission` | `0x72d3c` | Permission validation | Return 1 (permitted) |
| `UploadTaskImpl::Upload` | `0x6fc88` | Upload cheat data | Return 0 (block) |
| `WWW::UploadFromData` | `0x6d660` | Upload raw data | Return 0 (block) |
| `UploadTaskImpl::Finish` | `0x6d900` | Complete upload | Return 0 |
| `ConfigureReport::ReportPullConfigResult` | `0x79944` | Report config fetch | Return 0 |
| `CNetwork::init` | `0x6c594` | Network initialization | Return 0 |
| `UrlRequest::Initialize` | `0x70ff8` | HTTP request init | Return 0 (block requests) |
| `nativePostStartup` | `0x9b00c` | Plugin post-startup | Return 0 |
| `nativePreShutdown` | `0x9b054` | Plugin pre-shutdown | Return 0 |
| `nativeShutdown` | `0x9b09c` | Plugin shutdown | Return 0 |
| `nativeStartup` | `0x9afc4` | Plugin startup | Return 0 |

---

### 3. libTBlueData.so — 15 Functions

TBlue is Tencent's telemetry SDK. It reports device info, gameplay events, and "anomalies" (cheats) to servers.

| Function | Offset | What It Does | Bypass Action |
|----------|--------|--------------|---------------|
| `TDM_Initialize` | `0x91d74` | Init telemetry engine | Return 0 |
| `TDM_Report_Binary` | `0x91ff0` | **CRITICAL**: Send binary cheat report | Return 0 (block) |
| `TDM_Enable_Report` | `0x91e28` | Enable/disable reporting | Return 0 (disabled) |
| `TDMInit` (JNI) | `0x8e7ac` | Java-side init | Return 0 |
| `TDMEnableReport` (JNI) | `0x8e854` | Java enable report | Return 0 |
| `TDMReportBinary` (JNI) | `0x8f5c4` | Java binary report | Return 0 |
| `TDMReportEvent` (JNI) | `0x8ebf8` | Java event report | Return 0 |
| `TDMReportLogin` (JNI) | `0x8f7ac` | Java login event | Return 0 |
| `TDMEnableDeviceInfo` (JNI) | `0x8f914` | Device info collection | Return 0 |
| `TDMUtils::EncryptField` | `0x8e308` | Encrypt report data | Return 0 (don't encrypt) |
| `SetDeviceInfoEncryptKey` | `0x8e578` | Set encryption key | Return 0 |
| `PluginReport::OnStartup` | `0x92344` | Report plugin init | Return 0 |
| `PluginReport::OnShutdown` | `0x92370` | Report plugin shutdown | Return 0 |
| `PluginReport::OnPreShutdown` | `0x9236c` | Pre-shutdown report | Return 0 |
| `PluginReport::OnPostStartup` | `0x92368` | Post-startup report | Return 0 |

---

### 4. libhdmpve.so — 12 Functions

HDMP main library handles initialization, APK verification, network sniffing, and data downloads.

| Function | Offset | What It Does | Bypass Action |
|----------|--------|--------------|---------------|
| `hdmpve_none_account_initialize` | `0xc149c` | Anonymous init | Return 0 |
| `hdmpve_account_initialize` | `0xba224` | Account init | Return 0 |
| `hdmpve_tdir_initialize` | `0x3c6a74` | Directory init | Return 0 |
| `hdmpve_connector_Initialize` | `0x12f798` | Connector init | Return 0 |
| `hdmpve_custom_account_initialize` | `0xc88bc` | Custom account init | Return 0 |
| `CheckAppUpdate` | `0x2e9b90` | Version/integrity check | Return 0 |
| `IFSDllVerifyApiCompatable` | `0x247a9c` | API compatibility | Return 1 (compatible) |
| `isV2Signature` | `0x1b3ac0` | V2 APK signature check | Return 1 (valid) |
| `apollo_net_sniffer_Init` | `0x3f4e5c` | Network traffic sniffer | Return 0 (disabled) |
| `InitVersionMgr` | `0x2e9a98` | Version manager | Return 0 |
| `InitDataDownloader` | `0x2ea59c` | Data downloader | Return 0 |
| `InitPufferManager` | `0x1934a4` | Resource manager | Return 0 |

---

### 5. libsigner.so — 2 Functions

Adjust SDK signature verification. Prevents detection of modified APK.

| Function | Offset | What It Does | Bypass Action |
|----------|--------|--------------|---------------|
| `nOnResume` | `0x9a088` | Resume signature check | Return 0 (null) |
| `nSign` | `0x9afc0` | Generate APK signature | Return 0 (null → unsigned mode) |

---

### 6. System Call Hooks (Frida only)

| Target | Purpose | Bypass Action |
|--------|---------|---------------|
| `open()` / `openat()` | Read /proc, root paths | Redirect root paths to /dev/null |
| `read()` | Read /proc/self/status | Spoof TracerPid to 0 |
| `fgets()` | Read /proc/self/maps | Filter frida/magisk/xposed lines |
| `ptrace()` | Anti-debug (TRACEME) | Return 0 (success) |
| `kill()` | Self-termination on detect | Block self-kill signals |
| `strstr()` | String matching for root/frida | Return NULL for sensitive needles |
| `access()` / `stat()` | Check root binary existence | Return -1 (not found) |
| `inotify_add_watch()` | Monitor file changes | Block /proc watches |
| `fork()` | Spawn monitor processes | Allow but monitor |
| `pthread_create()` | AC thread creation | Monitor, don't block |

---

### 7. Java Layer Hooks (Frida only)

| Target | Purpose | Bypass Action |
|--------|---------|---------------|
| `PackageManager.getPackageInfo` | Signature verification | Pass through (monitoring) |
| `Build.*` properties | Emulator detection | Spoof to real device (Pixel 4) |
| `Settings.Secure.getString` | Dev mode / ADB detection | Return "0" (disabled) |

---

## Key Anti-Cheat Strings Found (from analysis)

```
"debugger=%s"           → Debugger detection reporting
"debugger:%s"           → Debugger state check
"|emulator_name="       → Emulator detection
"ms_scan_start"         → Memory scan initiation
"set_inline_hook_error" → Hook detection
"inline_hook_opcode_dismatch" → Hook detection (opcode mismatch)
"mt2_no_root"           → Root check flag
"check_state=%s"        → State verification
"check_state:%s"        → State verification variant
"ChkSetGameStatus{"     → Game status integrity
"tdm_report"            → Telemetry data manager report
"REPORT"                → Generic report marker
"COREREPORT"            → Core anti-cheat report
"ReportComplaint"       → Player report
"ScanCast"              → Memory scan
"wild scan"             → Pattern/wild memory scan
"hash_cache"            → Hash verification cache
"ms_mmap"               → Memory mapping scan
"ms_data_crc"           → CRC integrity check
"init_info->tss_sdk_send_data_to_svr" → TSS SDK server communication
"/data/data/%s/files"   → App data directory scan
"/data/user/10/%s/files"→ Multi-user app scan
"%s;crc:%s"             → CRC validation format
"mmap_fialed" [sic]     → mmap failure detection (typo in original)
"mrpcs_scan_thread_start_failed!" → Scan thread failure
```

---

## Known Limitations

1. **Game may crash** if certain init functions return too early. If so, let `AnoSDKInit` run but patch only reporting functions.
2. **Server-side detection**: If the server expects periodic AC reports and gets none, it may flag the account after some time.
3. **Version-specific**: These offsets are for BGMI 4.4.0 ARM64 ONLY. Different version = different offsets.
4. **No kernel bypass**: If the game uses kernel-level integrity checks, this userspace bypass won't help.

---

## Troubleshooting

| Problem | Solution |
|---------|----------|
| Game crashes on start | Remove `AnoSDKInit` and `AnoSDKInitEx` from patches |
| Game crashes in lobby | Remove `hdmpve_*_initialize` patches |
| Kicked after 5 min | Server detected missing reports. Add a timer to send fake reports |
| "Libraries not loaded" | Re-run bypass after game fully loads (in lobby) |
| GG crashes | Use split patches (one lib at a time) |

---

## Analysis Method

All offsets were obtained through:
1. **LIEF** - ELF parsing, export/import extraction, section analysis
2. **Capstone** - ARM64 disassembly of function prologues
3. **String extraction** - Pattern matching for AC-related strings
4. **Cross-referencing** - Mapping string references to code locations
5. **Init array analysis** - Finding auto-run constructors

No Ghidra decompilation was needed — the export names are descriptive enough to identify function purposes, and the disassembly confirms they are hookable standard ARM64 functions.
