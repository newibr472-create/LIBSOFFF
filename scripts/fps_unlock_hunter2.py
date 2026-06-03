#!/usr/bin/env python3
"""
FPS Unlock Hook Hunter v2 - Fixed prologue detection
Scans libUE4.so for FPS cap functions via ADRP xref resolution.
Outputs PATCH_LIB hooks for 120fps unlock.
"""

import lief
import capstone
import struct
import time

BINARY = "/home/z/my-project/download/bg_extracted/lib/arm64-v8a/libUE4.so"

# Confirmed VA offsets from .rodata
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
    "LowDeviceFrameLimit":         0x03CBDFDE,
    "FixedFrameRate":              0x03DEA276,
    "GetExpandDeviceMaxFPSByDeviceLevel": 0x03CCB829,
    "GetExpandMaxFPSConfigByQuality": 0x03DFA97E,
}

def main():
    start = time.time()
    binary = lief.parse(BINARY)
    md = capstone.Cs(capstone.CS_ARCH_ARM64, capstone.CS_MODE_ARM)
    md.detail = True

    text_section = None
    for s in binary.sections:
        if s.name == ".text":
            text_section = s
            break
    if not text_section:
        print("[-] No .text!")
        return

    text_va = text_section.virtual_address
    text_size = len(text_section.content)
    text_bytes = bytes(text_section.content)
    
    print(f"[*] .text: VA=0x{text_va:X}, size=0x{text_size:X}")
    
    # Build target page map
    page_map = {}
    for name, va in TARGETS.items():
        page = va & ~0xFFF
        page_map.setdefault(page, []).append((name, va))

    # ADRP scan
    print(f"\n[*] Scanning for ADRP instructions...")
    matched = {}  # page -> [(off, rd)]
    
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

    print(f"    Found {sum(len(v) for v in matched.values())} ADRPs targeting our pages [{time.time()-start:.1f}s]")

    # Resolve xrefs
    print(f"\n[*] Resolving ADRP -> ADD/LDR cross-references...")
    results = {}

    for page, entries in matched.items():
        targets_on_page = {va: name for name, va in page_map[page]}
        
        for (off, rd) in entries:
            for j in range(1, 10):
                noff = off + j * 4
                if noff >= text_size - 3:
                    break
                nw = struct.unpack('<I', text_bytes[noff:noff+4])[0]
                nva = text_va + noff
                dec = list(md.disasm(text_bytes[noff:noff+4], nva))
                if not dec:
                    continue
                ni = dec[0]
                final = None
                cap_reg = capstone.arm64.ARM64_REG_X0 + rd

                if ni.mnemonic == "add" and len(ni.operands) >= 3:
                    o = ni.operands
                    if (o[0].type == capstone.arm64.ARM64_OP_REG and
                        o[1].type == capstone.arm64.ARM64_OP_REG and
                        o[2].type == capstone.arm64.ARM64_OP_IMM):
                        if o[0].reg == cap_reg and o[1].reg == cap_reg:
                            final = page + o[2].imm

                elif ni.mnemonic == "ldr" and len(ni.operands) >= 2:
                    o = ni.operands
                    if o[1].type == capstone.arm64.ARM64_OP_MEM:
                        if o[1].mem.base == cap_reg:
                            final = page + o[1].mem.disp

                if final and final in targets_on_page:
                    name = targets_on_page[final]
                    func_entry = find_func_entry(text_bytes, text_va, text_size, nva)
                    if func_entry:
                        eo = func_entry - text_va
                        orig = ' '.join(f'{b:02X}' for b in text_bytes[eo:eo+4])
                        results.setdefault(name, []).append({
                            'xref': nva,
                            'func': func_entry,
                            'orig': orig,
                            'insn': f"{ni.mnemonic} {ni.op_str}"
                        })
                    else:
                        # Still report even without func entry
                        orig = ' '.join(f'{b:02X}' for b in text_bytes[off:off+4])
                        results.setdefault(name, []).append({
                            'xref': nva,
                            'func': nva,  # use xref itself
                            'orig': orig,
                            'insn': f"{ni.mnemonic} {ni.op_str}",
                            'no_func': True
                        })
                    break

    # Output
    print(f"\n{'='*70}")
    print(f"[+] FPS UNLOCK HOOKS - PATCH_LIB FORMAT")
    print(f"{'='*70}")

    for name in sorted(results.keys()):
        entries = results[name]
        # Deduplicate by func entry
        seen = set()
        for e in entries:
            if e['func'] in seen:
                continue
            seen.add(e['func'])
            print(f"\n--- {name} ---")
            if e.get('no_func'):
                print(f"  [!] Could not find function entry, using xref offset")
            print(f"  Xref:     {e['insn']} @ 0x{e['xref']:X}")
            print(f"  Function: 0x{e['func']:X}")
            print(f"  Original: {e['orig']}")
            func_hex = e['func']
            print(f'  PATCH_LIB("libUE4.so", "0x{func_hex:X}", "1F 20 03 D5");  // NOP')

    print(f"\n[+] Scan complete in {time.time()-start:.1f}s")


