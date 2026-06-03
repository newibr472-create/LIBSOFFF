# LIBSOFFF

BGMI 4.4.0 (Battlegrounds Mobile India) Anti-Cheat Reverse Engineering

## Target
- **libanogs.so** (5.4MB) - Tencent AnoSDK Anti-Cheat
- **libUE4.so** (223MB) - Unreal Engine 4 Game Engine

## Hooks Created
| Hook | Offset | Type | What it Blocks |
|------|--------|------|----------------|
| PhantomTrace | 0x2BFCF0 | HOOK_LIB | Debugger + Emulator Detection |
| ScanTrigger | 0x369658 | HOOK_LIB | Memory Scan Trigger |
| MemoryBranch | 0x31DCB0 | HOOK_LIB | Memory Scanner Dispatcher |
| Wisdom Bypass | 20 offsets | Verified | Multiple AC Functions |

## Discovery
- 44 string cross-references found
- 43 function regions identified  
- 15+ hookable candidates scored and ranked
- Tool: Python + LIEF + Capstone (no Ghidra needed)

## Files
- `hooks/` - Xynnacore header files
- `scripts/` - Analysis scripts
- `reports/` - Analysis reports
- `analysis/` - String dumps
- `prompt.txt` - Full progress report

## Setup
Open in GitHub Codespace with `.devcontainer/devcontainer.json` (Java 21 + Python 3.13)
