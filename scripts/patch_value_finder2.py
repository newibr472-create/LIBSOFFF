#!/usr/bin/env python3
"""
Find string " value" in libanogs.so → resolve ADRP xref → find function entry → create PATCH_LIB.
Fixed register matching using Capstone enum properly.
"""

import lief
import capstone
import struct

BINARY = "/home/z/my-project/download/bg_extracted/lib/arm64-v8a/libanogs.so"
TARGET_STRING = b" value"

# ARM64 register constants (Capstone enum values)
XREGS = {capstone.arm64.ARM64_REG_X0 + i: i for i in range(32)}

def decode_adrp(word, pc):
    """Decode ADRP instruction, return (target_page, rd_reg_num) or None"""
    if (word & 0x9F000000) != 0x90000000:
        return None
    immhi = (word >> 5) & 0x7FFFF
    immlo = (word >> 29) & 0x3
    imm = (immhi << 2) | immlo
    if imm & (1 << 20):
        imm -= (1 << 21)
    page = (pc & ~0xFFF) + (imm << 12)
    rd = word & 0x1F
    return (page, rd)

def find_function_entry(text_bytes, text_va, text_size, ref_va):
    """Scan backwards from ref_va to find function prologue (STP x29,x30 or SUB sp pattern)"""
    ref_off = ref_va - text_va
    
    # Scan backwards up to 4KB for function prologue
    for back in range(4, 1024, 4):
        off = ref_off - back
        if off < 0:
            break
        insn_word = struct.unpack('<I', text_bytes[off:off+4])[0]
        insn_va = text_va + off
        
        # Check for STP x29, x30, [sp, #imm]!  (typical function prologue)
        # Encoding: x1101 0001 0000 0000 0100 0000 0101 1111
        # with stp pre-index: 1x101 001 ...
        # STP Xt1, Xt2, [Xn, #imm]!  -> 1011 1001 0xxx xxxx xxxx xxxx xxxx xxxx
        if (insn_word & 0xFFC07FFF) == 0xA9807BFD:  # STP x29, x30, [sp, #imm]!
            return insn_va
        
        # STP x29, x30, [sp, #-imm]! (negative offset)
        if (insn_word & 0xFFC07FFF) == 0xA9007BFD:  # STP x29, x30, [sp, #imm]! (some encodings)
            return insn_va
            
        # SUB sp, sp, #imm (alternative prologue)
        if (insn_word & 0xFF0003FF) == 0xD10003FF:
            return insn_va
    
    return None

