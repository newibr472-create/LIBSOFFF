#!/usr/bin/env python3
"""
Independent Anti-Cheat Function Discovery in libanogs.so
NO prior offsets used. Pure reverse engineering analysis.
Uses: ELF parsing, string extraction, cross-reference mapping, function scoring.
"""

import struct
import os
import re
from collections import defaultdict, Counter
from capstone import *
from capstone.arm64 import *
import lief

BINARY_PATH = "/home/z/my-project/download/bg_extracted/lib/arm64-v8a/libanogs.so"
OUTPUT_PATH = "/home/z/my-project/download/new_hook_report.txt"

# ============================================================
# PHASE 0: ELF Parsing
# ============================================================
def parse_elf(path):
    """Parse ELF binary using LIEF for comprehensive analysis."""
    print(f"[*] Loading: {path}")
    binary = lief.parse(path)
    
    info = {
        "binary": binary,
        "path": path,
        "image_base": binary.imagebase,
        "segments": {},
        "sections": {},
        "functions": {},
        "plt_entries": {},
        "relocations": [],
        "dynsyms": [],
        "imports": {},
        "exports": [],
    }
    
    # Parse loadable segments
    for seg in binary.segments:
        if seg.type == lief.ELF.Segment.TYPE.LOAD:
            info["segments"][seg.virtual_address] = {
                "vaddr": seg.virtual_address,
                "size": seg.virtual_size,
                "flags": seg.flags,
                "offset": seg.file_offset,
            }
    
    # Parse sections
    for sec in binary.sections:
        info["sections"][sec.name] = {
            "vaddr": sec.virtual_address,
            "size": sec.size,
            "offset": sec.offset,
            "content": bytes(sec.content),
        }
    
    # Parse dynamic symbols (imports/exports)
    for sym in binary.dynamic_symbols:
        if sym.value != 0:
            info["dynsyms"].append({
                "name": sym.name,
                "addr": sym.value,
                "size": sym.size,
                "type": str(sym.type),
                "binding": str(sym.binding),
                "imported": sym.imported,
                "exported": sym.exported,
            })
            if sym.imported:
                info["imports"][sym.name] = sym.value
            elif sym.exported:
                info["exports"].append({"name": sym.name, "addr": sym.value})
    
    # Parse PLT
    for sec in binary.sections:
        if sec.name == ".plt":
            info["plt_entries"]["section"] = sec
    
    # Parse relocations
    for rel in binary.pltgot_relocations:
        info["relocations"].append({
            "addr": rel.address,
            "symbol": rel.symbol.name if rel.symbol else None,
            "type": str(rel.type),
        })
    for rel in binary.dynamic_relocations:
        info["relocations"].append({
            "addr": rel.address,
            "symbol": rel.symbol.name if rel.symbol else None,
            "type": str(rel.type),
        })
    
    return info

# ============================================================
# PHASE 1: String Discovery
# ============================================================
def find_all_strings(info):
    """Extract ALL ASCII strings from the binary."""
    strings = {}  # addr -> string
    rodata = info["sections"].get(".rodata")
    
    if rodata:
        data = rodata["content"]
        base = rodata["vaddr"]
        
        # Scan for printable ASCII sequences
        current = b""
        current_start = 0
        
        for i, b in enumerate(data):
            if 0x20 <= b <= 0x7E:  # Printable ASCII
                if not current:
                    current_start = i
                current += bytes([b])
            else:
                if len(current) >= 4:  # Minimum string length
                    addr = base + current_start
                    strings[addr] = current.decode('ascii', errors='replace')
                current = b""
        
        # Handle last string
        if len(current) >= 4:
            addr = base + current_start
            strings[addr] = current.decode('ascii', errors='replace')
    
    # Also scan .data section
    data_sec = info["sections"].get(".data")
    if data_sec:
        data = data_sec["content"]
        base = data_sec["vaddr"]
        current = b""
        current_start = 0
        for i, b in enumerate(data):
            if 0x20 <= b <= 0x7E:
                if not current:
                    current_start = i
                current += bytes([b])
            else:
                if len(current) >= 4:
                    addr = base + current_start
                    strings[addr] = current.decode('ascii', errors='replace')
                current = b""
    
    return strings

