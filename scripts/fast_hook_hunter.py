#!/usr/bin/env python3
"""
FAST Hook Discovery v2 - Fixed register matching
Uses Capstone for ADRP register extraction (consistent numbering).
"""
import struct, lief
from collections import defaultdict, Counter
from capstone import *
from capstone.arm64 import *

BINARY = "/home/z/my-project/download/bg_extracted/lib/arm64-v8a/libanogs.so"
OUTPUT = "/home/z/my-project/download/fast_hook_report.txt"

def main():
    print("[*] Loading libanogs.so...")
    binary = lief.parse(BINARY)
    
    rodata = binary.get_section(".rodata")
    text = binary.get_section(".text")
    data_sec = binary.get_section(".data")
    
    rodata_bytes = bytes(rodata.content)
    rodata_base = rodata.virtual_address
    text_bytes = bytes(text.content)
    text_base = text.virtual_address
    text_size = len(text_bytes)
    
    print(f"  .rodata: 0x{rodata_base:X} ({len(rodata_bytes)} bytes)")
    print(f"  .text:   0x{text_base:X} ({text_size} bytes)")
    
    # ======== STEP 1: Extract detection strings ========
    print("\n[1] Scanning detection strings...")
    detection_keywords = [
        "ms_scan", "ms_set", "scan_start", "scan_end", "scan_mem",
        "inline_hook", "hook_detect", "dismatch", "hook_check",
        "/proc/self", "/proc/", "maps", "cmdline", "status",
        "ptrace", "TracerPid", "debugger", "anti_debug",
        "su_binary", "magisk", "superuser", "supersu",
        "report", "upload", "violation", "detect", "punish",
        "AnoSDK", "anogs",
        "integrity", "verify", "signature", "checksum",
        "mprotect", "mmap", "page_protect",
        "encrypt", "decrypt", "cipher",
        "thread", "timer", "interval", "periodic",
        "emulator", "qemu", "goldfish",
        "signal", "sigaction",
        "dlopen", "dlsym", "strstr", "strcmp", "memcmp",
        "memory", "check_flag", "result", "log",
        "CertMD5", "debugger", "hook_found",
    ]
    
    det_strings = {}
    current = b""; start = 0
    for i, b in enumerate(rodata_bytes):
        if 0x20 <= b <= 0x7E:
            if not current: start = i
            current += bytes([b])
        else:
            if len(current) >= 3:
                addr = rodata_base + start
                s = current.decode('ascii', errors='replace')
                sl = s.lower()
                for kw in detection_keywords:
                    if kw.lower() in sl:
                        det_strings[addr] = s; break
            current = b""
    if len(current) >= 3:
        addr = rodata_base + start
        s = current.decode('ascii', errors='replace')
        for kw in detection_keywords:
            if kw.lower() in s.lower():
                det_strings[addr] = s; break
    
    # Also scan .data section
    if data_sec:
        db = bytes(data_sec.content)
        d_base = data_sec.virtual_address
        current = b""; start = 0
        for i, b in enumerate(db):
            if 0x20 <= b <= 0x7E:
                if not current: start = i
                current += bytes([b])
            else:
                if len(current) >= 3:
                    addr = d_base + start
                    s = current.decode('ascii', errors='replace')
                    for kw in detection_keywords:
                        if kw.lower() in s.lower():
                            det_strings[addr] = s; break
                current = b""
    
    print(f"  Found {len(det_strings)} detection strings")
    
    # String page map
    string_pages = {}
    for addr in det_strings:
        page = addr & ~0xFFF
        if page not in string_pages:
            string_pages[page] = {}
        string_pages[page][addr & 0xFFF] = addr
    
    print(f"  Target pages: {len(string_pages)}")
    
    # ======== STEP 2: Fast ADRP scan (raw bits) + Capstone verify ========
    print("\n[2] Scanning ADRP instructions...")
    md = Cs(CS_ARCH_ARM64, CS_MODE_ARM)
    md.detail = True
    
    # Phase 2a: Find ADRP targeting string pages (raw scan - FAST)
    adrp_candidates = []  # (offset, insn_word, pc)
    for offset in range(0, text_size, 4):
        insn_word = struct.unpack("<I", text_bytes[offset:offset+4])[0]
        if (insn_word & 0x9F000000) != 0x90000000:
            continue
        rd_bits = insn_word & 0x1F
        immlo = (insn_word >> 29) & 0x3
        immhi = (insn_word >> 5) & 0x7FFFF
        imm = (immhi << 2) | immlo
        if imm & (1 << 20): imm -= (1 << 21)
        pc = text_base + offset
        target_page = (pc & ~0xFFF) + (imm << 12)
        if target_page in string_pages:
            adrp_candidates.append((offset, pc))
    
    print(f"  ADRP candidates: {len(adrp_candidates)}")
    
    # Phase 2b: Disassemble ADRP + next 8 instructions, match with Capstone registers
    string_xrefs = []  # [(adrp_pc, resolved_str_addr, string, type)]
    
    for adrp_off, adrp_pc in adrp_candidates:
        # Disassemble ADRP + 8 following instructions (36 bytes)
        chunk_end = min(text_size, adrp_off + 40)
        code = text_bytes[adrp_off:chunk_end]
        insns = list(md.disasm(code, adrp_pc))
        
        if not insns or insns[0].mnemonic != "adrp":
            continue
        
        adrp_reg = insns[0].operands[0].reg if insns[0].operands else None
        if adrp_reg is None:
            continue
        
        # Get the target page from Capstone's own calculation
        adrp_target_page = insns[0].operands[1].imm if len(insns[0].operands) > 1 else 0
        
        # Check subsequent instructions for ADD Xreg, Xreg, #imm or LDR Xreg, [Xreg, #imm]
        for inst in insns[1:8]:
            if inst.mnemonic == "add" and len(inst.operands) >= 3:
                op0 = inst.operands[0]
                op1 = inst.operands[1]
                op2 = inst.operands[2]
                if op1.type == ARM64_OP_REG and op2.type == ARM64_OP_IMM:
                    if op1.reg == adrp_reg:
                        full_addr = adrp_target_page + op2.imm
                        if full_addr in det_strings:
                            string_xrefs.append((inst.address, full_addr, det_strings[full_addr], "ADR+ADD"))
                            break
            
            elif inst.mnemonic == "ldr" and len(inst.operands) >= 2:
                op0 = inst.operands[0]
                op1 = inst.operands[1]
                if op1.type == ARM64_OP_MEM:
                    if op1.mem.base == adrp_reg:
                        full_addr = adrp_target_page + (op1.mem.disp if op1.mem.disp else 0)
                        if full_addr in det_strings:
                            string_xrefs.append((inst.address, full_addr, det_strings[full_addr], "ADR+LDR"))
                            break
    
    print(f"  Resolved string xrefs: {len(string_xrefs)}")
    
    # ======== STEP 3: Group into functions ========
    print("\n[3] Grouping into functions...")
    
    # For each xref, find function boundary by scanning back for prologue
    func_map = defaultdict(lambda: {
        "addr": 0, "refs": [], "instructions": [], "bl_targets": [], 
        "size": 0, "file_offset": 0, "svc_count": 0, "stores": 0
    })
    
    for ref_addr, str_addr, string, xtype in string_xrefs:
        ref_offset = ref_addr - text_base
        
        # Scan backward for function prologue
        func_start_offset = ref_offset
        for back in range(ref_offset, max(0, ref_offset - 1024), -4):
            word = struct.unpack("<I", text_bytes[back:back+4])[0]
            # STP X29, X30 pattern (multiple variants)
            if (word & 0x7FC07FFF) == 0xA9007BFD:  # STP FP, LR [SP, #imm]!
                func_start_offset = back; break
            elif (word & 0x7FC03FFF) == 0xA9807BFD:  # STP FP, LR [SP, #imm]
                func_start_offset = back; break
            elif (word & 0x7FC07FFF) == 0xA9807BFD:
                func_start_offset = back; break
        
        func_addr = text_base + func_start_offset
        
        # Scan forward for function end
        func_end_offset = min(text_size, func_start_offset + 4096)
        for fwd in range(func_start_offset + 16, min(text_size, func_start_offset + 4096), 4):
            word = struct.unpack("<I", text_bytes[fwd:fwd+4])[0]
            if word == 0xD65F03C0:  # RET
                func_end_offset = fwd + 4; break
        
        func_size = func_end_offset - func_start_offset
        fm = func_map[func_addr]
        fm["addr"] = func_addr
        fm["file_offset"] = func_start_offset
        fm["size"] = max(fm["size"], func_size)
        fm["refs"].append({"str_addr": str_addr, "string": string, "ref_addr": ref_addr, "type": xtype})
    
    print(f"  Functions found: {len(func_map)}")
    
    # ======== STEP 4: Disassemble & score ========
    print("\n[4] Disassembling and scoring...")
    
    scored = []
    EXCLUDED = {"31DCB0", "369658"}
    
    for faddr, fm in func_map.items():
        if f"{faddr:X}" in EXCLUDED:
            continue
        
        off = fm["file_offset"]
        size = min(fm["size"], 4096)
        code = text_bytes[off:off+size]
        instructions = list(md.disasm(code, faddr))
        fm["instructions"] = instructions
        
        if len(instructions) < 4:
            continue
        
        # Analyze instructions
        bl_targets = []
        svc_count = 0
        stores = 0
        cbz_cbnz = 0  # Conditional branches
        
        for inst in instructions:
            if inst.mnemonic == "bl" and inst.operands:
                bl_targets.append(inst.operands[0].imm)
            elif inst.mnemonic == "svc":
                svc_count += 1
            elif inst.mnemonic in ("str", "stp", "strb", "strh", "stur", "stnp", "st1", "st2", "st3", "st4"):
                stores += 1
            elif inst.mnemonic in ("cbz", "cbnz", "b.eq", "b.ne", "b.gt", "b.lt", "b.ge", "b.le",
                                    "tbz", "tbnz", "beq", "bne", "bgt", "blt", "bge", "ble"):
                cbz_cbnz += 1
        
        # Get categories
        categories = set()
        for r in fm["refs"]:
            s = r["string"].lower()
            if any(x in s for x in ["ms_scan","scan_start","scan_end","ms_set","scan_mem"]): categories.add("MEMORY_SCAN")
            if any(x in s for x in ["inline_hook","hook_detect","dismatch","hook_check"]): categories.add("HOOK_DETECT")
            if any(x in s for x in ["/proc","maps","cmdline"]): categories.add("PROC_ACCESS")
            if any(x in s for x in ["ptrace","debugger","TracerPid","anti_debug"]): categories.add("ANTI_DEBUG")
            if any(x in s for x in ["report","violation","upload","punish","detect"]): categories.add("REPORT")
            if any(x in s for x in ["integrity","verify","signature","CertMD5"]): categories.add("INTEGRITY")
            if any(x in s for x in ["mprotect","mmap"]): categories.add("MEMORY_PROTECT")
            if any(x in s for x in ["strstr","strcmp","memcmp"]): categories.add("STRING_COMPARE")
            if any(x in s for x in ["magisk","superuser","supersu","su_binary"]): categories.add("ROOT_DETECT")
            if any(x in s for x in ["emulator","qemu","goldfish"]): categories.add("EMULATOR")
            if any(x in s for x in ["signal","sigaction"]): categories.add("SIGNAL")
            if any(x in s for x in ["encrypt","decrypt","cipher"]): categories.add("ENCRYPT")
        
        # Score
        score = 0
        num_det_refs = len(fm["refs"])
        score += num_det_refs * 12
        score += len(categories) * 8
        score += min(len(bl_targets), 20) * 2
        score += svc_count * 10
        score += min(stores, 20)
        score += min(cbz_cbnz, 15)  # Conditional branching = decision logic
        
        if 30 <= size <= 300: score += 20   # Small perfect for hook
        elif 300 < size <= 800: score += 15
        elif 800 < size <= 2000: score += 10
        elif 2000 < size <= 4000: score += 5
        
        # Bonus: high-priority categories
        for cat in categories:
            if cat in ("MEMORY_SCAN","HOOK_DETECT","REPORT","INTEGRITY","MEMORY_PROTECT"):
                score += 12
        
        scored.append({
            "addr": faddr,
            "offset": off,
            "size": size,
            "score": score,
            "num_det_refs": num_det_refs,
            "num_bl": len(bl_targets),
            "num_svc": svc_count,
            "num_stores": stores,
            "num_branches": cbz_cbnz,
            "categories": categories,
            "refs": fm["refs"],
            "instructions": instructions,
            "bl_targets": bl_targets,
            "num_insn": len(instructions),
        })
    
    scored.sort(key=lambda x: -x["score"])
    
    print(f"\n  TOP 15 HOOK CANDIDATES:")
    for i, sf in enumerate(scored[:15]):
        cats = ','.join(sorted(sf['categories'])) if sf['categories'] else '-'
        print(f"    #{i+1:2d} 0x{sf['addr']:X} score={sf['score']:3d} size={sf['size']:5d}B insn={sf['num_insn']:4d} refs={sf['num_det_refs']} BL={sf['num_bl']} SVC={sf['num_svc']} cats=[{cats}]")
    
    # ======== STEP 5: Full report ========
    print(f"\n[5] Writing report...")
    L = []
    def R(s=""): L.append(s)
    
    R("=" * 70)
    R("FAST HOOK DISCOVERY v2 - libanogs.so (BGMI 4.4.0)")
    R("Method: ADRP raw scan + Capstone register-matched resolution")
    R("=" * 70); R()
    
    R(f"Detection strings: {len(det_strings)}")
    R(f"ADRP candidates: {len(adrp_candidates)}")
    R(f"Resolved xrefs: {len(string_xrefs)}")
    R(f"Functions found: {len(scored)}")
    R()
    
    # Section A: Detection strings
    R("=" * 70)
    R("SECTION A: DETECTION STRINGS")
    R("=" * 70)
    for addr in sorted(det_strings.keys()):
        R(f"  0x{addr:X}: \"{det_strings[addr]}\"")
    R()
    
    # Section B: All xrefs
    R("=" * 70)
    R("SECTION B: ALL STRING CROSS-REFERENCES")
    R("=" * 70)
    for ref_addr, str_addr, string, xtype in string_xrefs:
        R(f"  @0x{ref_addr:X} [{xtype}] -> 0x{str_addr:X} \"{string[:90]}\"")
    R()
    
    # Section C: Top candidates with full disassembly
    R("=" * 70)
    R("SECTION C: TOP 15 HOOK CANDIDATES (FULL DISASSEMBLY)")
    R("=" * 70)
    
    for i, sf in enumerate(scored[:15]):
        R(f"\n{'='*60}")
        R(f"RANK #{i+1} | Score: {sf['score']}")
        R(f"Address: 0x{sf['addr']:X}")
        R(f"Size: {sf['size']} bytes ({sf['num_insn']} instructions)")
        R(f"Detection refs: {sf['num_det_refs']}")
        R(f"Categories: {', '.join(sorted(sf['categories'])) if sf['categories'] else 'NONE'}")
        R(f"BL calls: {sf['num_bl']} | SVC: {sf['num_svc']} | Stores: {sf['num_stores']} | Branches: {sf['num_branches']}")
        R()
        
        R("--- String References ---")
        for r in sf["refs"]:
            R(f"  0x{r['str_addr']:X} [{r['type']}] \"{r['string'][:100]}\"")
        R()
        
        R("--- Full Disassembly ---")
        for inst in sf["instructions"]:
            R(f"  0x{inst.address:08X}  {inst.mnemonic:8s} {inst.op_str}")
        R()
        
        if sf["bl_targets"]:
            R("--- Call Targets ---")
            seen = set()
            for t in sf["bl_targets"]:
                if t not in seen:
                    seen.add(t)
                    R(f"  BL 0x{t:X}")
            R()
    
    # Section D: Complete ranking
    R("=" * 70)
    R("SECTION D: COMPLETE RANKING")
    R("=" * 70)
    for i, sf in enumerate(scored):
        cats = ','.join(sorted(sf['categories'])) if sf['categories'] else '-'
        R(f"  #{i+1:2d} score={sf['score']:3d} | 0x{sf['addr']:X} | {sf['size']:5d}B | {sf['num_insn']:4d} insn | refs={sf['num_det_refs']} | BL={sf['num_bl']} | SVC={sf['num_svc']} | {cats}")
    R()
    
    with open(OUTPUT, "w") as f:
        f.write("\n".join(L))
    
    print(f"[!] Report: {OUTPUT} ({len(L)} lines)")
    return scored

if __name__ == "__main__":
    scored = main()
