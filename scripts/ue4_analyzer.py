#!/usr/bin/env python3
"""
UE4 libUE4.so Analyzer + Bypass Generator - BGMI 4.4.0
Analyzes arm64-v8a libUE4.so and generates PATCH_LIB bypass hooks.
Run inside GitHub Codespace with libUE4.so available.
"""

import struct
import sys
import os
import json
from collections import defaultdict

try:
    from capstone import Cs, CS_ARCH_ARM64, CS_MODE_ARM
    md = Cs(CS_ARCH_ARM64, CS_MODE_ARM)
    HAS_CAPSTONE = True
except ImportError:
    HAS_CAPSTONE = False
    print("[WARN] Install capstone: pip install capstone")

# ═══════════════════════════════════════════════════════════════
# CONFIG
# ═══════════════════════════════════════════════════════════════

LIB_PATH = os.environ.get("LIB_PATH", "/workspaces/LIBSOFFF/binaries/arm64-v8a/libUE4.so")
OUTPUT_DIR = os.environ.get("OUTPUT_DIR", "/workspaces/LIBSOFFF/analysis")
HOOKS_DIR = os.environ.get("HOOKS_DIR", "/workspaces/LIBSOFFF/hooks")

# ELF section boundaries
TEXT_START = 0x05602000
TEXT_END   = 0x0C4DFAD0
RODATA_START = 0x0347F2C0
RODATA_END   = 0x05601FFF

# ═══════════════════════════════════════════════════════════════
# STRING TARGETS
# ═══════════════════════════════════════════════════════════════

TARGETS = {
    "recoil": [
        b"RecoilKickADS", b"RecoilRecoverADS", b"RecoilKick",
        b"AccessoriesVRecoilFactor", b"AccessoriesHRecoilFactor",
        b"GameDeviationFactor", b"AnimationKick", b"AnimationKickDecay",
        b"RecoilPattern", b"BulletSpread", b"SpreadFactor",
        b"RecoilMagnitude", b"RecoilAngle", b"FirstShotRecoil",
    ],
    "shake": [
        b"CameraShake", b"CameraShakeScale", b"CameraShakeInnerRadius",
        b"CameraShakeFalloff", b"PlayCameraShake", b"StopCameraShake",
        b"ShakeAmplitude", b"ShakeFrequency", b"CameraAnimInst",
        b"WeaponShake", b"AimingShake", b"BreathShake",
    ],
    "fps": [
        b"FrameRateLimit", b"MaxFPS", b"bSmoothFrameRate",
        b"SmoothedFrameRateRange", b"FixedFrameRate", b"UseFixedFrameRate",
        b"CustomTimeStep", b"MaxTickRate", b"NetServerMaxTickRate",
        b"t.MaxFPS", b"FrameRateCap", b"TargetFrameRate",
    ],
    "fov": [
        b"FieldOfView", b"FOVAngle", b"DefaultFOV", b"DesiredFOV",
        b"CameraFOV", b"AimFOV", b"ScopeFOV", b"CurrentFOV",
        b"FOVMultiplier", b"BaseFOV", b"MinFOV", b"MaxFOV",
    ],
    "speed": [
        b"MaxWalkSpeed", b"MaxRunSpeed", b"MaxSprintSpeed",
        b"MovementSpeed", b"CharacterMovement", b"MaxAcceleration",
        b"SpeedMultiplier", b"BaseSpeed", b"CrouchSpeed",
        b"SwimSpeed", b"FlySpeed", b"VehicleSpeed",
    ],
    "grass": [
        b"GrassType", b"LandscapeGrass", b"FoliageType",
        b"GrassDensity", b"GrassDistance", b"CullDistance",
        b"FoliageDensity", b"TreeDensity", b"VegetationType",
        b"bEnableGrass", b"GrassLOD", b"FoliageLOD",
    ],
    "aimbot": [
        b"GetBoneLocation", b"GetSocketLocation", b"BoneArray",
        b"SkeletalMesh", b"GetBoneTransform", b"BoneSpaceTransform",
        b"head", b"neck_01", b"spine_03", b"RootComponent",
        b"GetActorLocation", b"GetActorRotation", b"LineTrace",
        b"K2_SetActorRotation", b"SetControlRotation",
    ],
    "hdr": [
        b"HDR", b"Tonemapper", b"FilmSlope", b"FilmToe",
        b"PostProcess", b"AutoExposure", b"Bloom",
        b"QualityLevel", b"ScalabilityGroup", b"ScreenPercentage",
    ],
}

# ═══════════════════════════════════════════════════════════════
# HELPERS
# ═══════════════════════════════════════════════════════════════

def find_all(data, pattern):
    results = []
    idx = 0
    while True:
        idx = data.find(pattern, idx)
        if idx == -1:
            break
        results.append(idx)
        idx += 1
    return results

