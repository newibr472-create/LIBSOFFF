#!/usr/bin/env python3
"""
120FPS Unlock Hunter - Scan libUE4.so for framerate limiter
Search FPS-related strings, find cap enforcement functions, create hook offset.
"""
import struct, lief
from collections import defaultdict
from capstone import *
from capstone.arm64 import *

BINARY = "/home/z/my-project/download/bg_extracted/lib/arm64-v8a/libUE4.so"
OUTPUT = "/home/z/my-project/download/fps_hunter_report.txt"

def main():
    print("[*] Loading libUE4.so (223MB)...")
    binary = lief.parse(BINARY)
    
    # Get key sections
    rodata = binary.get_section(".rodata")
    text = binary.get_section(".text")
    
    if not rodata or not text:
        print("[!] Missing sections"); return
    
    rodata_bytes = bytes(rodata.content)
    rodata_base = rodata.virtual_address
    text_bytes = bytes(text.content)
    text_base = text.virtual_address
    text_size = len(text_bytes)
    
    print(f"  .rodata: 0x{rodata_base:X} ({len(rodata_bytes)} bytes)")
    print(f"  .text:   0x{text_base:X} ({text_size} bytes)")
    
    # ======== STEP 1: FPS string patterns ========
    fps_keywords = [
        "MaxFPS", "maxfps", "MaxFps",
        "t.MaxFPS", "r.MaxFPS", "sg.MaxFPS",
        "SmoothFrameRate", "SmoothFrameRateRange",
        "FixedFrameRate", "FixedUpdateTimeStep",
        "FrameRate", "framerate", "frame_rate", "FrameRateLimit",
        "FPSCap", "fpscap", "FPSLimit", "fpslimit",
        "TargetFPS", "targetfps",
        "FrameTime", "frametime",
        "OneFrameThreadSleepTime",
        "GTSyncType", "SmoothFrameRate",
        "AllowRHIThread", "RHIThreadEnable",
        "FPS", "fps", "120", "144", "90",
        "MaxRefreshRate", "RefreshRate",
        "EnableSmoothFrameRate", "bUseFixedFrameRate",
        "Stat FPS", "STAT_FPS",
        "FEngineLoop", "UpdateAndRender",
        "FViewportClient", "Draw",
        "ApplicationTick", "Tick",
        "SetFrameRate", "GetFrameRate",
        "LimitFrameRate", "ApplyFrameRate",
        "fps_counter", "FPSCounter",
    ]
    
    # Also search for UE4 console variables (CVar) format
    fps_cvars = [
        "r.VSync", "r.GTSyncType",
        "t.MaxFPS", "sg.ViewDistanceQuality",
        "sg.AntiAliasingQuality", "sg.ShadowQuality",
        "sg.PostProcessQuality", "sg.TextureQuality",
        "sg.EffectsQuality", "sg.FoliageQuality",
    ]
    
    print("\n[1] Scanning for FPS-related strings in .rodata...")
    fps_strings = {}  # addr -> string
    
    # Scan .rodata
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
                for kw in fps_keywords + fps_cvars:
                    if kw.lower() in sl:
                        fps_strings[addr] = s; break
            current = b""
    
    # Also check .data.rel.ro (sometimes has string pointers)
    data_rel = binary.get_section(".data.rel.ro")
    if data_rel:
        db = bytes(data_rel.content)
        d_base = data_rel.virtual_address
        current = b""; start = 0
        for i, b in enumerate(db):
            if 0x20 <= b <= 0x7E:
                if not current: start = i
                current += bytes([b])
            else:
                if len(current) >= 3:
                    addr = d_base + start
                    s = current.decode('ascii', errors='replace')
                    sl = s.lower()
                    for kw in fps_keywords + fps_cvars:
                        if kw.lower() in sl:
                            fps_strings[addr] = s; break
                current = b""
    
    print(f"  Found {len(fps_strings)} FPS-related strings")
    
    # Categorize
    maxfps_strings = {}
    general_fps = {}
    for addr, s in fps_strings.items():
        sl = s.lower()
        if any(x in sl for x in ["maxfps", "framelimit", "fixedframerate", "smoothframerate"]):
            maxfps_strings[addr] = s
        else:
            general_fps[addr] = s
    
    print(f"  FPS Limit/Cap strings: {len(maxfps_strings)}")
    print(f"  General FPS strings: {len(general_fps)}")
    
    # ======== STEP 2: Scan for integer 60, 30, 90, 120 (MOV Wn, #value) ========
    print("\n[2] Searching for FPS value constants (MOV Wn, #60/#30/#90/#120)...")
    
    md = Cs(CS_ARCH_ARM64, CS_MODE_ARM)
    md.detail = True
    
    fps_value_refs = defaultdict(list)  # fps_value -> [{addr, instruction}]
    target_values = {30, 60, 90, 120, 144, 240}
    
    for offset in range(0, min(text_size, 10_000_000), 4):  # Scan first 10MB
        insn_word = struct.unpack("<I", text_bytes[offset:offset+4])[0]
        
        # MOV Wd, #imm16 (MOVZ) - encoding: 0 10 100101 00 imm16 Rd
        if (insn_word & 0xFF800000) == 0x52800000:
            rd = insn_word & 0x1F
            imm16 = (insn_word >> 5) & 0xFFFF
            if imm16 in target_values:
                addr = text_base + offset
                fps_value_refs[imm16].append({"offset": offset, "addr": addr})
    
    for val, refs in sorted(fps_value_refs.items()):
        print(f"  MOV Wn, #{val}: {len(refs)} occurrences")
    
    # ======== STEP 3: For FPS limit strings, find ADRP xrefs in .text ========
    print("\n[3] Finding code references to FPS limit strings...")
    
    # Build page map for FPS strings
    string_pages = {}
    for addr in fps_strings:
        page = addr & ~0xFFF
        if page not in string_pages:
            string_pages[page] = {}
        string_pages[page][addr & 0xFFF] = addr
    
    # Scan for ADRP targeting those pages (fast raw scan)
    fps_adrp_hits = []
    target_pages = set(string_pages.keys())
    
    scan_limit = min(text_size, 50_000_000)  # Scan up to 50MB
    for offset in range(0, scan_limit, 4):
        insn_word = struct.unpack("<I", text_bytes[offset:offset+4])[0]
        if (insn_word & 0x9F000000) != 0x90000000:
            continue
        immlo = (insn_word >> 29) & 0x3
        immhi = (insn_word >> 5) & 0x7FFFF
        imm = (immhi << 2) | immlo
        if imm & (1 << 20): imm -= (1 << 21)
        pc = text_base + offset
        page = (pc & ~0xFFF) + (imm << 12)
        if page in target_pages:
            fps_adrp_hits.append((offset, page, pc))
    
    print(f"  ADRP hits targeting FPS string pages: {len(fps_adrp_hits)}")
    
    # Resolve ADRP + ADD/LDR
    print("  Resolving string references...")
    fps_xrefs = []
    
    for adrp_off, page, pc in fps_adrp_hits:
        chunk_end = min(text_size, adrp_off + 40)
        code = text_bytes[adrp_off:chunk_end]
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
                    if full in fps_strings:
                        fps_xrefs.append({"ref_addr": inst.address, "str_addr": full, "string": fps_strings[full], "type": "ADR+ADD"})
                        break
            elif inst.mnemonic == "ldr" and len(inst.operands) >= 2:
                op1 = inst.operands[1]
                if op1.type == ARM64_OP_MEM and op1.mem.base == adrp_reg:
                    full = adrp_page_val + (op1.mem.disp if op1.mem.disp else 0)
                    if full in fps_strings:
                        fps_xrefs.append({"ref_addr": inst.address, "str_addr": full, "string": fps_strings[full], "type": "ADR+LDR"})
                        break
    
    print(f"  Resolved FPS string xrefs: {len(fps_xrefs)}")
    
    # Group into function regions
    fps_functions = defaultdict(lambda: {"refs": [], "addr": 0, "size": 0, "offset": 0})
    
    for xr in fps_xrefs:
        ref_off = xr["ref_addr"] - text_base
        # Find prologue
        func_start = ref_off
        for back in range(ref_off, max(0, ref_off - 2048), -4):
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
    
    # Calculate sizes
    sorted_addrs = sorted(fps_functions.keys())
    for i, addr in enumerate(sorted_addrs):
        if i + 1 < len(sorted_addrs):
            fps_functions[addr]["size"] = sorted_addrs[i+1] - addr
        else:
            fps_functions[addr]["size"] = 4096
    
    print(f"  Functions referencing FPS strings: {len(fps_functions)}")
    
    # ======== STEP 4: Score FPS functions ========
    print("\n[4] Scoring FPS functions...")
    
    scored_fps = []
    
    for faddr, fm in sorted(fps_functions.items(), key=lambda x: -len(x[1]["refs"])):
        off = fm["offset"]
        size = min(fm["size"], 4096)
        code = text_bytes[off:off+size]
        instructions = list(md.disasm(code, faddr))
        
        # Count references to maxfps/limit strings
        limit_refs = sum(1 for r in fm["refs"] if any(x in r["string"].lower() for x in ["maxfps", "limit", "fixed", "smooth", "cap", "lock"]))
        
        # Score
        score = limit_refs * 20 + len(fm["refs"]) * 5
        
        # Size bonus
        if 20 <= size <= 200: score += 25   # Small = likely a simple setter/getter
        elif 200 < size <= 800: score += 15
        
        scored_fps.append({
            "addr": faddr, "offset": off, "size": size, "score": score,
            "num_refs": len(fm["refs"]), "limit_refs": limit_refs,
            "refs": fm["refs"], "instructions": instructions,
            "num_insn": len(instructions),
        })
    
    scored_fps.sort(key=lambda x: -x["score"])
    
    print(f"\n  TOP 15 FPS FUNCTIONS:")
    for i, sf in enumerate(scored_fps[:15]):
        ref_strs = [r["string"][:40] for r in sf["refs"][:3]]
        print(f"    #{i+1:2d} 0x{sf['addr']:X} score={sf['score']:3d} size={sf['size']:5d}B refs={sf['num_refs']} limit={sf['limit_refs']} | {ref_strs}")
    
    # ======== STEP 5: Disassemble top candidates ========
    print(f"\n[5] Generating report...")
    L = []
    def R(s=""): L.append(s)
    
    R("=" * 70)
    R("120FPS UNLOCK HUNTER - libUE4.so (BGMI 4.4.0)")
    R("=" * 70); R()
    
    R(f"Total FPS strings: {len(fps_strings)}")
    R(f"FPS Limit strings: {len(maxfps_strings)}")
    R(f"ADRP hits: {len(fps_adrp_hits)}")
    R(f"Resolved xrefs: {len(fps_xrefs)}")
    R(f"FPS functions: {len(fps_functions)}")
    R()
    
    # Section A: All FPS strings
    R("=" * 70)
    R("SECTION A: ALL FPS-RELATED STRINGS")
    R("=" * 70)
    for addr in sorted(fps_strings.keys()):
        s = fps_strings[addr]
        tag = " *** LIMIT/CAP ***" if addr in maxfps_strings else ""
        R(f"  0x{addr:X}: \"{s[:100]}\"{tag}")
    R()
    
    # Section B: FPS constant locations
    R("=" * 70)
    R("SECTION B: FPS VALUE CONSTANTS (MOV Wn, #N)")
    R("=" * 70)
    for val in sorted(fps_value_refs.keys()):
        refs = fps_value_refs[val]
        R(f"  #{val}: {len(refs)} occurrences")
        for r in refs[:10]:
            R(f"    0x{r['addr']:X}")
        if len(refs) > 10:
            R(f"    ... and {len(refs)-10} more")
    R()
    
    # Section C: FPS string xrefs
    R("=" * 70)
    R("SECTION C: ALL FPS STRING CROSS-REFERENCES")
    R("=" * 70)
    for xr in sorted(fps_xrefs, key=lambda x: x["ref_addr"]):
        R(f"  @0x{xr['ref_addr']:X} [{xr['type']}] -> 0x{xr['str_addr']:X} \"{xr['string'][:80]}\"")
    R()
    
    # Section D: Top functions with disassembly
    R("=" * 70)
    R("SECTION D: TOP 15 FPS FUNCTIONS (FULL DISASSEMBLY)")
    R("=" * 70)
    
    for i, sf in enumerate(scored_fps[:15]):
        R(f"\n{'='*60}")
        R(f"FPS RANK #{i+1} | Score: {sf['score']}")
        R(f"Address: 0x{sf['addr']:X}")
        R(f"Size: {sf['size']} bytes ({sf['num_insn']} instructions)")
        R(f"FPS string refs: {sf['num_refs']} (limit-related: {sf['limit_refs']})")
        R()
        
        R("--- String References ---")
        for r in sf["refs"]:
            R(f"  0x{r['str_addr']:X} [{r['type']}] \"{r['string'][:100]}\"")
        R()
        
        R("--- Disassembly (first 64 instructions) ---")
        for inst in sf["instructions"][:64]:
            R(f"  0x{inst.address:08X}  {inst.mnemonic:8s} {inst.op_str}")
        R()
    
    # Section E: Complete ranking
    R("=" * 70)
    R("SECTION E: COMPLETE FPS FUNCTION RANKING")
    R("=" * 70)
    for i, sf in enumerate(scored_fps):
        ref_strs = '; '.join(r['string'][:30] for r in sf['refs'][:2])
        R(f"  #{i+1:2d} score={sf['score']:3d} | 0x{sf['addr']:X} | {sf['size']:5d}B | refs={sf['num_refs']} limit={sf['limit_refs']} | {ref_strs}")
    R()
    
    with open(OUTPUT, "w") as f:
        f.write("\n".join(L))
    
    print(f"[!] Report: {OUTPUT} ({len(L)} lines)")
    
    return scored_fps[:5]

if __name__ == "__main__":
    main()