def filter_detection_strings(strings):
    """Filter strings that are anti-cheat related."""
    categories = {
        "MEMORY_SCAN": ["ms_scan", "mem_scan", "scan_mem", "memory_scan", "scan_start", "scan_end", "scan_result", "ms_set"],
        "HOOK_DETECT": ["inline_hook", "hook_detect", "hook_check", "dismatch", "opcode_check", "hook_found", "il2cpp_hook"],
        "PROC_ACCESS": ["/proc/self", "/proc/", "maps", "status", "cmdline", "mem", "fd/", "task/"],
        "ANTI_DEBUG": ["ptrace", "TracerPid", "debugger", "anti_debug", "jdwp", "frida", "xposed", "substrate"],
        "ROOT_DETECT": ["su_binary", "superuser", "magisk", "supersu", "busybox", "daemonsu", "chainfire"],
        "INTEGRITY": ["signature", "integrity", "cert", "verify", "checksum", "hash_check", "md5", "sha256"],
        "REPORT": ["report", "upload", "send_data", "violation", "detect", "alert", "punish", "ban"],
        "ANOSDK": ["AnoSDK", "anosdk", "anogs", "ano_", "tencent_ace"],
        "THREAD": ["thread_create", "scan_thread", "check_thread", "timer", "interval"],
        "EMULATOR": ["emulator", "simulator", "qemu", "vbox", "goldfish", "ranchu", "vmservice"],
        "LIBRARY": ["lib/", ".so", "dlopen", "dlsym", "load_lib"],
        "SIGNAL": ["signal", "sigaction", "sigprocmask", "SIGKILL", "SIGSEGV"],
        "MEMORY_PROTECT": ["mprotect", "mmap", "munmap", "page_protect", "write_protect"],
        "ENCRYPT": ["encrypt", "decrypt", "aes", "rc4", "xor_key", "cipher"],
    }
    
    matched = {}
    for addr, s in strings.items():
        sl = s.lower()
        for cat, keywords in categories.items():
            for kw in keywords:
                if kw.lower() in sl:
                    matched[addr] = {"string": s, "category": cat, "keyword": kw}
                    break
            if addr in matched:
                break
    
    return matched

# ============================================================
# PHASE 2: Code Section Disassembly & Cross-Reference Mapping
# ============================================================
def find_adrp_references(text_content, text_base, strings, info):
    """
    In ARM64, strings are loaded via ADRP + ADD or ADRP + LDR.
    Find all ADRP instructions that point to string addresses.
    Returns: {function_addr: [{string_addr, string, instruction_addr}]}
    """
    md = Cs(CS_ARCH_ARM64, CS_MODE_ARM)
    md.detail = True
    
    refs = defaultdict(list)
    adrp_pages = {}  # Store ADRP target pages for each instruction
    
    instructions = list(md.disasm(text_content, text_base))
    
    for inst in instructions:
        mnemonic = inst.mnemonic
        if mnemonic == "adrp":
            # ADRP Xn, #page
            if inst.operands:
                op = inst.operands[0]
                if op.type == ARM64_OP_REG:
                    # Get page address
                    page_addr = inst.operands[1].imm if len(inst.operands) > 1 else 0
                    adrp_pages[inst.address] = {
                        "reg": op.reg,
                        "page": page_addr,
                    }
        
        elif mnemonic == "add" and inst.operands:
            # ADD Xn, Xn, #offset -> combines with ADRP
            op0 = inst.operands[0]
            op1 = inst.operands[1]
            op2 = inst.operands[2] if len(inst.operands) > 2 else None
            
            if op1.type == ARM64_OP_REG and op2 and op2.type == ARM64_OP_IMM:
                # Check if any recent ADRP loaded the base register
                for prev_addr in range(inst.address - 8, inst.address - 20, -4):
                    if prev_addr in adrp_pages:
                        ap = adrp_pages[prev_addr]
                        if ap["reg"] == op1.reg:
                            full_addr = ap["page"] + op2.imm
                            if full_addr in strings:
                                refs[inst.address].append({
                                    "string_addr": full_addr,
                                    "string": strings[full_addr],
                                    "ref_type": "ADR+ADD",
                                    "ref_addr": inst.address,
                                })
                            break
        
        elif mnemonic == "ldr" and inst.operands:
            # LDR Xn, [Xn, #offset] -> loads from memory page
            op0 = inst.operands[0]
            op1 = inst.operands[1]
            if op1.type == ARM64_OP_MEM:
                base_reg = op1.mem.base
                offset = op1.mem.disp if op1.mem.disp else 0
                
                for prev_addr in range(inst.address - 8, inst.address - 20, -4):
                    if prev_addr in adrp_pages:
                        ap = adrp_pages[prev_addr]
                        if ap["reg"] == base_reg:
                            full_addr = ap["page"] + offset
                            if full_addr in strings:
                                refs[inst.address].append({
                                    "string_addr": full_addr,
                                    "string": strings[full_addr],
                                    "ref_type": "ADR+LDR",
                                    "ref_addr": inst.address,
                                })
                            break
    
    return refs, instructions

