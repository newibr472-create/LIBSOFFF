# WOLVES PVT BYPASS v3.8.1 — Deep Analysis & Upgrade Roadmap

## Project Overview

**Project Name:** WOLVES PVT BYPASS (aka "RADAHN BP") v3.8.1  
**Target:** PUBG Mobile / BGMI anti-cheat bypass for GameLoop (Tencent Emulator)  
**Architecture:** Dual-component — Android native .so bypass + Windows C# loader  
**Current State:** ARM32-only (armeabi-v7a), requires upgrade to ARM64 (arm64-v8a) for PUBG Mobile 4.4.0+

---

## What Was Found (Kya Mila)

### Component 1: Android Native Bypass (.so)

**Output:** `libAkAudioVisiual.so` (disguised name)  
**Framework:** Dobby inline hooking + KittyMemory memory patching  
**Entry Point:** `__attribute__((constructor)) lib_main()` — auto-executes on dlopen

**4-Thread Architecture:**
1. **anogs_thread** — Waits for `libanogs.so`, creates shadow memory, hooks 2 AnoGS functions, does FD cleanup
2. **AntiBan_thread** — Waits for `libUE4.so`, creates shadow memory, hooks 7 UE4 anti-cheat functions
3. **TDataMaster_thread** — Waits for `libTDataMaster.so` + `libgcloud.so`, hooks 9 reporting functions
4. **PatchMonitor_thread** — Watchdog checking if anti-cheat restores patched bytes (jni version only)

**Bypass Mechanisms:**
- Emulator detection spoof via `__system_property_get()` hooking (17+ Android props spoofed as Samsung Galaxy S10e)
- File path blocking via `access/fopen/stat` hooks (16+ emulator indicator paths blocked)
- AnoGS (libanogs.so) device info + report data hooks — returns 0 to suppress telemetry
- UE4 engine anti-cheat hooks — emulator detect bypass, AC struct init, AC thread kill, data zeroing
- TDataMaster + libgcloud hooks — SIGPIPE bypass, report termination, config threshold spoofing (999/9999/99999)
- PhysX ContactReport vtable rewriting — custom vtable entries to bypass contact detection
- Shadow memory technique — copies .so to malloc'd buffer for CRC evasion
- FD cleanup — closes detectable memfd/ashmem file descriptors after hooking

### Component 2: Windows C# Loader (WOLF)

**Output:** `RADAHN BP.exe` (Windows Forms .NET 8.0)  
**UI:** Guna.UI2.WinForms custom theme  
**License:** KeyAuth (currently BYPASSED for testing)

**Injection Methods:**
1. `.so Library Replacement` — Push bypass .so as `libGVoicePlugin.so` via ADB
2. `Direct Memory Patching` — WriteProcessMemory on `AndroidEmulatorEx.exe` at RVA `0x427AD74` (2x NOP)
3. `Anti-Cheat Library Deletion` — Remove `libGVoicePlugin.so` + `libopenplatform.so` after game launch

**Version Support:** VN (com.vng.pubgmobile), KR (com.pubg.krmobile), GL (com.tencent.ig), TW (com.rekoo.pubgm)

**Anti-Debugging:** 46-process blacklist (IDA, x64dbg, Cheat Engine, Ghidra, Frida, etc.) → self-deletion on detection

---

## All Hook Offsets (ARM32 — Current)

### libanogs.so (2 hooks)
| Offset | Hook Function | Purpose |
|--------|--------------|---------|
| `0x3766E0` | hsub_3766E0 | Device info reporting block — returns 0 |
| `0xF1E3E` | hAnoSDKGetReportData3 | AnoSDK report data collection — returns 0 |

