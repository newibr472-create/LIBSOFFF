#!/usr/bin/env python3
"""
Phase 2: Find .text xrefs to UE4 property pointer tables
These pointer tables in .data.rel.ro are UProperty arrays.
UE4 code loads from these tables via LDR from GOT or ADRP+LDR.
"""

import lief
import capstone
import struct
import time
import mmap
import os

BINARY = "/home/z/my-project/download/bg_extracted/lib/arm64-v8a/libUE4.so"

# Key pointer table addresses (from Phase 1) - these are in .data.rel.ro
PROPERTY_TABLES = {
    # Weapon Recoil cluster (0xBB8D90 - 0xBB9240)
    "RecoilKickADS":           0xBB8DB8,
    "AnimationKick":           0xBB8DA0,
    "AccessoriesHRecoilFactor": 0xBB9208,
    "AccessoriesVRecoilFactor": 0xBB9238,
    "GameDeviationFactor":     0xBB9190,
    # Camera Shake cluster
    "CameraShakeInnerRadius":  0xBB73C0,
    "CameraShake":             0x86DB30,
    "CameraShakeScale":        0xF0F098,
    # Bullet
    "BulletSpeed":             0x9F1070,
    # Grass
    "GrassDensity":            0x1C6C820,
    # Flash
    "FlashSpeed":              0x18AF088,
    # Aim
    "AimAssist":               0x11F6BA0,
    # FOV
    "FOV":                     0xCF4460,
    # ShootWeapon (many refs, take first few unique)
    "ShootWeapon":             0x89FCD8,
}

