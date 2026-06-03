#!/usr/bin/env python3
"""
120FPS Unlock - TARGETED scan for FPS limit functions in libUE4.so
Focus on: GetFrameRateLimit, SetFrameRateLimit, SetMaxFpsFromDevice, 
         bUseFpsLock, CurrentMaxFPS, GetExpandDeviceMaxFPSByDeviceLevel
"""
import struct, lief
from collections import defaultdict
from capstone import *
from capstone.arm64 import *

BINARY = "/home/z/my-project/download/bg_extracted/lib/arm64-v8a/libUE4.so"
OUTPUT = "/home/z/my-project/download/fps_targeted_report.txt"

def main():
    print("[*] Loading libUE4.so...")
    binary = lief.parse(BINARY)
    rodata = binary.get_section(".rodata")
    text = binary.get_section(".text")
    rodata_bytes = bytes(rodata.content)
    rodata_base = rodata.virtual_address
    text_bytes = bytes(text.content)
    text_base = text.virtual_address
    text_size = len(text_bytes)
    
    # TARGET STRINGS (the most important FPS functions)
    targets = {
        "SetFrameRateLimit":   0x3D6AD21,
        "GetFrameRateLimit":   0x3C6B11F,
        "SetMaxFpsFromDevice": 0x3E3F880,
        "bUseFpsLock":         0x3E4FA02,
        "CurrentMaxFPS":       0x3DB6FDB,
        "GetExpandDeviceMaxFPSByDeviceLevel": 0x3CCB829,
        "GetExpandMaxFPSConfigByQuality":      0x3DFA97E,
        "bLowDeviceFrameRateLimitEnabled":     0x3DE8C3F,
        "bUseFixedFrameRate": 0x3E7C5A2,
        "FixedFrameRate":     0x3DEA276,
        "TargetFrameRate":    0x3D5BB9D,
        "targetFPS":          0x3D4BFF8,
    }
    
    # Build page map
    string_pages = {}
    for name, addr in targets.items():
        page = addr & ~0xFFF
        string_pages.setdefault(page, {})[addr & 0xFFF] = (addr, name)
    
    print(f"  Target strings: {len(targets)}")
    print(f"  Target pages: {len(string_pages)}")
    
    # Scan .text for ADRP targeting these pages
    md = Cs(CS_ARCH_ARM64, CS_MODE_ARM)
    md.detail = True
    
    print("\n[1] Scanning .text for ADRP targeting FPS strings...")
    
    # Raw scan (fast) - only scan sections of .text that have ADRP
    # ADRP can only reach +/- 4GB from PC, so all of .text can target all of .rodata
    fps_xrefs = []
    
    # Sample scan: every 100th offset to speed up, then refine
    # Actually for 223MB we need a smarter approach
    # Let's scan in chunks
    
    chunk_size = 100_000_000  # 100MB chunks
    total_scanned = 0
    
    for chunk_start in range(0, text_size, chunk_size):
        chunk_end = min(text_size, chunk_start + chunk_size)
        print(f"  Scanning 0x{text_base + chunk_start:X} - 0x{text_base + chunk_end:X} ({chunk_end - chunk_start} bytes)...")
        
        for offset in range(chunk_start, chunk_end, 4):
            insn_word = struct.unpack("<I", text_bytes[offset:offset+4])[0]
            if (insn_word & 0x9F000000) != 0x90000000:
                continue
            immlo = (insn_word >> 29) & 0x3
            immhi = (insn_word >> 5) & 0x7FFFF
            imm = (immhi << 2) | immlo
            if imm & (1 << 20): imm -= (1 << 21)
            pc = text_base + offset
            page = (pc & ~0xFFF) + (imm << 12)
            
            if page not in string_pages:
                continue
            
            # Decode ADRP + next instructions
            code_end = min(text_size, offset + 24)
            code = text_bytes[offset:code_end]
            insns = list(md.disasm(code, pc))
            if not insns or insns[0].mnemonic != "adrp":
                continue
            
            adrp_reg = insns[0].operands[0].reg if insns[0].operands else None
            adrp_page_val = insns[0].operands[1].imm if len(insns[0].operands) > 1 else 0
            
            for inst in insns[1:6]:
                if inst.mnemonic == "add" and len(inst.operands) >= 3:
                    op1, op2 = inst.operands[1], inst.operands[2]
                    if op1.type == ARM64_OP_REG and op2.type == ARM64_OP_IMM and op1.reg == adrp_reg:
                        full = adrp_page_val + op2.imm
                        off_in_page = full & 0xFFF
                        if off_in_page in string_pages[page]:
                            str_addr, str_name = string_pages[page][off_in_page]
                            fps_xrefs.append({
                                "ref_addr": inst.address,
                                "str_addr": str_addr,
                                "str_name": str_name,
                                "string": str_name,
                                "type": "ADR+ADD"
                            })
                        break
                elif inst.mnemonic == "ldr" and len(inst.operands) >= 2:
                    op1 = inst.operands[1]
                    if op1.type == ARM64_OP_MEM and op1.mem.base == adrp_reg:
                        full = adrp_page_val + (op1.mem.disp if op1.mem.disp else 0)
                        off_in_page = full & 0xFFF
                        if off_in_page in string_pages[page]:
                            str_addr, str_name = string_pages[page][off_in_page]
                            fps_xrefs.append({
                                "ref_addr": inst.address,
                                "str_addr": str_addr,
                                "str_name": str_name,
                                "string": str_name,
                                "type": "ADR+LDR"
                            })
                        break
    
    print(f"\n  Total FPS xrefs found: {len(fps_xrefs)}")
    for xr in sorted(fps_xrefs, key=lambda x: x["ref_addr"]):
        print(f"    @0x{xr['ref_addr']:X} [{xr['type']}] -> 0x{xr['str_addr']:X} \"{xr['str_name']}\"")
    
    # Group into functions
    print("\n[2] Grouping into functions...")
    fps_functions = defaultdict(lambda: {"refs": [], "addr": 0, "size": 0, "offset": 0, "instructions": []})
    
    for xr in fps_xrefs:
        ref_off = xr["ref_addr"] - text_base
        func_start = ref_off
        for back in range(ref_off, max(0, ref_off - 4096), -4):
            word = struct.unpack("<I", text_bytes[back:back+4])[0]
            if (word & 0x7FC07FFF) == 0xA9007BFD:
                func_start = back; break
            elif (word & 0x7FC03FFF) == 0xA9807BFD:
                func_start = back; break
            elif (word & 0xFF0003FF) == 0xD10003FF:
                func_start = back; break
        
        func_addr = text_base + func_start
        fm = fps_functions[func_addr]
        fm["addr"] = func_addr
        fm["offset"] = func_start
        fm["refs"].append(xr)
    
    # Sizes
    sorted_addrs = sorted(fps_functions.keys())
    for i, addr in enumerate(sorted_addrs):
        if i + 1 < len(sorted_addrs):
            fps_functions[addr]["size"] = min(sorted_addrs[i+1] - addr, 16384)
        else:
            fps_functions[addr]["size"] = 4096
    
    print(f"  FPS functions: {len(fps_functions)}")
    
    # Disassemble each
    print("\n[3] Disassembling...")
    for faddr in sorted_addrs:
        fm = fps_functions[faddr]
        size = min(fm["size"], 4096)
        code = text_bytes[fm["offset"]:fm["offset"]+size]
        fm["instructions"] = list(md.disasm(code, faddr))
    
    # Score
    scored = []
    priority_names = {"GetFrameRateLimit", "SetFrameRateLimit", "SetMaxFpsFromDevice", 
                      "bUseFpsLock", "CurrentMaxFPS", "GetExpandDeviceMaxFPSByDeviceLevel"}
    
    for faddr in sorted_addrs:
        fm = fps_functions[faddr]
        score = 0
        names = set(r["str_name"] for r in fm["refs"])
        
        for r in fm["refs"]:
            if r["str_name"] in priority_names:
                score += 25
            else:
                score += 5
        
        score += min(len(fm["refs"]), 5) * 3
        size = fm["size"]
        if 20 <= size <= 500: score += 15
        elif 500 < size <= 2000: score += 10
        
        scored.append({
            "addr": faddr, "offset": fm["offset"], "size": size,
            "score": score, "refs": fm["refs"],
            "instructions": fm["instructions"],
            "num_insn": len(fm["instructions"]),
            "names": names,
        })
    
    scored.sort(key=lambda x: -x["score"])
    
    print(f"\n  RANKED RESULTS:")
    for i, sf in enumerate(scored):
        print(f"  #{i+1:2d} 0x{sf['addr']:X} score={sf['score']:3d} size={sf['size']:5d}B | {sorted(sf['names'])}")
    
    # Generate report
    L = []
    def R(s=""): L.append(s)
    
    R("=" * 70)
    R("120FPS UNLOCK - TARGETED ANALYSIS - libUE4.so (BGMI 4.4.0)")
    R("=" * 70); R()
    
    R(f"Target strings: {len(targets)}")
    R(f"Xrefs found: {len(fps_xrefs)}")
    R(f"Functions: {len(fps_functions)}")
    R()
    
    R("TARGET STRINGS:")
    for name, addr in sorted(targets.items(), key=lambda x: x[1]):
        R(f"  0x{addr:X}: \"{name}\"")
    R()
    
    R("ALL XREFS:")
    for xr in sorted(fps_xrefs, key=lambda x: x["ref_addr"]):
        R(f"  @0x{xr['ref_addr']:X} [{xr['type']}] -> 0x{xr['str_addr']:X} \"{xr['str_name']}\"")
    R()
    
    R("=" * 70)
    R("TOP FUNCTIONS WITH FULL DISASSEMBLY")
    R("=" * 70)
    
    for i, sf in enumerate(scored):
        R(f"\n{'='*60}")
        R(f"FPS #{i+1} | Score: {sf['score']}")
        R(f"Address: 0x{sf['addr']:X}")
        R(f"Size: {sf['size']} bytes ({sf['num_insn']} instructions)")
        R(f"String names: {sorted(sf['names'])}")
        R()
        
        R("--- String References ---")
        for r in sf["refs"]:
            R(f"  0x{r['str_addr']:X}: \"{r['str_name']}\"")
        R()
        
        R("--- Disassembly (first 80 instructions) ---")
        for inst in sf["instructions"][:80]:
            R(f"  0x{inst.address:08X}  {inst.mnemonic:8s} {inst.op_str}")
        R()
    
    with open(OUTPUT, "w") as f:
        f.write("\n".join(L))
    
    print(f"\n[!] Report: {OUTPUT} ({len(L)} lines)")
    return scored

if __name__ == "__main__":
    scored = main()
