#!/usr/bin/env python3
"""
64-bit Offset Finder for BGMI 4.4.0 NoRecoil/NoShake patches
============================================================
Strategy: Find FLOAT stores to recoil/shake/FOV/grass/flash addresses
in 64-bit libUE4.so by:
  1. Searching for relevant UE4 property strings
  2. Finding ADRP+ADD/LDR xrefs to those strings
  3. Finding float store instructions near those xrefs
  4. Locating the exact patch points

32-bit patches to convert:
  0x2EE2E34  No Shake (float->0)  
  0x61278E0  No Shake (float->0)
  0x2A90ED8  No Recoil (float->0)
  0x67CF028  iPad View (FOV=266.0)
  0x4EFD5B0  Aimbot (float->0)
  0x322C9C0  120 FPS  
  0x322C5C8  HDR Unlock
  0x6D9EE28  No Grass/Tree
  0x6F3CB00  Flash Speed
"""

import lief
import capstone
import struct
import time

BINARY = "/home/z/my-project/download/bg_extracted/lib/arm64-v8a/libUE4.so"

# UE4 property strings to search for
RECOIL_STRINGS = [
    b"RecoilKickADS",
    b"RecoilKickHIP",
    b"RecoilKick",
    b"AccessoriesVRecoilFactor",
    b"AccessoriesHRecoilFactor",
    b"AnimationKick",
    b"GameDeviationFactor",
    b"CameraShakeInnerRadius",
    b"CameraShakeOuterRadius",
    b"CameraShakeScale",
    b"CameraShake",
    b"FieldOfView",
    b"DesiredFOV",
    b"GrassDensity",
    b"FoliageDensity",
    b"bNoGrass",
    b"Grass",
    b"FlashSpeed",
    b"Flashlight",
    b"AimAssist",
    b"Targeting",
    b"bEnableHDR",
    b"HDR",
    b"bSupportHighDynamicRange",
    b"NoRecoil",
    b"NoShake",
    b"ShootWeapon",
    b"FireWeapon",
    b"WeaponRecoil",
    b"bAimDebugEnabled",
    b"AimOffset",
    b"Spread",
    b"BulletSpeed",
]

def find_strings_in_sections(binary, target_strings):
    """Find target strings and return (string_name, section_name, VA) tuples"""
    results = {}
    for section in binary.sections:
        content = bytes(section.content) if section.content else b''
        for ts in target_strings:
            idx = 0
            while True:
                idx = content.find(ts, idx)
                if idx == -1:
                    break
                va = section.virtual_address + idx
                name = ts.decode('ascii', errors='replace')
                # Check it's a proper null-terminated string (property name)
                if idx > 0 and content[idx-1:idx] == b'\x00' or idx == 0:
                    null = content.find(b'\x00', idx)
                    if null > idx and (null - idx) == len(ts):
                        # Exact match as standalone string
                        if name not in results:
                            results[name] = (va, section.name, idx)
                idx += 1
    return results

def find_adrp_xrefs(text_bytes, text_va, text_size, target_pages):
    """Find ADRP instructions targeting given pages and resolve ADD/LDR"""
    md = capstone.Cs(capstone.CS_ARCH_ARM64, capstone.CS_MODE_ARM)
    md.detail = True
    
    results = []
    adrp_map = {}
    
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
        
        if page in target_pages:
            adrp_map.setdefault(page, []).append((i, rd))
    
    for page, entries in adrp_map.items():
        for (off, rd) in entries:
            for j in range(1, 12):
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
                
                if final and final in target_pages[page]:
                    results.append((nva, ni.mnemonic, ni.op_str, target_pages[page][final]))
    
    return results