def find_bl_targets(instructions, info):
    """Find all BL (branch with link) calls and their targets."""
    calls = defaultdict(list)
    
    for inst in instructions:
        if inst.mnemonic == "bl" and inst.operands:
            target = inst.operands[0].imm
            # Resolve to import or function name
            target_name = None
            for imp_name, imp_addr in info["imports"].items():
                if imp_addr == target:
                    target_name = f"IMP:{imp_name}"
                    break
            
            if target_name is None:
                # Check PLT relocations
                for rel in info["relocations"]:
                    if rel["addr"] == target and rel["symbol"]:
                        target_name = f"PLT:{rel['symbol']}"
                        break
            
            if target_name is None:
                target_name = f"0x{target:X}"
            
            calls[inst.address].append({
                "target": target,
                "name": target_name,
                "addr": inst.address,
            })
    
    return calls

# ============================================================
# PHASE 3: Function Boundary Detection & Mapping
# ============================================================
def detect_function_boundaries(info, instructions):
    """
    Use ELF symbol table + .init_array + heuristic to find function boundaries.
    """
    functions = {}
    
    # From dynamic symbols
    for sym in info["dynsyms"]:
        if sym["addr"] != 0 and sym["name"] and not sym["imported"]:
            functions[sym["addr"]] = {
                "name": sym["name"],
                "addr": sym["addr"],
                "size": sym["size"] if sym["size"] > 0 else 0,
                "type": sym["type"],
            }
    
    # From .init_array (constructors)
    init_array = info["sections"].get(".init_array")
    if init_array:
        data = init_array["content"]
        base = init_array["vaddr"]
        for i in range(0, len(data), 8):
            if i + 8 <= len(data):
                addr = struct.unpack("<Q", data[i:i+8])[0]
                if addr != 0:
                    functions[addr] = functions.get(addr, {
                        "name": f"init_array_{i//8}",
                        "addr": addr,
                        "size": 0,
                        "type": "init_array",
                    })
    
    # Heuristic: find function prologues (STP X29, X30 / SUB SP, SP patterns)
    text_section = info["sections"].get(".text")
    if text_section:
        text_start = text_section["vaddr"]
        text_end = text_start + text_section["size"]
        
        for inst in instructions:
            if inst.address < text_start or inst.address >= text_end:
                continue
            
            # Common ARM64 function prologues
            hex_bytes = inst.bytes.hex() if inst.bytes else ""
            
            # STP X29, X30, [SP, #-N]!
            if inst.mnemonic == "stp" and inst.operands:
                ops = inst.operands
                if len(ops) >= 2:
                    reg1 = ops[0].reg if ops[0].type == ARM64_OP_REG else None
                    reg2 = ops[1].reg if ops[1].type == ARM64_OP_REG else None
                    if reg1 in (ARM64_REG_X29, ARM64_REG_FP) and reg2 == ARM64_REG_X30:
                        if inst.address not in functions:
                            functions[inst.address] = {
                                "name": f"FUN_{inst.address:X}",
                                "addr": inst.address,
                                "size": 0,
                                "type": "prologue",
                            }
            
            # SUB SP, SP, #imm (alternate prologue)
            elif inst.mnemonic == "sub" and inst.operands:
                ops = inst.operands
                if len(ops) >= 2:
                    r1 = ops[0].reg if ops[0].type == ARM64_OP_REG else None
                    r2 = ops[1].reg if ops[1].type == ARM64_OP_REG else None
                    if r1 == ARM64_REG_SP and r2 == ARM64_REG_SP:
                        if inst.address not in functions:
                            functions[inst.address] = {
                                "name": f"FUN_{inst.address:X}",
                                "addr": inst.address,
                                "size": 0,
                                "type": "sub_sp_prologue",
                            }
    
    # Calculate sizes
    sorted_addrs = sorted(functions.keys())
    for i, addr in enumerate(sorted_addrs):
        if functions[addr]["size"] == 0:
            if i + 1 < len(sorted_addrs):
                functions[addr]["size"] = sorted_addrs[i+1] - addr
            else:
                functions[addr]["size"] = 256  # estimate
    
    return functions

