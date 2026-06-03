#!/usr/bin/env python3
"""
Find string " value" in libanogs.so and resolve all ADRP cross-references.
Output exact offsets for PATCH_LIB hook creation.
"""

import lief
import capstone
import struct
import sys

BINARY = "/home/z/my-project/download/bg_extracted/lib/arm64-v8a/libanogs.so"
TARGET_STRING = b" value"  # space + value

def main():
    print(f"[*] Loading: {BINARY}")
    binary = lief.parse(BINARY)
    
    md = capstone.Cs(capstone.CS_ARCH_ARM64, capstone.CS_MODE_ARM)
    md.detail = True
    
    # Step 1: Find target string offset in binary
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
            print(f"  [+] Found at VA: 0x{abs_off:X}  (section: {section.name})")
            # Show context
            ctx = content[max(0,idx-8):idx+len(TARGET_STRING)+32]
            print(f"      Context: {ctx!r}")
            pos = idx + 1
    
    if not target_offsets:
        print("[-] String not found!")
        return
    
    # Step 2: Get .text section
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
    print(f"\n[*] .text section: VA=0x{text_va:X}, size=0x{text_size:X}")
    
    # Step 3: Scan for ADRP instructions and resolve their targets
    # ADRP encoding: [31] op=1, [30:29] immlo, [28:24] 10000, [23:5] immhi, [4:0] Rd
    # Target = PC_page + (immhi:immlo << 12)
    
    print(f"\n[*] Scanning for ADRP -> target cross-references...")
    
    # Build a map: page_address -> list of (offset, instruction_bytes, decoded)
    adrp_map = {}
    
    for i in range(0, text_size - 3, 4):
        insn_bytes = text_bytes[i:i+4]
        if len(insn_bytes) < 4:
            break
        word = struct.unpack('<I', insn_bytes)[0]
        
        # Check ADRP: bit[31]=1, bits[28:24]=10000
        if (word & 0x9F000000) != 0x90000000:
            continue
        
        pc = text_va + i
        # Decode ADRP
        immhi = (word >> 5) & 0x7FFFF
        immlo = (word >> 29) & 0x3
        imm = (immhi << 2) | immlo
        if imm & (1 << 20):  # sign extend 21-bit
            imm -= (1 << 21)
        page_base = (pc & ~0xFFF) + (imm << 12)
        
        rd = word & 0x1F
        
        decoded = list(md.disasm(insn_bytes, pc))
        if decoded:
            adrp_map.setdefault(page_base, []).append((i, insn_bytes, decoded[0], rd))
    
    print(f"    Total ADRP instructions: {sum(len(v) for v in adrp_map.values())}")
    
    # Step 4: For each target string offset, find ADRPs that point to its page
    for target_va in target_offsets:
        target_page = target_va & ~0xFFF
        print(f"\n[*] Target: 0x{target_va:X}, page: 0x{target_page:X}")
        
        if target_page not in adrp_map:
            print(f"  [-] No ADRP references to page 0x{target_page:X}")
            continue
        
        adrp_entries = adrp_map[target_page]
        print(f"  [*] Found {len(adrp_entries)} ADRP instruction(s) pointing to page")
        
        # Step 5: For each ADRP, check next few instructions for ADD/LDR that completes the address
        for (off, raw, decoded, rd) in adrp_entries:
            adrp_va = text_va + off
            print(f"\n  ADRP @ 0x{adrp_va:X} (x{rd})")
            
            # Check next 6 instructions
            for j in range(1, 7):
                next_off = off + j * 4
                if next_off >= text_size - 3:
                    break
                next_bytes = text_bytes[next_off:next_off+4]
                next_word = struct.unpack('<I', next_bytes)[0]
                next_va = text_va + next_off
                next_decoded = list(md.disasm(next_bytes, next_va))
                
                if not next_decoded:
                    continue
                
                ni = next_decoded[0]
                mnem = ni.mnemonic
                
                # Check ADD xRd, xRd, #offset or LDR xRt, [xRd, #offset]
                if mnem == "add":
                    ops = ni.operands
                    if len(ops) >= 2:
                        if ops[0].type == capstone.arm64.ARM64_OP_REG and ops[1].type == capstone.arm64.ARM64_OP_REG:
                            if ops[1].reg == (capstone.arm64.ARM64_REG_X0 + rd) or (isinstance(ops[1].reg, int) and ops[1].reg == rd):
                                # This is ADD xRd, xRd, #imm -> complete address
                                if len(ops) >= 3 and ops[2].type == capstone.arm64.ARM64_OP_IMM:
                                    final_addr = target_page + ops[2].imm
                                    if final_addr == target_va:
                                        print(f"    MATCH! ADD @ 0x{next_va:X}: {ni.mnemonic} {ni.op_str}")
                                        print(f"    Final address: 0x{final_addr:X} == target 0x{target_va:X}")
                                        print(f"    >>> PATCH HERE: 0x{next_va:X}")
                                        
                elif mnem == "ldr":
                    ops = ni.operands
                    if len(ops) >= 2:
                        if ops[1].type == capstone.arm64.ARM64_OP_MEM:
                            mem = ops[1].mem
                            if mem.base == (capstone.arm64.ARM64_REG_X0 + rd):
                                final_addr = target_page + mem.disp
                                if final_addr == target_va:
                                    print(f"    MATCH! LDR @ 0x{next_va:X}: {ni.mnemonic} {ni.op_str}")
                                    print(f"    Final address: 0x{final_addr:X} == target 0x{target_va:X}")
                                    print(f"    >>> PATCH HERE: 0x{next_va:X}")
                                    
                elif mnem == "adr" or mnem == "adrp":
                    continue  # skip nested ADR/ADRP
                    
                else:
                    # Also check for SUB (negative offset from page)
                    if mnem == "sub":
                        ops = ni.operands
                        if len(ops) >= 3 and ops[2].type == capstone.arm64.ARM64_OP_IMM:
                            final_addr = target_page - ops[2].imm
                            if final_addr == target_va:
                                print(f"    MATCH! SUB @ 0x{next_va:X}: {ni.mnemonic} {ni.op_str}")
                                print(f"    >>> PATCH HERE: 0x{next_va:X}")
    
    # Step 6: Summary
    print(f"\n{'='*60}")
    print(f"[+] Done. Use matched offsets for PATCH_LIB")
    print(f"    Hex: C0 03 5F D6 (ARM64 RET)")

if __name__ == "__main__":
    main()