def find_adrp_add_refs(data, target_addr, start, end):
    """Find ADRP+ADD pairs referencing target_addr"""
    refs = []
    page = target_addr & ~0xFFF
    off12 = target_addr & 0xFFF
    
    for off in range(start, min(end, len(data) - 4), 4):
        instr = struct.unpack_from('<I', data, off)[0]
        if (instr & 0xFF800000) != 0x91000000:
            continue
        imm12 = (instr >> 10) & 0xFFF
        if imm12 != off12:
            continue
        if off < 4:
            continue
        prev = struct.unpack_from('<I', data, off - 4)[0]
        if (prev & 0x9F000000) != 0x90000000:
            continue
        rd = prev & 0x1F
        rn = (instr >> 5) & 0x1F
        if rd != rn:
            continue
        immhi = (prev >> 5) & 0x7FFFF
        immlo = (prev >> 29) & 0x3
        imm = (immhi << 2) | immlo
        if imm & (1 << 20):
            imm -= (1 << 21)
        result = ((off - 4) & ~0xFFF) + (imm << 12)
        if result == page:
            refs.append(off - 4)
    return refs

def find_func_start(data, addr):
    """Walk back to find STP X29,X30 prologue"""
    for off in range(addr, max(addr - 0x2000, TEXT_START), -4):
        instr = struct.unpack_from('<I', data, off)[0]
        # STP X29, X30, [SP, #-offset]!
        if (instr & 0xFFE00000) == 0xA9800000:
            r1 = instr & 0x1F
            r2 = (instr >> 10) & 0x1F
            if r1 == 29 and r2 == 30:
                return off
        # STP with negative offset (pre-index)
        if (instr & 0xFFC003E0) == 0xA98003E0:
            r2 = (instr >> 10) & 0x1F
            if r2 == 30:
                return off
    return addr

def disasm(data, addr, n=5):
    if not HAS_CAPSTONE:
        return ""
    chunk = data[addr:addr + n * 4]
    lines = []
    for i in md.disasm(chunk, addr):
        lines.append(f"  0x{i.address:08X}: {i.mnemonic} {i.op_str}")
    return "\n".join(lines)

# ═══════════════════════════════════════════════════════════════
# MAIN
# ═══════════════════════════════════════════════════════════════