def main():
    print(f"[*] Loading: {BINARY}")
    binary = lief.parse(BINARY)
    md = capstone.Cs(capstone.CS_ARCH_ARM64, capstone.CS_MODE_ARM)
    md.detail = True

    # Step 1: Find target string
    print(f"\n[*] Searching for string: {TARGET_STRING!r}")
    target_offsets = []
    for section in binary.sections:
        content = bytes(section.content)
        pos = 0
        while True:
            idx = content.find(TARGET_STRING, pos)
            if idx == -1:
                break
            abs_off = section.virtual_address + idx
            target_offsets.append(abs_off)
            ctx = content[max(0,idx-16):idx+len(TARGET_STRING)+48]
            # Find null terminator
            null_idx = ctx.find(b'\x00', len(TARGET_STRING))
            if null_idx != -1:
                full_str = ctx[:null_idx]
            else:
                full_str = ctx[:32]
            print(f"  [+] VA: 0x{abs_off:X}  full: {full_str!r}")
            pos = idx + 1

    if not target_offsets:
        print("[-] String not found!")
        return

    # Step 2: Get .text
    text_section = None
    for s in binary.sections:
        if s.name == ".text":
            text_section = s
            break
    if not text_section:
        print("[-] No .text section!")
        return

    text_bytes = bytes(text_section.content)
    text_va = text_section.virtual_address
    text_size = len(text_bytes)
    print(f"\n[*] .text: VA=0x{text_va:X}, size=0x{text_size:X}")

    # Step 3: Build ADRP map
    print(f"\n[*] Building ADRP cross-reference map...")
    adrp_map = {}  # page -> [(offset, insn_word, rd)]
    
    for i in range(0, text_size - 3, 4):
        word = struct.unpack('<I', text_bytes[i:i+4])[0]
        result = decode_adrp(word, text_va + i)
        if result:
            page, rd = result
            adrp_map.setdefault(page, []).append((i, word, rd))

    print(f"    Total ADRPs: {sum(len(v) for v in adrp_map.values())}")

    # Step 4: For each target string, find matching xrefs
    results = []
    
    for target_va in target_offsets:
        target_page = target_va & ~0xFFF
        print(f"\n[*] Target VA: 0x{target_va:X}  page: 0x{target_page:X}")

        if target_page not in adrp_map:
            print(f"  [-] No ADRP for page")
            continue

        for (off, raw_word, rd) in adrp_map[target_page]:
            adrp_va = text_va + off
            
            # Check next 8 instructions for ADD/LDR completing the address
            for j in range(1, 9):
                noff = off + j * 4
                if noff >= text_size - 3:
                    break
                nword = struct.unpack('<I', text_bytes[noff:noff+4])[0]
                nva = text_va + noff
                ndec = list(md.disasm(text_bytes[noff:noff+4], nva))
                if not ndec:
                    continue
                ni = ndec[0]
                
                final_addr = None
                
                # Check ADD xN, xN, #imm where N matches ADRP's rd
                if ni.mnemonic == "add" and len(ni.operands) >= 3:
                    ops = ni.operands
                    if (ops[0].type == capstone.arm64.ARM64_OP_REG and 
                        ops[1].type == capstone.arm64.ARM64_OP_REG and
                        ops[2].type == capstone.arm64.ARM64_OP_IMM):
                        reg0 = ops[0].reg
                        reg1 = ops[1].reg
                        # Both regs must be x{rd}
                        cap_reg = capstone.arm64.ARM64_REG_X0 + rd
                        if reg0 == cap_reg and reg1 == cap_reg:
                            final_addr = target_page + ops[2].imm
                
                # Check LDR with [xN, #disp]
                elif ni.mnemonic == "ldr" and len(ni.operands) >= 2:
                    ops = ni.operands
                    if ops[1].type == capstone.arm64.ARM64_OP_MEM:
                        mem = ops[1].mem
                        cap_reg = capstone.arm64.ARM64_REG_X0 + rd
                        if mem.base == cap_reg:
                            final_addr = target_page + mem.disp

                if final_addr and final_addr == target_va:
                    print(f"  [+] MATCH at 0x{nva:X}: {ni.mnemonic} {ni.op_str}")
                    print(f"      ADRP @ 0x{adrp_va:X} -> x{rd} + offset -> 0x{final_addr:X}")
                    
                    # Find function entry
                    func_entry = find_function_entry(text_bytes, text_va, text_size, nva)
                    if func_entry:
                        # Get original bytes at function entry
                        entry_off = func_entry - text_va
                        orig_hex = text_bytes[entry_off:entry_off+4].hex().upper()
                        orig_hex_spaced = ' '.join(orig_hex[i:i+2] for i in range(0, len(orig_hex), 2))
                        
                        print(f"      Function entry: 0x{func_entry:X}")
                        print(f"      Original bytes: {orig_hex_spaced}")
                        print(f"      Patch (RET):   C0 03 5F D6")
                        results.append({
                            'string_va': target_va,
                            'xref_va': nva,
                            'xref_insn': f"{ni.mnemonic} {ni.op_str}",
                            'func_entry': func_entry,
                            'orig_hex': orig_hex_spaced,
                        })
                    break

    # Step 5: Output PATCH_LIB format
    print(f"\n{'='*70}")
    print(f"[+] RESULTS - PATCH_LIB format:")
    print(f"{'='*70}")
    
    for i, r in enumerate(results):
        print(f"\n--- Result #{i+1} ---")
        print(f"String:    \" value\" @ 0x{r['string_va']:X}")
        print(f"Xref:      {r['xref_insn']} @ 0x{r['xref_va']:X}")
        print(f"Func:      0x{r['func_entry']:X}")
        print(f"Original:  {r['orig_hex']} @ 0x{r['func_entry']:X}")
        print(f"Patched:   C0 03 5F D6 (RET) @ 0x{r['func_entry']:X}")
        print(f"\nPATCH_LIB(\"libanogs.so\", \" value\", \"C0 03 5F D6\");")
        
        # Also show the full function disassembly (first 20 instructions)
        print(f"\n[*] Function disassembly (first 20 insns from 0x{r['func_entry']:X}):")
        entry_off = r['func_entry'] - text_va
        count = 0
        pos = entry_off
        while pos < text_size - 3 and count < 20:
            dec = list(md.disasm(text_bytes[pos:pos+4], text_va + pos))
            if dec:
                insn = dec[0]
                raw_hex = text_bytes[pos:pos+4].hex().upper()
                raw_hex_spaced = ' '.join(raw_hex[i:i+2] for i in range(0, len(raw_hex), 2))
                print(f"    0x{text_va+pos:08X}: {raw_hex_spaced}  {insn.mnemonic} {insn.op_str}")
                # Stop at RET or B
                if insn.mnemonic == "ret" or insn.mnemonic == "b":
                    break
            else:
                break
            pos += 4
            count += 1

    # Also dump raw hex around the function entry for verification
    if results:
        r = results[0]
        entry_off = r['func_entry'] - text_va
        print(f"\n[*] Raw bytes at function entry 0x{r['func_entry']:X}:")
        chunk = text_bytes[entry_off:entry_off+32]
        print(f"    {' '.join(f'{b:02X}' for b in chunk)}")

if __name__ == "__main__":
    main()