### libUE4.so (8 hooks + 21 internal offsets)
| Offset | Hook Function | Purpose |
|--------|--------------|---------|
| `0x427AD74` | ue4_emulator_detect_stub | UE4 emulator detection — returns 0 |
| `0x4CD1AD8` | hsub_4CD1AD8 | AntiCheat Struct Init — returns null vtable |
| `0x6D064B0` | hsub_6D064B0 | AntiCheat Obfuscated handler — returns 0 |
| `0x6B505C8` | hsub_6B505C8 | AntiCheat Data Parse — zeroes detection bytes |
| `0x6B500F8` | hsub_6B500F8 | AntiCheat Data Read — zeroes 7 struct fields |
| `0x6B3DED0` | hsub_6B3DED0 | Core AntiCheat Thread — kills thread |
| `0x4CD33B4` | hsub_4CD33B4 | AntiCheat Full Init — pass-through |
| `0x79DFEEC` | hsub_79DFEEC | PhysX ContactReport vtable rewrite |

**Internal offsets in hsub_79DFEEC (21 additional):**  
`0x79DFC80`, `0x79DEE34`, `0x79DEE28`, `0x79DEE4C`, `0x79DEE40`, `0x79DEE58`, `0x79DEE64`, `0x79DEE70`, `0x79DEE7C`, `0x79DEE88`, `0x79DEE94`, `0x79DEEA0`, `0x79DEEC8`, `0x79DEEE4`, `0x79DEEF0`, `0x79DEF08`, `0x79DEF14`, `0x79DEF20`, `0x79DEBA8`, `0x238FF3C`, `0x238F5A0`, `0x238EB7C`, `0x7867A24`

### libTDataMaster.so (4 hooks)
| Offset | Hook Function | Purpose |
|--------|--------------|---------|
| `0x6E778` | hsub_6E778 | SIGPIPE handler bypass |
| `0x6CB8C` | hsub_6CB8C | Report packet terminator |
| `0x1B4CC` | hTDM_GetInstance | TDM instance block |
| `0x248E0` | hsub_248E0 | AC config init — spoofed thresholds |

### libgcloud.so (5 hooks)
| Offset | Hook Function | Purpose |
|--------|--------------|---------|
| `0x1B4D8` | hPluginReport_D2 | PluginReport D2 suppress |
| `0x1B4E4` | hPluginReport_D0 | PluginReport D0 suppress |
| `0x1B514` | hOnPostStartup | OnPostStartup suppress |
| `0x1B520` | hOnPreShutdown | OnPreShutdown suppress |
| `0x1B52C` | hOnShutdown | OnShutdown suppress |

**Total: 19 unique hook offsets + 21 internal offsets = 40 offsets to update**

---

## Critical Bugs Found (Current Code)

| # | Severity | Issue |
|---|----------|-------|
| 1 | **CRITICAL** | `Android.mk` compiles `../BYPASS.cpp` (root version with everything commented out) instead of `jni/BYPASS.cpp` (production version with all hooks active) |
| 2 | **CRITICAL** | Shadow memory allocated but CRC redirect hooks (memcpy/memcmp) never installed — CRC evasion is non-functional |
| 3 | **CRITICAL** | libc hooks (`__system_property_get`, `access`, `fopen`, `stat`) defined but NEVER installed — emulator detection bypass is dead code |
| 4 | **HIGH** | All pointer globals are `DWORD` (uint32_t) — will truncate 64-bit addresses on ARM64 |
| 5 | **HIGH** | `hsub_79DFEEC` uses `_DWORD*` array indexing — completely broken on ARM64 (8-byte pointers) |
| 6 | **HIGH** | Dobby prebuilt only for `armeabi-v7a` — no ARM64 build exists |
| 7 | **MEDIUM** | KR cleanup button handler checks `gl.Checked` instead of `kr.Checked` — Korean cleanup never runs |
| 8 | **MEDIUM** | Self-deletion uses wrong executable name (`WOLVES.exe` vs `RADAHN BP`) |
| 9 | **LOW** | `OBFUSCATE()` is a no-op — all strings visible in binary |
| 10 | **LOW** | `PatchMonitor_thread` has empty g_patches vector — monitors nothing |