def main():
    print("=" * 70)
    print("  UE4 ANALYZER + BYPASS GENERATOR")
    print("  BGMI 4.4.0 | libUE4.so | arm64-v8a")
    print("=" * 70)
    
    if not os.path.exists(LIB_PATH):
        print(f"\n[!] File not found: {LIB_PATH}")
        print("    Set LIB_PATH env variable or run from repo root")
        sys.exit(1)
    
    with open(LIB_PATH, 'rb') as f:
        data = f.read()
    
    print(f"\n[+] Loaded {len(data)/1024/1024:.1f} MB")
    print(f"[+] .text:   0x{TEXT_START:08X} - 0x{TEXT_END:08X} ({(TEXT_END-TEXT_START)//1024//1024} MB)")
    print(f"[+] .rodata: 0x{RODATA_START:08X} - 0x{RODATA_END:08X}")
    
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    os.makedirs(HOOKS_DIR, exist_ok=True)

    # ═══════════════════════════════════════════════════════════
    # PHASE 1: String Discovery
    # ═══════════════════════════════════════════════════════════
    print(f"\n{'═' * 70}")
    print("PHASE 1: STRING DISCOVERY")
    print("═" * 70)
    
    all_strings = {}
    for cat, patterns in TARGETS.items():
        all_strings[cat] = {}
        for pat in patterns:
            locs = find_all(data, pat)
            rodata = [l for l in locs if RODATA_START <= l <= RODATA_END]
            if rodata:
                all_strings[cat][pat.decode()] = rodata[0]
    
    total_found = sum(len(v) for v in all_strings.values())
    print(f"\n[+] Found {total_found} strings in .rodata\n")
    for cat, strings in all_strings.items():
        if strings:
            print(f"  [{cat.upper()}] {len(strings)} strings")
            for name, addr in sorted(strings.items(), key=lambda x: x[1])[:5]:
                print(f"    0x{addr:08X}: {name}")
            if len(strings) > 5:
                print(f"    ... +{len(strings)-5} more")

    # ═══════════════════════════════════════════════════════════
    # PHASE 2: Cross-Reference Analysis  
    # ═══════════════════════════════════════════════════════════
    print(f"\n{'═' * 70}")
    print("PHASE 2: CROSS-REFERENCE ANALYSIS (this takes time...)")
    print("═" * 70)
    
    xrefs = {}
    functions = {}
    
    for cat, strings in all_strings.items():
        xrefs[cat] = {}
        for name, str_addr in strings.items():
            sys.stdout.write(f"\r  Scanning [{cat}]: {name:<30}")
            sys.stdout.flush()
            refs = find_adrp_add_refs(data, str_addr, TEXT_START, TEXT_END)
            if refs:
                xrefs[cat][name] = refs
                for ref in refs[:3]:
                    fs = find_func_start(data, ref)
                    if fs not in functions:
                        functions[fs] = {'cat': cat, 'string': name, 'ref': ref}
    
    total_xrefs = sum(len(v) for v in xrefs.values())
    print(f"\n\n[+] Found {total_xrefs} xrefs → {len(functions)} unique functions\n")
    
    for cat, refs in xrefs.items():
        if refs:
            print(f"  [{cat.upper()}]")
            for name, reflist in list(refs.items())[:5]:
                for r in reflist[:1]:
                    fs = find_func_start(data, r)
                    print(f"    {name}: func@0x{fs:X} (ref@0x{r:X})")

    # ═══════════════════════════════════════════════════════════
    # PHASE 3: Generate Bypass Hooks
    # ═══════════════════════════════════════════════════════════
    print(f"\n{'═' * 70}")
    print("PHASE 3: BYPASS GENERATION")
    print("═" * 70)
    
    hook_lines = []
    hook_lines.append("/*")
    hook_lines.append(" * AUTO-GENERATED UE4 BYPASS - BGMI 4.4.0 arm64-v8a")
    hook_lines.append(" * Generated by ue4_analyzer.py on Codespace")
    hook_lines.append(" */\n")
    
    patch_count = 0
    
    for cat, refs in xrefs.items():
        if not refs:
            continue
        hook_lines.append(f"\n// ═══ {cat.upper()} ═══")
        
        for name, reflist in refs.items():
            for ref in reflist[:2]:
                fs = find_func_start(data, ref)
                
                if cat in ('recoil', 'shake'):
                    # Return 0.0f (MOVI V0, #0; RET)
                    patch = "0F 04 00 0E C0 03 5F D6"
                    desc = f"return 0.0f"
                elif cat == 'fps':
                    # Return 120 (MOV W0, #120; RET)
                    patch = "00 0F 80 52 C0 03 5F D6"
                    desc = "return 120"
                elif cat == 'fov':
                    # Skip - need runtime hook, not static patch
                    patch = None
                    desc = "HOOK_POINT (runtime)"
                elif cat == 'speed':
                    # Skip - dangerous to patch statically
                    patch = None
                    desc = "HOOK_POINT (runtime)"
                elif cat == 'grass':
                    # Just RET (skip render)
                    patch = "C0 03 5F D6"
                    desc = "skip render (RET)"
                elif cat == 'aimbot':
                    patch = None
                    desc = "HOOK_POINT (bone read)"
                elif cat == 'hdr':
                    # Return 5 (max quality)
                    patch = "A0 00 80 52 C0 03 5F D6"
                    desc = "return 5 (max)"
                else:
                    patch = None
                    desc = "HOOK_POINT"
                
                if patch:
                    hook_lines.append(
                        f'PATCH_LIB("libUE4.so", "0x{fs:X}", "{patch}");'
                        f'  // {name} → {desc}'
                    )
                    patch_count += 1
                else:
                    hook_lines.append(
                        f'// HOOK: 0x{fs:X}  // {name} → {desc}'
                    )
    
    hook_lines.append(f"\n// Total: {patch_count} patches")
    
    hook_file = os.path.join(HOOKS_DIR, "UE4_AutoBypass_arm64.h")
    with open(hook_file, 'w') as f:
        f.write("\n".join(hook_lines))
    print(f"\n[+] Generated: {hook_file} ({patch_count} patches)")

    # ═══════════════════════════════════════════════════════════
    # PHASE 4: Save Analysis JSON
    # ═══════════════════════════════════════════════════════════
    
    analysis = {
        'binary': os.path.basename(LIB_PATH),
        'size': len(data),
        'arch': 'AARCH64',
        'strings': {cat: {k: f"0x{v:X}" for k, v in s.items()} 
                    for cat, s in all_strings.items()},
        'xrefs': {cat: {k: [f"0x{r:X}" for r in refs] for k, refs in x.items()} 
                  for cat, x in xrefs.items()},
        'functions': [
            {'addr': f"0x{k:X}", 'cat': v['cat'], 'string': v['string']}
            for k, v in sorted(functions.items())
        ],
        'patch_count': patch_count,
    }
    
    json_file = os.path.join(OUTPUT_DIR, "ue4_analysis.json")
    with open(json_file, 'w') as f:
        json.dump(analysis, f, indent=2)
    print(f"[+] Analysis saved: {json_file}")
    
    # ═══════════════════════════════════════════════════════════
    # PHASE 5: Function Disassembly Report
    # ═══════════════════════════════════════════════════════════
    
    if HAS_CAPSTONE:
        report_file = os.path.join(OUTPUT_DIR, "function_disasm.txt")
        with open(report_file, 'w') as f:
            f.write("UE4 Key Functions Disassembly\n")
            f.write("=" * 60 + "\n\n")
            for addr, info in sorted(functions.items())[:50]:
                f.write(f"FUNC 0x{addr:08X} [{info['cat']}] {info['string']}\n")
                f.write(disasm(data, addr, 10) + "\n\n")
        print(f"[+] Disassembly: {report_file}")
    
    print(f"\n{'═' * 70}")
    print(f"  DONE! {total_found} strings → {total_xrefs} xrefs → {patch_count} patches")
    print(f"{'═' * 70}\n")

if __name__ == "__main__":
    main()
