#!/usr/bin/env python3
"""
Direct Float Constant Search in libUE4.so 64-bit
===================================================
32-bit patches zero float values (00 00 00 00) or set specific floats.
In 64-bit, find equivalent locations by searching for:
  1. Float constant loads (FMOV Sd, #imm / MOV Zi, #imm patterns)
  2. STR S operations near recoil/shake string references
  3. FNAME-based pattern matching

Alternative: Use UE4 FName comparison patterns to find property setter functions.
UE4 uses FName::Compare or operator== with hardcoded FName indexes.

In 64-bit UE4, property setters use FProperty->Offset_Internal to know
WHERE to write the float. We need to find these setter functions.
"""

import lief
import capstone
import struct
import time

BINARY = "/home/z/my-project/download/bg_extracted/lib/arm64-v8a/libUE4.so"

# Known 64-bit string locations (from our previous scan)
KNOWN_STRINGS = {
    "RecoilKickADS":           0x03DB58E7,
    "AnimationKick":          0x03CB527E,
    "CameraShakeScale":        0x03D10A8A,
    "CameraShakeInnerRadius":  0x03E1D961,
    "CameraShake":             0x03C877A6,
    "AccessoriesVRecoilFactor": 0x03DE061E,
    "AccessoriesHRecoilFactor": 0x03E0ABE6,
    "GameDeviationFactor":     0x03E1F329,
    "GrassDensity":            0x03C699FD,
    "Grass":                   0x03DFEB27,
    "BulletSpeed":             0x03C88EE5,
    "FlashSpeed":              0x03DBC4CC,
    "AimAssist":               0x03C7A9F3,
    "ShootWeapon":             0x03C87A7D,
    "FieldOfView":             None,  # Need to find
    "bEnableHDR":              None,  # Need to find
}