def find_func_entry(text_bytes, text_va, text_size, ref_va):
    """Find function entry by scanning backwards for prologue patterns"""
    ref_off = ref_va - text_va
    
    for back in range(4, 4096, 4):
        off = ref_off - back
        if off < 0:
            break
        w = struct.unpack('<I', text_bytes[off:off+4])[0]
        va = text_va + off
        
        # Pattern 1: STP x29, x30, [sp, #imm]! (64-bit, signed offset, pre-index)
        # Encoding: x0101 001 xx1x xxxx xxxx xxxx xxxx xxxx
        # For pre-index: 11 (bit31:30=11), 101 (bit29:27=101), 001 (bit26:24=001)
        if (w & 0x7FC00000) == 0x29800000 and (w >> 31) & 1:
            # STP pre-index, check registers
            opc = (w >> 22) & 3
            if opc == 2:  # STP 64-bit
                rt = w & 0x1F
                rt2 = (w >> 10) & 0x1F
                rn = (w >> 5) & 0x1F
                if rn == 31 and rt in (29, 30) and rt2 in (29, 30) and rt != rt2:
                    return va

        # Pattern 2: STP x29, x30, [sp, #imm] (64-bit, signed offset, no writeback)
        if (w & 0x7FC00000) == 0x29000000 and not ((w >> 31) & 1) and not ((w >> 30) & 1):
            opc = (w >> 22) & 3
            if opc == 2:
                rt = w & 0x1F
                rt2 = (w >> 10) & 0x1F
                rn = (w >> 5) & 0x1F
                if rn == 31 and rt in (29, 30) and rt2 in (29, 30) and rt != rt2:
                    return va

        # Pattern 3: SUB sp, sp, #imm
        if (w & 0xFF0003FF) == 0xD10003FF:
            return va

        # Pattern 4: STP x29, x30, [sp, #-imm]!
        # Alternative encoding check
        if (w & 0xFFE003E0) == 0xA9BC7BFD:
            return va
        if (w & 0xFFE003E0) == 0xA9807BFD:
            return va
            
        # Pattern 5: PACIASP
        if w == 0xD503237F:
            return va
        
        # Pattern 6: STP with different register combinations that are typical prologues
        # STP d8, d9 style (save D registers at function start)
        # This is less common but check for x19/x20 saves too
        if (w & 0x7FC00000) == 0x29800000 and (w >> 31) & 1:
            opc = (w >> 22) & 3
            if opc == 2:
                rt = w & 0x1F
                rt2 = (w >> 10) & 0x1F
                rn = (w >> 5) & 0x1F
                if rn == 31:  # sp-based
                    # Check if preceded by something that looks like a function start
                    if back >= 8:
                        prev_w = struct.unpack('<I', text_bytes[off-off+4:off-off+8])[0] if False else 0
                    return va

    return None


if __name__ == "__main__":
    main()