---

## What Needs to Be Done (Kya Karna Hai)

### Phase 1: Fix Critical Bugs (Before ARM64 Migration)

1. **Fix Android.mk** — Change `LOCAL_SRC_FILES := ../BYPASS.cpp` to `LOCAL_SRC_FILES := BYPASS.cpp`
2. **Merge two BYPASS.cpp files** — Consolidate root and jni versions into one with all hooks active
3. **Install libc hooks** — Add `DobbyHook()` calls for `__system_property_get`, `access`, `fopen`, `stat`
4. **Install CRC redirect hooks** — Add `DobbyHook()` for `memcpy`/`memcmp` to redirect reads to shadow memory
5. **Fix KR button handler** — Change `gl.Checked` to `kr.Checked`
6. **Fix self-deletion** — Use `Assembly.GetExecutingAssembly().Location` instead of hardcoded name

### Phase 2: ARM64 Migration (For PUBG Mobile 4.4.0+)

1. **Change all pointer types** — `DWORD` to `uintptr_t` for all base addresses and allocations
2. **Fix hook signatures** — Change `int` params to `uintptr_t`/pointer types for ARM64 compatibility
3. **Fix hsub_79DFEEC vtable rewriting** — Use `uint64_t*` instead of `_DWORD*`, recalculate all array indices
4. **Fix struct offsets** — All hardcoded struct offsets (1252, 12, 44, 60, 64, 76) must be recalculated from ARM64 IDA
5. **Update Application.mk** — Change `APP_ABI := armeabi-v7a` to `APP_ABI := arm64-v8a`
6. **Build Dobby for ARM64** — Compile from `Dobby_repo/` source for `arm64-v8a`
7. **Update lib paths** — Change `/system/lib/` to `/system/lib64/`, `lib/arm/` to `lib/arm64/`
8. **Update Windows loader paths** — All ADB push/copy paths from `lib/arm/` to `lib/arm64/`
9. **Update memory patch RVA** — `0x427AD74` is ARM32-specific, needs new RVA from ARM64 `AndroidEmulatorEx.exe`
10. **Fix ELF validation** — Invert ARM64 warning to be "compatible" and ARM32 to be "incompatible"

### Phase 3: Offset Updates (Per Version — PUBG 4.4.0)

Every PUBG update changes all function offsets. For version 4.4.0:

1. **Extract new APK** and decompile with apktool
2. **Analyze libanogs.so (arm64-v8a)** with IDA/Ghidra — find new offsets for device info + report data functions
3. **Analyze libUE4.so (arm64-v8a)** — find new offsets for all 8 UE4 hooks + 21 internal vtable offsets
4. **Analyze libTDataMaster.so (arm64-v8a)** — find new offsets for SIGPIPE, report, GetInstance, config init
5. **Analyze libgcloud.so (arm64-v8a)** — find new offsets for all 5 reporting hooks
6. **Update all header files** — anogs_hooks.h, ue4_hooks.h, tdm_hooks.h with new offsets
7. **Update BYPASS.cpp** — New offsets + ARM64-compatible types
8. **Update Form1.cs** — New emulator patch RVA for 64-bit GameLoop

### How to Find New Offsets (IDA/Ghidra Method)

| Library | Search Strategy |
|---------|----------------|
| libanogs.so | Search for "AnoSDKGetDeviceInfo" / "AnoSDKGetReportData" strings → trace xrefs to functions |
| libUE4.so | Search "emulator" / "4.3" strings for emu detect; search "AntiCheat" / "LinearDamping" for AC hooks |
| libTDataMaster.so | Search for "SIGPIPE" / "GetInstance" / signal handler patterns; singleton patterns |
| libgcloud.so | Search for "PluginReport" / "OnPostStartup" / "OnShutdown" lifecycle callbacks |

---

## File Structure