def main():
    start = time.time()
    print(f"[*] Loading libUE4.so (64-bit)...")
    binary = lief.parse(BINARY)
    
    text_section = None
    for s in binary.sections:
        if s.name == ".text":
            text_section = s
            break
    text_va = text_section.virtual_address
    text_size = len(text_section.content)
    text_bytes = bytes(text_section.content)
    
    print(f"    .text: VA=0x{text_va:X}, size=0x{text_size:X}")
    
    # Step 1: Find all target strings
    print(f"\n[*] Searching for {len(RECOIL_STRINGS)} target strings...")
    string_results = find_strings_in_sections(binary, RECOIL_STRINGS)
    
    for name, (va, sec, idx) in sorted(string_results.items(), key=lambda x: x[1][0]):
        print(f"  [+] {name:45s} @ 0x{va:08X} ({sec})")
    
    if not string_results:
        print("  [-] No exact matches found, trying substring search...")
        # Try broader search - find strings that CONTAIN our targets
        for section in binary.sections:
            content = bytes(section.content) if section.content else b''
            for ts in RECOIL_STRINGS:
                idx = 0
                while True:
                    idx = content.find(ts, idx)
                    if idx == -1:
                        break
                    va = section.virtual_address + idx
                    name = ts.decode('ascii', errors='replace')
                    print(f"  [~] {name:45s} @ 0x{va:08X} (substring)")
                    string_results[name] = (va, section.name, idx)
                    idx += 1
    
    print(f"\n    Found {len(string_results)} strings")
    
    # Step 2: Build target pages for ADRP scanning
    target_pages = {}
    for name, (va, sec, _) in string_results.items():
        page = va & ~0xFFF
        if page not in target_pages:
            target_pages[page] = {}
        target_pages[page][va] = name
    
    print(f"\n[*] Scanning for ADRP xrefs to {len(target_pages)} pages...")
    print(f"    This scans {text_size/1024/1024:.0f}MB of .text, please wait...")
    
    xrefs = find_adrp_xrefs(text_bytes, text_va, text_size, target_pages)
    
    print(f"    Found {len(xrefs)} cross-references [{time.time()-start:.1f}s]")
    
    for va, mnem, op_str, name in xrefs:
        print(f"  [xref] {name:40s} @ 0x{va:08X} ({mnem} {op_str})")
    
    # Step 3: For each xref, find the function and nearby float operations
    md = capstone.Cs(capstone.CS_ARCH_ARM64, capstone.CS_MODE_ARM)
    md.detail = True
    
    print(f"\n[*] Analyzing xref contexts for float patch points...")
    
    patch_candidates = []
    for va, mnem, op_str, name in xrefs:
        # Disassemble surrounding 50 instructions
        ref_off = va - text_va
        ctx_start = max(0, ref_off - 50 * 4)
        ctx_end = min(text_size, ref_off + 50 * 4)
        
        float_insns = []
        for j in range(ctx_start, ctx_end, 4):
            jva = text_va + j
            jw = text_bytes[j:j+4]
            dec = list(md.disasm(jw, jva))
            if dec:
                ni = dec[0]
                # Look for STR S (store float) instructions
                if ni.mnemonic == "str" and len(ni.operands) >= 2:
                    o = ni.operands
                    if o[0].type == capstone.arm64.ARM64_OP_REG:
                        reg = o[0].reg
                        # S0-S31 are FP registers
                        if capstone.arm64.ARM64_REG_S0 <= reg <= capstone.arm64.ARM64_REG_S31:
                            float_insns.append((jva, ni.mnemonic, ni.op_str))
                # Also FMOV
                if ni.mnemonic == "fmov":
                    float_insns.append((jva, ni.mnemonic, ni.op_str))
                # LDR float
                if ni.mnemonic == "ldr" and len(ni.operands) >= 2:
                    o = ni.operands
                    if o[0].type == capstone.arm64.ARM64_OP_REG:
                        reg = o[0].reg
                        if capstone.arm64.ARM64_REG_S0 <= reg <= capstone.arm64.ARM64_REG_S31:
                            float_insns.append((jva, ni.mnemonic, ni.op_str))
        
        if float_insns:
            patch_candidates.append((name, va, float_insns))
            print(f"\n  [{name}] xref @ 0x{va:08X}")
            for fi_va, fi_mn, fi_op in float_insns[:10]:
                print(f"    float op @ 0x{fi_va:08X}: {fi_mn} {fi_op}")
    
    # Summary
    print(f"\n{'='*70}")
    print(f"[+] ANALYSIS COMPLETE in {time.time()-start:.1f}s")
    print(f"[+] Strings found: {len(string_results)}")
    print(f"[+] Cross-references: {len(xrefs)}")
    print(f"[+] Float operation candidates: {len(patch_candidates)}")
    
    return string_results, xrefs, patch_candidates

if __name__ == "__main__":
    main()
