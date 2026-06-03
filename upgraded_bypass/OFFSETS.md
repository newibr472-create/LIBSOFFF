# ARM64 Offset Mapping — PUBG Mobile 4.4.0
# WOLVES PVT BYPASS v4.0

## libanogs.so (AnoGS Anti-Cheat)

| ARM32 (old v3.8.1) | ARM64 (new v4.0) | Function | Purpose |
|---|---|---|---|
| 0x3766E0 | **0x1D551C** | AnoSDKGetReportData | Device info reporting block |
| 0xF1E3E | **0x1D7938** | AnoSDKGetReportData3 | Report data v3 (trampoline → 0x1CC128) |
| — | **0x1D78CC** | AnoSDKGetReportData2 | Report data v2 (trampoline → 0x1C79D4) |
| — | **0x1D3814** | AnoSDKInit | SDK init |
| — | **0x1D6EA8** | AnoSDKIoctl | IOCTL comm channel |
| — | **0x1D6598** | AnoSDKIoctlOld | IOCTL old channel |
| — | **0x1DB894** | JNI_OnLoad | JNI entry point |

## libUE4.so (Game Engine — from SDK dump)

| ARM32 (old) | ARM64 (new) | Function | Purpose |
|---|---|---|---|
| 0x427AD74 | **0x7625FB0** | IsEmulator | Runtime emulator detect → returns false |
| — | **0x7625FE8** | IsEmulatorWhenInit | Init-time emulator detect → returns false |
| — | **0x761E918** | GetEmulatorName | Get emulator name → returns null |
| — | **0x6FB028C** | ReportAntiCheatInfo | Report AC info → blocked |
| — | **0x6D95FEC** | ReportAntiCheatDetailData | Report detailed AC data → blocked |
| — | **0x6FA37C4** | CheckNeedReport | Check if SACData should report → false |
| — | **0x6FB78A0** | ServerHandleHitDataArray | Server handle hit data → blocked |
| — | **0x6FAAA44** | HandleHitDataArray | Handle hit data array → blocked |
| 0x4CD1AD8 | — | AC Struct Init | Obfuscated in ARM64 — use SDK approach |
| 0x6D064B0 | — | AC Obfuscated | Replaced by ReportAntiCheatInfo hook |
| 0x6B505C8 | — | AC Data Parse | Replaced by CheckNeedReport hook |
| 0x6B500F8 | — | AC Data Read | Replaced by HandleHitDataArray hook |
| 0x6B3DED0 | — | Core AC Thread | Replaced by IsEmulator + report hooks |
| 0x4CD33B4 | — | AC Full Init | Replaced by SDK-level approach |
| 0x79DFEEC | — | PhysX ContactReport | Complex vtable — needs separate analysis |

## PlayerController Member Offsets (ARM64)

| Offset | Member | Purpose |
|---|---|---|
| 0x23E0 | AntiCheatManagerComp | PlayerAntiCheatManager pointer |
| 0x23F0 | bShouldReportAntiCheat | Bool — can be patched to 0 |
| 0x3DD8 | bUseAntiDataReportFilter | Bool — report filter |
| 0x3DD9 | bUseAntiDataReportFilterNew | Bool — new filter |
| 0x3DDA | bUseAntiDataReportFilterCheck | Bool — filter check |

## SceneComponent AntiCheatRandValue Offsets (ARM64)

| Offset | Member |
|---|---|
| 0x2D0 | AntiCheatRandValue0 |
| 0x2D8 | AntiCheatRandValue1 |
| 0x2E0 | AntiCheatRandValue2 |
| 0x2E8 | AntiCheatRandValue3 |
| 0x2F0 | AntiCheatRandValue4 |
| 0x2F8 | AntiCheatRandValue5 |
| 0x300 | AntiCheatRandValue6 |

## libTDataMaster.so (Telemetry)

| ARM32 (old) | ARM64 (new) | Function | Purpose |
|---|---|---|---|
| 0x1B4CC | **0x4D47C** | GetInstance | TDM singleton → return null |
| 0x6E778 | **0xA74A0** | SIGPIPE handler | Signal handler bypass |
| 0x6CB8C | **0x4E33C** | Report send | Core report dispatch → blocked |
| 0x248E0 | **0x569C0** | AC config init | Config init → early return |
| — | **0x653EC** | AC config check | Config verification → skip |
| — | **0x53138** | Report_Binary vtbl | Report binary vtable impl |
| — | **0x92534** | TDM_Initialize | C API init |

## PluginReport vtable (in libTDataMaster.so on ARM64!)

| ARM32 (libgcloud.so) | ARM64 (libTDataMaster.so) | Function | Note |
|---|---|---|---|
| 0x1B4D8 | **0x92AE4** | D2 destructor | Already ret on ARM64 |
| 0x1B4E4 | **0x92AE8** | D0 destructor | Deletes object |
| — | **0x92B04** | OnStartup | 36 bytes — needs hook |
| 0x1B514 | **0x92B28** | OnPostStartup | Already ret (no-op!) |
| 0x1B520 | **0x92B2C** | OnPreShutdown | Already ret (no-op!) |
| 0x1B52C | **0x92B30** | OnShutdown | Already ret (no-op!) |

## libgcloud.so (Cloud SDK)

| ARM32 (old) | ARM64 (new) | Function | Purpose |
|---|---|---|---|
| — | **0x097D18** | PluginManager::GetInstance | Return null |
| — | **0xF4AEC** | gcloud_init | Prevent init |
| — | **0x097DFC** | ReleaseInstance | Release |

## Total Hooks Installed: 13 (v4.0) vs 17 (v3.8.1)

v4.0 uses a smarter approach:
- 3 libanogs hooks (GetReportData/2/3)
- 8 libUE4 hooks (IsEmulator + report blocking)
- 4 libTDataMaster hooks (GetInstance, ReportSend, ACConfig, OnStartup)
- 2 libgcloud hooks (GetInstance, init)
+ 4 libc hooks (property spoof, file block) [NEW — was dead code before]
+ 2 CRC evasion hooks (memcpy, memcmp) [NEW — was dead code before]
**Total: 21 active hooks (vs 17 in v3.8.1, but 8 were broken/not installed)**
