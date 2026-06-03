#!/usr/bin/env python3
"""Debug: check what ADRP resolves to and what follows"""
import lief
import capstone
import struct
import time

BINARY = "/home/z/my-project/download/bg_extracted/lib/arm64-v8a/libUE4.so"
TARGETS = {
    "SetMaxFpsFromDevice":         0x03E3F880,
    "SetFrameRateLimit":           0x03D6AD21,
    "GetFrameRateLimit":           0x03C6B11F,
    "CurrentMaxFPS":               0x03DB6FDB,
    "TargetFrameRate":             0x03D5BB9D,
    "bLowDeviceFrameRateLimitEnabled": 0x03DE8C3F,
    "bSmoothFrameRate":             0x03C559D1,
    "FrameRateLock":                0x03CEEA51,
    "GMIOSFrameRateLock":           0x03D6CBD3,
}

binary = lief.parse(BINARY)
md = capstone.Cs(capstone.CS_ARCH_ARM64, capstone.CS_MODE_ARM)
md.detail = True

text_section = None
for s in binary.sections:
    if s.name == ".text":
        text_section = s
text_va = text_section.virtual_address
text_size = len(text_section.content)
text_bytes = bytes(text_section.content)

page_map = {}
for name, va in TARGETS.items():
    page_map.setdefault(va & ~0xFFF, []).append((name, va))

# Scan for ADRPs
matched = {}
for i in range(0, text_size - 3, 4):
    w = struct.unpack('<I', text_bytes[i:i+4])[0]
    if (w & 0x9F000000) != 0x90000000:
        continue
    pc = text_va + i
    immhi = (w >> 5) & 0x7FFFF
    immlo = (w >> 29) & 0x3
    imm = (immhi << 2) | immlo
    if imm & (1 << 20):
        imm -= 1 << 21
    page = (pc & ~0xFFF) + (imm << 12)
    rd = w & 0x1F
    if page in page_map:
        matched.setdefault(page, []).append((i, rd))

# Debug: print first 3 ADRPs for each target and show following instructions
for page, entries in list(matched.items())[:3]:
    for name, va in page_map[page]:
        print(f"\n--- {name} @ 0x{va:X} (page 0x{page:X}) ---")
        for (off, rd) in entries[:5]:
            adrp_va = text_va + off
            print(f"\n  ADRP @ 0x{adrp_va:X} -> page 0x{page:X} (x{rd})")
            # Print next 8 instructions
            for j in range(0, 9):
                noff = off + j * 4
                if noff >= text_size - 3:
                    break
                nva = text_va + noff
                nw = struct.unpack('<I', text_bytes[noff:noff+4])[0]
                nh = ' '.join(f'{b:02X}' for b in text_bytes[noff:noff+4])
                dec = list(md.disasm(text_bytes[noff:noff+4], nva))
                if dec:
                    ni = dec[0]
                    # Try to compute address from ADD/LDR
                    extra = ""
                    if ni.mnemonic == "add" and len(ni.operands) >= 3:
                        o = ni.operands
                        if o[2].type == capstone.arm64.ARM64_OP_IMM:
                            computed = page + o[2].imm
                            extra = f"  => 0x{computed:X}"
                    elif ni.mnemonic == "ldr" and len(ni.operands) >= 2:
                        o = ni.operands
                        if o[1].type == capstone.arm64.ARM64_OP_MEM and o[1].mem.disp != 0:
                            computed = page + o[1].mem.disp
                            extra = f"  => 0x{computed:X}"
                    print(f"    0x{nva:08X}: {nh}  {ni.mnemonic} {ni.op_str}{extra}")
                else:
                    print(f"    0x{nva:08X}: {nh}  ???")
