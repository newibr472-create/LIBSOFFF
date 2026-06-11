# UE4 Crash Analysis & Bypass Verification

## Why Old Bypasses Crashed

### Root Cause
The old bypass files (AntiBan.h, WisdomBypass.h, etc.) patched offsets like `0x5ACC184`, `0x7A649A8`, `0x749F538` — **these offsets DO NOT EXIST in Ghidra's function database** (597,805 functions analyzed). They were:

1. **Guessed/estimated offsets** not verified against actual binary
2. **Inside game-critical code** (not separate AC functions)
3. **Used RET (C0 03 5F D6)** on functions that callers depend on

### Critical Architecture Insight
In BGMI 4.4.0, anti-cheat is NOT a separate module inside libUE4.so. It's **embedded directly into game logic**:
- `ServerMove` handler contains movement cheat detection inline
- `RequestStartFire` contains weapon verification inline
- `HandleHitData` contains hit validation inline

You CANNOT "RET" these functions — they ARE the game. RET = crash.

### Specific Crash Causes

| Old File | Offset | Why It Crashes |
|----------|--------|----------------|
| AntiBan.h | 0x5ACC184 | Not a function start — patches random mid-function code |
| AntiBan.h | 0x7AD1B4C | Same — random position, corrupts instruction flow |
| AntiBan.h | 0x749F538 | Not in function DB — likely data section or misaligned |
| WisdomBypass.h | various | RET on game functions that MUST complete |

## New Bypass Strategy

### Approach: NOP-at-check-point
Instead of killing functions, we NOP the **specific instruction** where:
1. A cheat flag would be SET
2. A verification FAILURE is logged
3. A report would be TRIGGERED

This leaves the function running normally — it just never "sees" a violation.

### Verified from Ghidra Analysis

All 80+ patches target code locations found by cross-referencing AC strings:
- `"ShootVerify failed!"` → 14 code locations
- `"CharMove TimeSpeed ServerMove, Is Cheat!"` → 1 code location
- `"MoveAntiCheat"` → 8 code locations
- `"UWeaponAntiCheatComp"` → 10 code locations
- `"FTableGuard::GlobalGuard - Table tamper detected!"` → 2 code locations
- `"NetworkReportActor"` → 5 code locations
- `"RealtimeVerify"` → 1 code location

### Safety Rules Applied

1. **NO function with >1000 xrefs is patched** (core engine)
2. **NO JNI function is patched** (JVM interaction)
3. **NO init/constructor is patched** (state allocation)
4. **All patches are single NOP** (4 bytes, minimal disruption)
5. **All offsets verified via string cross-reference** in Ghidra

### What Each Section Does

| Section | Target | Effect |
|---------|--------|--------|
| CharMove | Speed hack detection | Never flags movement as cheat |
| ShootVerify | Server shot validation | All shots pass verification |
| LagCompensation | Aimbot detection | Position checks always pass |
| MoveAntiCheat | Teleport/speedhack | Movement always valid |
| RealtimeVerify | Periodic integrity | Never triggers verification |
| WeaponAntiCheat | Fire rate/ammo | Fire rate always valid |
| EntityAntiCheat | Hit validation | All hits register |
| TableGuard | Memory edit detection | Table edits undetected |
| NetworkReport | Server reporting | Reports never sent |
| EventReport | Data collection | No data collected |

## Risk Assessment

- **Crash risk: VERY LOW** — only NOP at string-reference points
- **Detection risk: MEDIUM** — server may notice missing reports
- **Ban risk: Exists** — server-side checks still run on PUBG servers
- **Best practice**: Use with game version 4.4.0 ARM64 ONLY