# 32-bit patches to convert
PATCHES_32BIT = {
    "0x2EE2E34": ("No Shake (float->0)", 1),
    "0x61278E0": ("No Shake (float->0)", 2),
    "0x2A90ED8": ("No Recoil (float->0)", 3),
    "0x67CF028": ("iPad View FOV=266.0", 4),
    "0x4EFD5B0": ("Aimbot (float->0)", 5),
    "0x322C9C0": ("120 FPS", 6),
    "0x322C5C8": ("HDR Unlock", 7),
    "0x6D9EE28": ("No Grass/Tree", 8),
    "0x6F3CB00": ("Flash Speed", 9),
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

    print(f"[*] .text: VA=0x{text_va:X}, size=0x{text_size:X}")

    # Scan for ADRP instructions targeting pages of our property tables
    print(f"\n[*] Scanning for ADRP xrefs to property tables...")
    
    table_pages = {}
    for name, va in PROPERTY_TABLES.items():
        page = va & ~0xFFF
        table_pages.setdefault(page, []).append((name, va))
    
    adrp_matches = {}  # page -> [(off, rd)]
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
        if page in table_pages:
            adrp_matches.setdefault(page, []).append((i, rd))

    total = sum(len(v) for v in adrp_matches.values())
    print(f"    Found {total} ADRPs targeting {len(adrp_matches)} property table pages")
    
    # Resolve ADRP+ADD/LDR
    print(f"\n[*] Resolving cross-references...")
    xrefs = {}  # name -> [(xref_va, insn)]
    
    for page, entries in adrp_matches.items():
        targets = {va: name for name, va in table_pages[page]}
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

                if final and final in targets:
                    name = targets[final]
                    xrefs.setdefault(name, []).append((nva, f"{ni.mnemonic} {ni.op_str}"))
    
    print(f"\n{'='*70}")
    print(f"[+] PROPERTY TABLE CROSS-REFERENCES")
    print(f"{'='*70}")
    
    for name in sorted(xrefs.keys()):
        entries = xrefs[name]
        print(f"\n  {name}:")
        for va, insn in entries[:5]:
            print(f"    xref @ 0x{va:08X}: {insn}")
    
    # Now find float operations near each xref
    print(f"\n{'='*70}")
    print(f"[+] FLOAT OPERATIONS NEAR XREFS (PATCH CANDIDATES)")
    print(f"{'='*70}")
    
    patch_points = {}
    for name, entries in xrefs.items():
        for xref_va, _ in entries:
            ref_off = xref_va - text_va
            # Search ±200 instructions for STR S / FMOV / LDR S
            search_start = max(0, ref_off - 200 * 4)
            search_end = min(text_size, ref_off + 200 * 4)
            
            float_ops = []
            for j in range(search_start, search_end, 4):
                jva = text_va + j
                jw = text_bytes[j:j+4]
                dec = list(md.disasm(jw, jva))
                if not dec:
                    continue
                ni = dec[0]
                is_float = False
                is_store = False
                is_load = False
                
                if ni.mnemonic == "str" and len(ni.operands) >= 2:
                    o = ni.operands
                    if o[0].type == capstone.arm64.ARM64_OP_REG:
                        reg = o[0].reg
                        if capstone.arm64.ARM64_REG_S0 <= reg <= capstone.arm64.ARM64_REG_S31:
                            is_float = True
                            is_store = True
                
                if ni.mnemonic == "ldr" and len(ni.operands) >= 2:
                    o = ni.operands
                    if o[0].type == capstone.arm64.ARM64_OP_REG:
                        reg = o[0].reg
                        if capstone.arm64.ARM64_REG_S0 <= reg <= capstone.arm64.ARM64_REG_S31:
                            is_float = True
                            is_load = True
                
                if ni.mnemonic == "fmov" or ni.mnemonic == "fmul" or ni.mnemonic == "fadd" or ni.mnemonic == "fsub":
                    is_float = True
                
                if is_float:
                    float_ops.append((jva, ni.mnemonic, ni.op_str, is_store, is_load))
            
            if float_ops:
                # Find function entry for this xref
                func_entry = find_func_entry(text_bytes, text_va, text_size, xref_va)
                stores = [(v, m, o) for v, m, o, s, l in float_ops if s]
                loads = [(v, m, o) for v, m, o, s, l in float_ops if l]
                others = [(v, m, o) for v, m, o, s, l in float_ops if not s and not l]
                
                patch_points[name] = {
                    'xref': xref_va,
                    'func': func_entry,
                    'stores': stores[:5],
                    'loads': loads[:5],
                    'others': others[:5],
                }
    
    # Output PATCH_LIB recommendations
    print(f"\n{'='*70}")
    print(f"[+] RECOMMENDED 64-BIT PATCH_LIB HOOKS")
    print(f"{'='*70}")
    
    for name, info in sorted(patch_points.items()):
        print(f"\n  --- {name} ---")
        if info['func']:
            print(f"  Function entry: 0x{info['func']:X}")
        print(f"  Xref: 0x{info['xref']:X}")
        if info['stores']:
            print(f"  Float STORES (best patch targets):")
            for v, m, o in info['stores']:
                orig = ' '.join(f'{b:02X}' for b in text_bytes[v-text_va:v-text_va+4])
                print(f"    0x{v:08X}: {orig}  {m} {o}")
                print(f"    PATCH_LIB(\"libUE4.so\", \"0x{v:X}\", \"00 00 00 00\");  // Zero float store")
    
    print(f"\n[+] Done in {time.time()-start:.1f}s")

def find_func_entry(text_bytes, text_va, text_size, ref_va):
    ref_off = ref_va - text_va
    for back in range(4, 4096, 4):
        off = ref_off - back
        if off < 0:
            break
        w = struct.unpack('<I', text_bytes[off:off+4])[0]
        # STP x29, x30, [sp, #imm]! (pre-index)
        if (w & 0x7FC00000) == 0x29800000 and (w >> 31) & 1:
            opc = (w >> 22) & 3
            if opc == 2:
                rt = w & 0x1F
                rt2 = (w >> 10) & 0x1F
                rn = (w >> 5) & 0x1F
                if rn == 31 and rt in (29, 30) and rt2 in (29, 30) and rt != rt2:
                    return text_va + off
        # SUB sp, sp
        if (w & 0xFF0003FF) == 0xD10003FF:
            return text_va + off
        # PACIASP
        if w == 0xD503237F:
            return text_va + off
    return None

if __name__ == "__main__":
    main()