def map_refs_to_functions(refs, functions):
    """Map string references to containing functions."""
    func_strings = defaultdict(list)
    
    sorted_addrs = sorted(functions.keys())
    
    for ref_addr, ref_list in refs.items():
        # Find containing function
        containing = None
        for faddr in sorted_addrs:
            if faddr <= ref_addr < faddr + functions[faddr]["size"]:
                containing = faddr
                break
        
        if containing is not None:
            for r in ref_list:
                func_strings[containing].append(r)
    
    return func_strings

def map_calls_to_functions(calls, functions):
    """Map BL calls to containing functions."""
    func_calls = defaultdict(list)
    
    sorted_addrs = sorted(functions.keys())
    
    for call_addr, call_list in calls.items():
        for faddr in sorted_addrs:
            if faddr <= call_addr < faddr + functions[faddr]["size"]:
                for c in call_list:
                    func_calls[faddr].append(c)
                break
    
    return func_calls

# ============================================================
# PHASE 4: Scoring & Ranking
# ============================================================
def score_functions(func_strings, func_calls, functions, detection_strings):
    """Score each function by anti-cheat relevance."""
    
    EXCLUDED = {"31DCB0", "369658"}  # Previously analyzed offsets
    
    scored = []
    
    for faddr, str_refs in func_strings.items():
        hex_str = f"{faddr:X}"
        if hex_str in EXCLUDED:
            continue
        
        func = functions.get(faddr)
        if not func:
            continue
        
        score = 0
        details = {
            "addr": faddr,
            "name": func["name"],
            "size": func["size"],
            "num_string_refs": len(str_refs),
            "num_calls": len(func_calls.get(faddr, [])),
            "detection_categories": set(),
            "string_refs": str_refs,
            "call_targets": func_calls.get(faddr, []),
            "callers": [],
        }
        
        # 1. Number of detection string references
        details["num_string_refs"] = len(str_refs)
        score += min(len(str_refs) * 4, 40)
        
        # 2. Category diversity
        for ref in str_refs:
            if ref["string_addr"] in detection_strings:
                cat = detection_strings[ref["string_addr"]]["category"]
                details["detection_categories"].add(cat)
        details["category_diversity"] = len(details["detection_categories"])
        score += len(details["detection_categories"]) * 8
        
        # 3. Function size (medium = sweet spot for hookable functions)
        size = func["size"]
        if 50 <= size <= 500:
            score += 15  # Small, hookable
        elif 500 < size <= 2000:
            score += 12  # Medium
        elif 2000 < size <= 5000:
            score += 8   # Larger
        elif size > 5000:
            score -= 5   # Too large, probably not a single check
        
        # 4. High-priority categories bonus
        high_priority_cats = {"MEMORY_SCAN", "HOOK_DETECT", "REPORT", "INTEGRITY", "MEMORY_PROTECT"}
        for cat in details["detection_categories"]:
            if cat in high_priority_cats:
                score += 15
        
        # 5. Call complexity
        calls = details["call_targets"]
        external_calls = [c for c in calls if c["name"].startswith("IMP:") or c["name"].startswith("PLT:")]
        details["num_external_calls"] = len(external_calls)
        if len(external_calls) > 3:
            score += 10
        elif len(external_calls) > 1:
            score += 5
        
        # 6. Named function bonus (exported/init_array = more important)
        if func["type"] == "init_array":
            score += 8
            details["special"] = "init_array (constructor)"
        elif func["name"].startswith("AnoSDK"):
            score += 5
            details["special"] = "AnoSDK exported"
        
        # Count callers
        caller_count = 0
        for other_faddr, other_calls in func_calls.items():
            for c in other_calls:
                if c["target"] == faddr:
                    caller_count += 1
                    details["callers"].append({"from": other_faddr, "name": functions.get(other_faddr, {}).get("name", "?")})
        details["num_callers"] = caller_count
        if caller_count == 1:
            score += 8  # Single caller = dispatcher pattern
        elif 1 < caller_count <= 5:
            score += 5
        
        details["score"] = score
        scored.append(details)
    
    scored.sort(key=lambda x: -x["score"])
    return scored

