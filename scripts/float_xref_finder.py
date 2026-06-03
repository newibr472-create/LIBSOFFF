#!/usr/bin/env python3
"""
Find ADRP xrefs to float constant 266.0 and other key floats
"""

import lief
import capstone
import struct
import time

BINARY = "/home/z/my-project/download/bg_extracted/lib/arm64-v8a/libUE4.so"

# Key float VA locations in .rodata
FLOAT_TARGETS = {
    "266.0 (FOV iPad)":  0x03480EB0,
    "120.0 (FPS)":       0x03485384,
    "60.0 (FPS default)": 0x0347FA58,
    "100.0 (FOV def)":   0x0347F854,
    "400.0 (MaxFOV)":    0x0347F404,
    "5.0 (HDR)":         0x0347F550,
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
    text_va = text_section.virtual_address
    text_size = len(text_section.content)
    text_bytes = bytes(text_section.content)

    # Build page map
    page_map = {}
    for name, va in FLOAT_TARGETS.items():
        page = va & ~0xFFF
        page_map.setdefault(page, []).append((name, va))

    # ADRP scan
    print(f"[*] Scanning {text_size/1024/1024:.0f}MB .text for ADRP xrefs to float pages...")
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

    total = sum(len(v) for v in matched.values())
    print(f"    Found {total} ADRPs in {time.time()-start:.1f}s")

    # Resolve
    print(f"\n[*] Resolving ADRP+LDR to float addresses...")
    results = {}
    
    for page, entries in matched.items():
        targets = {va: name for name, va in page_map[page]}
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

                if final and final in targets:
                    name = targets[final]
                    func_entry = find_func(text_bytes, text_va, text_size, nva)
                    results.setdefault(name, []).append({
                        'xref': nva, 'func': func_entry, 
                        'insn': f"{ni.mnemonic} {ni.op_str}",
                        'orig': ' '.join(f'{b:02X}' for b in text_bytes[noff:noff+4])
                    })
                    break

    # Output
    print(f"\n{'='*70}")
    print(f"[+] FLOAT CONSTANT XREFS - PATCH POINTS")
    print(f"{'='*70}")
    
    for name in sorted(results.keys()):
        entries = results[name]
        seen_funcs = set()
        for e in entries:
            if e['func'] and e['func'] in seen_funcs:
                continue
            if e['func']:
                seen_funcs.add(e['func'])
            print(f"\n  {name}")
            print(f"    xref: 0x{e['xref']:08X} ({e['insn']})")
            if e['func']:
                print(f"    func: 0x{e['func']:08X}")
            # Disassemble function start
            if e['func']:
                fo = e['func'] - text_va
                for k in range(min(20, (text_size - fo) // 4)):
                    fva = text_va + fo + k * 4
                    fw = text_bytes[fo+k*4:fo+k*4+4]
                    fh = ' '.join(f'{b:02X}' for b in fw)
                    dec = list(md.disasm(fw, fva))
                    if dec:
                        print(f"      0x{fva:08X}: {fh}  {dec[0].mnemonic} {dec[0].op_str}")
                        if dec[0].mnemonic == "ret":
                            break
    
    print(f"\n[+] Done in {time.time()-start:.1f}s")

def find_func(text_bytes, text_va, text_size, ref_va):
    off = ref_va - text_va
    for back in range(4, 4096, 4):
        bo = off - back
        if bo < 0: break
        w = struct.unpack('<I', text_bytes[bo:bo+4])[0]
        if (w & 0x7FC00000) == 0x29800000 and (w >> 31) & 1:
            opc = (w >> 22) & 3
            if opc == 2:
                rt, rt2, rn = w & 0x1F, (w >> 10) & 0x1F, (w >> 5) & 0x1F
                if rn == 31 and rt in (29, 30) and rt2 in (29, 30) and rt != rt2:
                    return text_va + bo
        if (w & 0xFF0003FF) == 0xD10003FF:
            return text_va + bo
        if w == 0xD503237F:
            return text_va + bo
    return None

if __name__ == "__main__":
    main()
