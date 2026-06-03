#!/usr/bin/env python3
"""
Find all BL nanosleep calls in libUE4.so .text section
These are the FPS frame timing / cap enforcement points.
Also find BL clock_nanosleep if used.
"""

import lief
import capstone
import struct
import time

BINARY = "/home/z/my-project/download/bg_extracted/lib/arm64-v8a/libUE4.so"
NANOSLEEP_PLT = 0xC4E2170  # nanosleep PLT stub
# Also check for BL to nearby PLT entries (sometimes inlined)
# BL encoding: 1001 01xx xxxx xxxx xxxx xxxx xxxx
# Target = PC + imm*4 (26-bit signed)

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
    
    print(f"[*] .text: VA=0x{text_va:X}, size=0x{text_size:X}")
    print(f"[*] nanosleep PLT: 0x{NANOSLEEP_PLT:X}")
    
    # Find all BL instructions targeting nanosleep PLT
    results = []
    
    # Calculate relative offset range
    # BL offset = (target - pc) / 4, 26-bit signed
    # Range: ±128MB from any instruction in .text
    
    for i in range(0, text_size - 3, 4):
        w = struct.unpack('<I', text_bytes[i:i+4])[0]
        # Check BL encoding: bits [31:26] = 100101
        if (w >> 26) != 0b100101:
            continue
        
        pc = text_va + i
        imm26 = w & 0x3FFFFFF
        if imm26 & (1 << 25):
            imm26 -= (1 << 26)
        target = pc + imm26 * 4
        
        if target == NANOSLEEP_PLT:
            results.append(i)
    
    print(f"\n[+] Found {len(results)} BL nanosleep calls in {time.time()-start:.1f}s")
    
    # For each call site, find function entry and show context
    print(f"\n{'='*70}")
    print(f"[+] BL NANOSLEEP CALL SITES")
    print(f"{'='*70}")
    
    for idx, off in enumerate(results):
        va = text_va + off
        
        # Find function entry (scan backwards for STP/SUB sp prologue)
        func_entry = None
        for back in range(4, 4096, 4):
            boff = off - back
            if boff < 0:
                break
            bw = struct.unpack('<I', text_bytes[boff:boff+4])[0]
            bva = text_va + boff
            
            # STP x29, x30, [sp, #imm]!
            if (bw & 0x7FC00000) == 0x29800000 and (bw >> 31) & 1:
                opc = (bw >> 22) & 3
                rt = bw & 0x1F
                rt2 = (bw >> 10) & 0x1F
                rn = (bw >> 5) & 0x1F
                if rn == 31 and rt in (29, 30) and rt2 in (29, 30) and rt != rt2:
                    func_entry = bva
                    break
            # SUB sp, sp, #imm
            if (bw & 0xFF0003FF) == 0xD10003FF:
                func_entry = bva
                break
            # PACIASP
            if bw == 0xD503237F:
                func_entry = bva
                break
        
        # Get original hex at func entry and at BL site
        entry_orig = ' '.join(f'{b:02X}' for b in text_bytes[off:off+4]) if func_entry is None else \
                     ' '.join(f'{b:02X}' for b in text_bytes[func_entry-text_va:func_entry-text_va+4])
        bl_orig = ' '.join(f'{b:02X}' for b in text_bytes[off:off+4])
        
        # Disassemble context: 5 insns before + BL + 5 after
        print(f"\n--- Call #{idx+1} @ 0x{va:X} ---")
        if func_entry:
            print(f"  Function entry: 0x{func_entry:X}")
        print(f"  BL bytes: {bl_orig}")
        
        # Show disassembly around the call
        ctx_start = max(0, off - 20*4)  # 20 insns before
        ctx_end = min(text_size, off + 12*4)  # 12 insns after
        print(f"  Context (disassembly):")
        for j in range(ctx_start, ctx_end, 4):
            jva = text_va + j
            jw = text_bytes[j:j+4]
            jh = ' '.join(f'{b:02X}' for b in jw)
            dec = list(md.disasm(jw, jva))
            if dec:
                ins = dec[0]
                marker = " <<<" if j == off else ""
                print(f"    0x{jva:08X}: {jh}  {ins.mnemonic} {ins.op_str}{marker}")
        
        print(f"\n  PATCH at BL site:")
        print(f'    PATCH_LIB("libUE4.so", "0x{va:X}", "1F 20 03 D5");  // NOP the BL nanosleep')
        if func_entry:
            print(f"    PATCH_LIB(\"libUE4.so\", \"0x{func_entry:X}\", \"C0 03 5F D6\");  // RET entire function")
    
    print(f"\n[+] Done. {len(results)} nanosleep call sites found in {time.time()-start:.1f}s")

if __name__ == "__main__":
    main()