```
sam+bp.rar (original archive)
├── BYPASS.cpp                    # Main bypass (root - hooks commented out)
├── AkAudioVisiual.so             # Pre-built ARM32 bypass .so
├── libs/armeabi-v7a/libAkAudioVisiual.so  # Same .so in build output
├── hooks/
│   ├── anogs_hooks.h             # AnoGS hook definitions
│   ├── tdm_hooks.h               # TDataMaster + libgcloud hooks
│   └── ue4_hooks.h               # UE4 engine hooks
├── Includes/
│   ├── Logger.h                  # Android logcat wrapper
│   ├── Macros.h                  # HOOK_LIB / PATCH_LIB macros
│   ├── obfuscate.h               # String obfuscation (DISABLED)
│   └── Utils.h                   # Library base address lookups
├── jni/
│   ├── Android.mk                # NDK build (BUG: compiles wrong file)
│   ├── Application.mk            # ARM32 only
│   ├── BYPASS.cpp                # Production bypass (all hooks active)
│   ├── Dobby_inc/dobby.h         # Dobby header
│   ├── Dobby_repo/               # Full Dobby source code
│   └── KittyMemory/              # Memory patching header
├── WOLF/                         # Windows C# loader
│   ├── Form1.cs                  # Main UI + injection logic
│   ├── Form2.cs                  # Login screen (KeyAuth BYPASSED)
│   ├── auth/LicenseAuth.cs       # KeyAuth SDK
│   ├── WOLF.csproj               # .NET 8.0 project (Assembly: RADAHN BP)
│   └── bin1/                     # Pre-built Windows binaries
├── libGVoicePlugin.so            # Bypass .so named as GVoice plugin
└── libCrashSightPlugin.so        # Additional bypass .so variant
```

---

## KeyAuth Configuration

| Parameter | Value |
|-----------|-------|
| App Name | THVIP |
| Owner ID | 3JTXaaS3Mb |
| Version | 1.0 |
| Ed25519 Key | 5586b4bc69c7a4b487e4563a4cd96afd39140f919bd31cea7d1c6a1e8439422b |
| Status | **BYPASSED** (login button skips to Form1 directly) |

---

## Risk Assessment

| Risk | Severity | Description |
|------|----------|-------------|
| Shadow memory not utilized | HIGH | CRC checks will detect modifications |
| Libc hooks never installed | HIGH | Emulator detection bypass non-functional |
| Wrong file compiled | HIGH | Production code never builds |
| No ARM64 support | MEDIUM | Modern PUBG runs on 64-bit |
| Hardcoded vtable offsets | MEDIUM | Breaks every game update |
| No string obfuscation | LOW | All strings readable in binary |

---

## Summary (Khulaasa)

Ye bypass project PUBG Mobile/BGMI ke anti-cheat system ko bypass karta hai GameLoop emulator pe. Currently ye sirf ARM32 (32-bit) pe kaam karta hai. PUBG Mobile 4.4.0 ab ARM64 (64-bit) use karta hai, isliye poora bypass ARM64 ke liye migrate karna padega.

**Mukhya Samasyaein (Main Problems):**
1. Android.mk galat file compile karta hai — production code kabhi build nahi hoti
2. Shadow memory banaya jaata hai par use nahi hota — CRC evasion kaam nahi karta
3. Libc hooks define hain par install nahi hote — emulator detection bypass dead code hai
4. Sab offsets ARM32 ke hain — ARM64 ke liye naye offsets chahiye
5. Pointer types 32-bit hain — ARM64 pe crash hoga
6. Dobby ka ARM64 build nahi hai

**Kya Karna Hai:**
1. Pehle saare bugs fix karo (wrong file compile, missing hook installs)
2. Phir ARM64 migration karo (pointer types, struct offsets, vtable indexing)
3. Phir naye APK (4.4.0) ke libraries analyze karo IDA/Ghidra se
4. Naye offsets find karo aur update karo
5. Build system ARM64 ke liye configure karo
6. Windows loader ko bhi ARM64 paths ke liye update karo
