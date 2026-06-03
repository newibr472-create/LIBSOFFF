# Complete Offset Reference — All Hooks & Internal Offsets

## ARM32 Offsets (Current — armeabi-v7a)

### libanogs.so
```
0x3766E0 — hsub_3766E0 (Device info reporter — returns 0)
0xF1E3E  — hAnoSDKGetReportData3 (Report data collection — returns 0)
```

### libUE4.so
```
0x427AD74 — ue4_emulator_detect_stub (Emulator detect — returns 0)
0x4CD1AD8 — hsub_4CD1AD8 (AntiCheat Struct Init — returns null vtable)
0x6D064B0 — hsub_6D064B0 (AntiCheat Obfuscated — returns 0)
0x6B505C8 — hsub_6B505C8 (AntiCheat Data Parse — zeroes bytes 12-17)
0x6B500F8 — hsub_6B500F8 (AntiCheat Data Read — zeroes 7 struct fields)
0x6B3DED0 — hsub_6B3DED0 (Core AntiCheat Thread — kills thread)
0x4CD33B4 — hsub_4CD33B4 (AntiCheat Full Init — pass-through)
0x79DFEEC — hsub_79DFEEC (PhysX ContactReport vtable rewrite)
```

### Internal offsets inside hsub_79DFEEC (libUE4.so)
```
0x79DFC80 — Direct function call (vtable initializer)
0x79DEE34 — Vtable entry
0x79DEE28 — Vtable entry
0x79DEE4C — Vtable entry
0x79DEE40 — Vtable entry
0x79DEE58 — Vtable entry
0x79DEE64 — Vtable entry
0x79DEE70 — Vtable entry
0x79DEE7C — Vtable entry
0x79DEE88 — Vtable entry
0x79DEE94 — Vtable entry
0x79DEEA0 — Vtable entry
0x79DEEC8 — Vtable entry
0x79DEEE4 — Vtable entry
0x79DEEF0 — Vtable entry
0x79DEF08 — Vtable entry
0x79DEF14 — Vtable entry
0x79DEF20 — Vtable entry
0x79DEBA8 — Vtable entry
0x238FF3C — Vtable entry
0x238F5A0 — Vtable entry
0x238EB7C — Vtable entry
0x7867A24 — Vtable entry
```

### libTDataMaster.so
```
0x6E778  — hsub_6E778 (SIGPIPE handler bypass — forces flags on)
0x6CB8C  — hsub_6CB8C (Report packet terminator — kills outbound data)
0x1B4CC  — hTDM_GetInstance (TDM instance block — returns 0)
0x248E0  — hsub_248E0 (AC config init — spoofed thresholds)
```

### libgcloud.so
```
0x1B4D8  — hPluginReport_D2 (PluginReport D2 destructor — empty body)
0x1B4E4  — hPluginReport_D0 (PluginReport D0 destructor — empty body)
0x1B514  — hOnPostStartup (OnPostStartup — returns 0)
0x1B520  — hOnPreShutdown (OnPreShutdown — returns 0)
0x1B52C  — hOnShutdown (OnShutdown — returns 0)
```

### Windows-side (AndroidEmulatorEx.exe)
```
0x427AD74 — UE4 emulator detection NOP patch (2 bytes: 0x90 0x90)
```

## ARM64 Offsets (PUBG 4.4.0 — TO BE DETERMINED)

All offsets above need to be re-resolved from ARM64 IDA/Ghidra analysis of:
- lib/arm64-v8a/libanogs.so
- lib/arm64-v8a/libUE4.so
- lib/arm64-v8a/libTDataMaster.so
- lib/arm64-v8a/libgcloud.so

## Struct Offsets That Change on ARM64

### tdm_hooks.h — hsub_6E778
```
*(result + 1252) = 1  — ARM32 struct offset, needs ARM64 recalculation
*(a2 + 16) = 1        — May stay same or change based on struct alignment
```

### tdm_hooks.h — hsub_248E0
```
*(result + 12)  = 0     — AC config field
*(result + 44)  = 999   — AC config field
*(result + 60)  = 9999  — AC config field
*(result + 64)  = 999   — AC config field
*(result + 76)  = 99999 — AC config field
All these offsets change on ARM64 due to pointer size differences
```

### ue4_hooks.h — hsub_79DFEEC
```
a1[84] through a1[127] — _DWORD* array indexing (4 bytes per entry)
On ARM64: must use uint64_t* with indices halved (a1[42] through a1[63])
```

### ue4_hooks.h — hsub_6B500F8
```
a2[0]=0, a2[1]=0, a2[2]=0, a2[3]=0, a2[4]=0, a2[5]=0, a2[6]=0
Using int* — on ARM64 struct fields may have different offsets
```