# ============================================================
# PHASE 5: Deep Disassembly of Top Candidates
# ============================================================
def disassemble_function(text_content, text_base, func_addr, func_size):
    """Disassemble a specific function and return instructions."""
    md = Cs(CS_ARCH_ARM64, CS_MODE_ARM)
    md.detail = True
    
    offset = func_addr - text_base
    if offset < 0 or offset >= len(text_content):
        return []
    
    code = text_content[offset:offset + min(func_size, 2048)]  # Cap at 2048 bytes
    return list(md.disasm(code, func_addr))

# ============================================================
# MAIN
# ============================================================
def main():
    print("=" * 60)
    print("INDEPENDENT ANTI-CHEAT HOOK DISCOVERY")
    print("Target: libanogs.so (BGMI 4.4.0)")
    print("Method: ELF + String + Cross-Reference + Scoring Analysis")
    print("=" * 60)
    
    # Phase 0: Parse ELF
    print("\n[PHASE 0] Parsing ELF...")
    info = parse_elf(BINARY_PATH)
    
    print(f"  Image base: 0x{info['image_base']:X}")
    print(f"  Sections: {list(info['sections'].keys())}")
    print(f"  Exports: {len(info['exports'])}")
    print(f"  Imports: {len(info['imports'])}")
    print(f"  Dynamic symbols: {len(info['dynsyms'])}")
    print(f"  Relocations: {len(info['relocations'])}")
    
    # Phase 1: String discovery
    print("\n[PHASE 1] Extracting strings...")
    all_strings = find_all_strings(info)
    print(f"  Total strings found: {len(all_strings)}")
    
    detection_strings = filter_detection_strings(all_strings)
    print(f"  Detection-related strings: {len(detection_strings)}")
    
    # Group by category
    cat_count = Counter()
    for v in detection_strings.values():
        cat_count[v["category"]] += 1
    print("  Category breakdown:")
    for cat, cnt in cat_count.most_common():
        print(f"    {cat}: {cnt}")
    
    # Phase 2: Disassemble .text and find references
    print("\n[PHASE 2] Disassembling .text section...")
    text_section = info["sections"].get(".text")
    if not text_section:
        print("  ERROR: .text section not found!")
        return
    
    text_content = text_section["content"]
    text_base = text_section["vaddr"]
    print(f"  .text: base=0x{text_base:X}, size={text_section['size']} bytes")
    
    # Find string references
    print("  Finding ADRP-based string references...")
    refs, instructions = find_adrp_references(text_content, text_base, all_strings, info)
    print(f"  Total ADRP references: {len(refs)}")
    
    # Filter to detection-related refs only
    det_refs = defaultdict(list)
    for ref_addr, ref_list in refs.items():
        for r in ref_list:
            if r["string_addr"] in detection_strings:
                det_refs[ref_addr].append(r)
    print(f"  Detection string references: {sum(len(v) for v in det_refs.values())}")
    
    # Find BL calls
    print("  Finding BL call targets...")
    calls = find_bl_targets(instructions, info)
    print(f"  Total BL calls: {len(calls)}")
    
    # Phase 3: Function boundaries
    print("\n[PHASE 3] Detecting function boundaries...")
    functions = detect_function_boundaries(info, instructions)
    print(f"  Functions detected: {len(functions)}")
    
    # Map refs and calls to functions
    func_strings = map_refs_to_functions(det_refs, functions)
    func_calls = map_calls_to_functions(calls, functions)
    print(f"  Functions with detection string refs: {len(func_strings)}")
    print(f"  Functions with BL calls: {len(func_calls)}")
    
    # Phase 4: Scoring
    print("\n[PHASE 4] Scoring functions...")
    scored = score_functions(func_strings, func_calls, functions, detection_strings)
    print(f"  Functions scored: {len(scored)}")
    
    # Phase 5: Generate report
    print("\n[PHASE 5] Generating detailed report...")
    report = []
    
    def R(line=""):
        report.append(line)
    
    R("=" * 70)
    R("INDEPENDENT ANTI-CHEAT HOOK DISCOVERY REPORT")
    R("Target: libanogs.so (BGMI 4.4.0)")
    R("Method: Pure ELF + Capstone analysis (NO prior offsets used)")
    R("=" * 70)
    R()
    
    R(f"Binary: {BINARY_PATH}")
    R(f"Image Base: 0x{info['image_base']:X}")
    R(f"Total Strings: {len(all_strings)}")
    R(f"Detection Strings: {len(detection_strings)}")
    R(f"Functions Detected: {len(functions)}")
    R(f"Instructions Disassembled: {len(instructions)}")
    R()
    
    R("=" * 70)
    R("SECTION A: ALL DETECTION STRINGS FOUND")
    R("=" * 70)
    R()
    
    for addr in sorted(detection_strings.keys()):
        ds = detection_strings[addr]
        R(f"  0x{addr:X} [{ds['category']}] \"{ds['string']}\"")
    R()
    
    R("=" * 70)
    R("SECTION B: TOP 20 SCORED FUNCTIONS (BEST HOOK CANDIDATES)")
    R("=" * 70)
    R()
    
    for i, sf in enumerate(scored[:20]):
        R(f"{'='*60}")
        R(f"  RANK #{i+1} | Score: {sf['score']}")
        R(f"  Address: 0x{sf['addr']:X}")
        R(f"  Name: {sf['name']}")
        R(f"  Size: {sf['size']} bytes")
        R(f"  Detection string refs: {sf['num_string_refs']}")
        R(f"  Detection categories: {', '.join(sorted(sf['detection_categories']))}")
        R(f"  Category diversity: {sf['category_diversity']}")
        R(f"  External calls: {sf['num_external_calls']}")
        R(f"  Callers: {sf['num_callers']}")
        if sf.get("special"):
            R(f"  SPECIAL: {sf['special']}")
        R()
        
        R(f"  --- String References ---")
        for ref in sf["string_refs"]:
            R(f"    0x{ref['string_addr']:X} [{ref['ref_type']}] \"{ref['string'][:80]}\"")
        R()
        
        if sf["call_targets"]:
            R(f"  --- External/PLT Calls ---")
            for c in sf["call_targets"][:15]:
                R(f"    BL 0x{c['target']:X} ({c['name']})")
            R()
        
        if sf["callers"]:
            R(f"  --- Callers ---")
            for cl in sf["callers"][:10]:
                R(f"    Called from: 0x{cl['from']:X} ({cl['name']})")
            R()
        
        # Disassemble the first 32 instructions of this function
        R(f"  --- Disassembly (first 32 instructions) ---")
        disasm = disassemble_function(text_content, text_base, sf["addr"], sf["size"])
        for j, inst in enumerate(disasm[:32]):
            R(f"    0x{inst.address:08X}  {inst.mnemonic:8s} {inst.op_str}")
        R()
    
    R("=" * 70)
    R("SECTION C: ALL FUNCTIONS WITH DETECTION STRING REFS (COMPLETE LIST)")
    R("=" * 70)
    R()
    
    for i, sf in enumerate(scored):
        if i < 20:
            continue  # Already shown above
        R(f"  #{i+1} Score={sf['score']} | 0x{sf['addr']:X} | {sf['name']} | size={sf['size']} | refs={sf['num_string_refs']} | cats={', '.join(sorted(sf['detection_categories']))}")
    
    R()
    R("=" * 70)
    R("SECTION D: SVC (SYSCALL) LOCATIONS")
    R("=" * 70)
    R()
    
    md = Cs(CS_ARCH_ARM64, CS_MODE_ARM)
    md.detail = True
    svc_locs = []
    for inst in instructions:
        if inst.mnemonic == "svc":
            svc_locs.append(inst)
    
    R(f"  Total SVC instructions: {len(svc_locs)}")
    R()
    
    for inst in svc_locs[:60]:
        # Find containing function
        func_name = "?"
        for faddr in sorted(functions.keys()):
            if faddr <= inst.address < faddr + functions[faddr]["size"]:
                func_name = functions[faddr]["name"]
                break
        R(f"    0x{inst.address:08X}  {inst.mnemonic:8s} {inst.op_str}  (in {func_name})")
    R()
    
    R("=" * 70)
    R("SECTION E: INTERESTING EXTERNAL IMPORTS & THEIR USAGE COUNT")
    R("=" * 70)
    R()
    
    import_counter = Counter()
    for func_addr, call_list in func_calls.items():
        for c in call_list:
            if c["name"].startswith("IMP:") or c["name"].startswith("PLT:"):
                import_counter[c["name"]] += 1
    
    for name, count in import_counter.most_common(40):
        R(f"  {count:4d}x  {name}")
    R()
    
    R("=" * 70)
    R("SECTION F: EXPORTED FUNCTIONS")
    R("=" * 70)
    R()
    for exp in info["exports"]:
        R(f"  0x{exp['addr']:X}  {exp['name']}")
    R()
    
    R("=" * 70)
    R("SECTION G: .init_array (CONSTRUCTOR FUNCTIONS)")
    R("=" * 70)
    R()
    init_array = info["sections"].get(".init_array")
    if init_array:
        data = init_array["content"]
        base = init_array["vaddr"]
        for i in range(0, len(data), 8):
            if i + 8 <= len(data):
                addr = struct.unpack("<Q", data[i:i+8])[0]
                fname = functions.get(addr, {}).get("name", f"FUN_{addr:X}")
                R(f"  [{i//8:2d}] 0x{addr:X}  ({fname})")
                # Show disassembly of constructor
                disasm = disassemble_function(text_content, text_base, addr, 256)
                for inst in disasm[:20]:
                    R(f"       0x{inst.address:08X}  {inst.mnemonic:8s} {inst.op_str}")
                R()
    R()
    
    # Write report
    report_text = "\n".join(report)
    with open(OUTPUT_PATH, "w") as f:
        f.write(report_text)
    
    print(f"\n[!] Report saved to: {OUTPUT_PATH}")
    print(f"[!] Report size: {len(report_text)} bytes, {len(report)} lines")
    
    # Summary
    if scored:
        print(f"\n{'='*50}")
        print("TOP 5 HOOK CANDIDATES:")
        for i, sf in enumerate(scored[:5]):
            print(f"  #{i+1}: 0x{sf['addr']:X} (score={sf['score']}) - {sf['name']}")
            print(f"       Categories: {', '.join(sorted(sf['detection_categories']))}")
            print(f"       Size: {sf['size']} bytes | String refs: {sf['num_string_refs']}")
        print(f"{'='*50}")

if __name__ == "__main__":
    main()