def main():
    start = time.time()
    binary = lief.parse(BINARY)
    md = capstone.Cs(capstone.CS_ARCH_ARM64, capstone.CS_MODE_ARM)
    md.detail = True

    # First find missing strings
    print("[*] Searching for additional strings...")
    for sec in binary.sections:
        content = bytes(sec.content) if sec.content else b''
        for extra in [b"FieldOfView", b"bEnableHDR", b"HDR", b"bSupportHDR",
                      b"DesiredFieldOfView", b"RuntimeFieldOfView", b"FOV",
                      b"NoGrass", b"bNoGrass", b"bDisableGrass", b"GrassScaling",
                      b"FlashSpeed", b"bFlashlight", b"FlashIntensity",
                      b"RecoilKick", b"RecoilPitch", b"RecoilYaw",
                      b"Spread", b"BulletSpread"]:
            idx = content.find(extra)
            while idx != -1:
                va = sec.virtual_address + idx
                # Check standalone
                before = content[idx-1] if idx > 0 else 0
                if before == 0:
                    null = content.find(b'\x00', idx)
                    if null > 0 and (null - idx) == len(extra):
                        name = extra.decode()
                        if name not in KNOWN_STRINGS or KNOWN_STRINGS[name] is None:
                            KNOWN_STRINGS[name] = va
                            print(f"  [+] {name:40s} @ 0x{va:08X}")
                idx = content.find(extra, idx + 1)

    # Now search for FIELD_OF_VIEW and HDR strings more broadly
    print("\n[*] All found strings:")
    for name, va in sorted(KNOWN_STRINGS.items(), key=lambda x: x[1] if x[1] else 0):
        if va:
            print(f"  {name:45s} 0x{va:08X}")

    # Step 2: UE4 FName-based search
    # In compiled UE4, property setters don't reference strings directly.
    # Instead, they compare FName indices. The comparison looks like:
    #   LDR Xn, [object, #NameOffset]  ; load FName from object
    #   CMP Wn, #FNameIndex           ; compare with known index
    #
    # But finding FNameIndex requires UE4Dumper output.
    # 
    # ALTERNATIVE: Search for the CHARACTERISTIC byte patterns near offsets.
    # For float zeroing patches, the original 32-bit code does:
    #   STR R0, [R1, #offset]  where R0 = 0.0f
    # In 64-bit equivalent:
    #   STR W0, [X1, #offset]  or  STP Wn, Wm, [Xbase, #off]
    #   Or more commonly in UE4:
    #   FMOV S0, #0.0  (0x1E200800) then STR S0, [Xn, #off]
    
    # Let's try a completely different approach:
    # Search for FMOV S0, #0.0 (which is 0x1E200800 in ARM64)
    # This instruction creates a zero float, commonly used for reset/clear operations
    # If we find it near weapon-related code patterns, those are our targets.
    
    print("\n[*] Searching for FMOV S0, #0.0 (0x1E200800) pattern...")
    print("    (This is the ARM64 way to create a zero float)")
    
    text_section = None
    for s in binary.sections:
        if s.name == ".text":
            text_section = s
            break
    text_va = text_section.virtual_address
    text_size = len(text_section.content)
    text_bytes = bytes(text_section.content)
    
    # Search for FMOV Sd, #0.0 in various register forms
    # FMOV S0, #0.0 = 0x1E200800
    # FMOV S1, #0.0 = 0x1E201800
    # etc.
    fmov_zero = []
    for i in range(0, text_size - 3, 4):
        w = struct.unpack('<I', text_bytes[i:i+4])[0]
        # FMOV Sd, #0.0: 0x1E20_0800 + (d << 5)
        # Pattern: 0001_1110_0010_0000_0000_1ddd_dd00_0000
        # Where d = destination register (0-31)
        if (w & 0xFFFFFC00) == 0x1E200800:
            rd = (w >> 5) & 0x1F
            va = text_va + i
            fmov_zero.append((va, rd))
    
    print(f"    Found {len(fmov_zero)} FMOV Sd, #0.0 instructions")
    
    # This is too many. Let's narrow down.
    # Better approach: search for the specific patterns from 32-bit.
    #
    # The 32-bit patches are at SPECIFIC addresses that write zero floats.
    # In 64-bit, equivalent functions exist but at different offsets.
    # The function NAMES are the same (RecoilKickADS, CameraShakeScale, etc.)
    # but the code is at different VAs.
    #
    # BEST APPROACH: Find the BYTE PATTERN of the surrounding code.
    # For 32-bit: each patch location has unique nearby bytes.
    # We can't directly map 32-bit→64-bit without both binaries.
    #
    # PRACTICAL APPROACH for BGMI modders:
    # 1. Use GameGuardian to find the float values at runtime
    # 2. Or use UE4Dumper/SDK dump to get property offsets
    # 3. Or search for the FName comparison pattern
    
    # Let me try the FName approach:
    # UE4 compiled code checks property FName like this:
    #   ADRL/ADRP + ADD to load a pointer to a static FName struct
    #   LDR from that struct to get the FNameIndex/Number
    #   CMP with current property's FName
    #   
    # The FName struct is 8 bytes: {int32 ComparisonIndex, int32 Number}
    # Static FNames are stored in .data.rel.ro section
    
    print("\n[*] Checking .data.rel.ro for FName structures...")
    data_rel_ro = None
    for s in binary.sections:
        if s.name == ".data.rel.ro":
            data_rel_ro = s
            break
    
    if data_rel_ro:
        content = bytes(data_rel_ro.content)
        print(f"    .data.rel.ro: VA=0x{data_rel_ro.virtual_address:X}, size=0x{len(content):X}")
    
    # FINAL APPROACH: Use the string VA locations to find FNAME entries
    # In UE4, FNames reference strings. The FName table maps:
    #   FNameIndex → CString in a string table
    # The string table is typically a chunked array.
    # But since we found "RecoilKickADS" at 0x03DB58E7 in .rodata,
    # we can search .data.rel.ro for pointers TO that address.
    
    print("\n[*] Searching for FName table entries pointing to our strings...")
    if data_rel_ro:
        content = bytes(data_rel_ro.content)
        base = data_rel_ro.virtual_address
        
        for name, str_va in KNOWN_STRINGS.items():
            if not str_va:
                continue
            # Search for 8-byte pointer to string VA
            target_bytes = struct.pack('<Q', str_va)
            idx = 0
            while True:
                idx = content.find(target_bytes, idx)
                if idx == -1:
                    break
                entry_va = base + idx
                print(f"  [FName ptr] {name:35s} str=0x{str_va:08X} → entry @ 0x{entry_va:08X}")
                # Show surrounding bytes (FName struct)
                start = max(0, idx - 16)
                end = min(len(content), idx + 24)
                chunk = content[start:end]
                hex_str = ' '.join(f'{b:02X}' for b in chunk)
                print(f"             context: {hex_str}")
                idx += 8
    
    # Also search for the string VAs as 32-bit values (FName may use offset encoding)
    print("\n[*] Searching for string offset references in .data.rel.ro...")
    # In UE4, FName may use Get FNameEntry() with an index into a chunk table
    # The chunk table stores {int32 StringOffset, int32 BlockIndex} pairs
    # Let's try searching for the string address as a relative offset
    
    elapsed = time.time() - start
    print(f"\n[+] Analysis complete in {elapsed:.1f}s")
    
    # Print summary of what we know
    print(f"\n{'='*70}")
    print(f"[+] 64-BIT STRING LOCATIONS (CONFIRMED):")
    print(f"{'='*70}")
    for name, va in sorted(KNOWN_STRINGS.items()):
        if va:
            print(f"  {name:45s} 0x{va:08X}")
        else:
            print(f"  {name:45s}  NOT FOUND")

if __name__ == "__main__":
    main()
