#!/usr/bin/env python3
"""
Final 64-bit Patch Finder - Float Constant & Pattern Matching
=============================================================
Since UE4 uses FName indirection and ADRP won't find xrefs to .data.rel.ro,
we use FLOAT CONSTANT search to find the EXACT patch points.

Each 32-bit patch does one of:
  1. Zero a float (00 00 00 00) → Find STR W0, [Xn, #off] with W0=0
  2. Set FOV to 266.0 → Find the constant 266.0 (0x43850000 as float)
  3. Return 120 FPS → MOV W0, #120; RET
  4. Return 5 (HDR) → MOV W0, #5; RET

We search for:
  - Float 266.0 = 0x43850000 (IEEE 754)
  - Float constants used in FOV, recoil, shake calculations
  - MOV W0, #120 / MOV W0, #5 patterns near FPS functions
"""

import lief
import capstone
import struct
import time
import mmap
import os

BINARY = "/home/z/my-project/download/bg_extracted/lib/arm64-v8a/libUE4.so"

# Float constants to search for
FLOAT_TARGETS = {
    0x43850000: "266.0 (iPad FOV)",
    0x42C80000: "100.0 (default FOV)",
    0x42700000: "60.0 (default FPS)",
    0x42F00000: "120.0 (FPS target)",
    0x40A00000: "5.0 (HDR level)",
    0x43C80000: "400.0 (max FOV)",
    0x41200000: "10.0",
    0x41800000: "16.0",
    0x42480000: "50.0",
    0x428C0000: "70.0",
    0x42A00000: "80.0",
    0x42B40000: "90.0",
    0x41A00000: "20.0",
    0x3F800000: "1.0",
    0x40000000: "2.0",
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

    # Step 1: Search for MOV W0, #120 and MOV W0, #5 near known FPS function
    # FPS function: 0xBC2BF38 - 0xBC2C074
    # 120 FPS: MOV W0, #120 = 0x52800780
    # RET:       C0 03 5F D6
    # HDR: MOV W0, #5 = 0x528000A0
    
    print("[*] Step 1: Search for MOV W0, #120 (0x52800780) near FPS function area")
    print(f"    FPS function: 0xBC2BF38 - 0xBC2C074")
    
    # Search broader area around FPS function
    search_start = 0xBC20000 - text_va
    search_end = 0xBC30000 - text_va
    for i in range(search_start, min(search_end, text_size - 3), 4):
        w = struct.unpack('<I', text_bytes[i:i+4])[0]
        if w == 0x52800780:  # MOV W0, #120
            va = text_va + i
            print(f"  MOV W0, #120 @ 0x{va:08X}")
            # Show context
            for j in range(-4, 8):
                ctx_off = i + j * 4
                if 0 <= ctx_off < text_size - 3:
                    ctx_w = struct.unpack('<I', text_bytes[ctx_off:ctx_off+4])[0]
                    ctx_va = text_va + ctx_off
                    ctx_h = ' '.join(f'{b:02X}' for b in text_bytes[ctx_off:ctx_off+4])
                    dec = list(md.disasm(text_bytes[ctx_off:ctx_off+4], ctx_va))
                    if dec:
                        marker = " <<<" if j == 0 else ""
                        print(f"    0x{ctx_va:08X}: {ctx_h}  {dec[0].mnemonic} {dec[0].op_str}{marker}")

    print("\n[*] Step 2: Search for MOV W0, #5 (0x528000A0) for HDR")
    for i in range(search_start, min(search_end, text_size - 3), 4):
        w = struct.unpack('<I', text_bytes[i:i+4])[0]
        if w == 0x528000A0:  # MOV W0, #5
            va = text_va + i
            print(f"  MOV W0, #5 @ 0x{va:08X}")
    
    # Step 3: Search for float constant 266.0 (FOV=266)
    # In ARM64, 266.0 can be loaded as:
    # a) FMOV S0, #266.0 (if encodable as immediate)
    # b) LDR from literal pool (most common for arbitrary floats)
    # c) MOVZ/MOVK construction
    
    print(f"\n[*] Step 3: Search for float 266.0 (0x43850000) in .text literal pools")
    float_266 = struct.pack('<f', 266.0)
    print(f"    266.0 bytes: {float_266.hex()}")
    
    # Search in .text (literal pools are embedded in .text between functions)
    count = 0
    for i in range(0, text_size - 3, 4):
        chunk = text_bytes[i:i+4]
        if chunk == float_266:
            va = text_va + i
            # Check if this is in a literal pool (between functions, aligned)
            # Literal pools are typically after RET instructions
            if i > 0:
                prev_w = struct.unpack('<I', text_bytes[i-4:i])[0]
                # RET = 0xD65F03C0
                if prev_w == 0xD65F03C0:
                    print(f"  Float 266.0 @ 0x{va:08X} (after RET - likely literal pool)")
                    count += 1
    print(f"    Found {count} candidates")
    
    # Step 4: Search for float constants in .rodata
    print(f"\n[*] Step 4: Search for float 266.0 in .rodata")
    rodata = None
    for s in binary.sections:
        if s.name == ".rodata":
            rodata = s
            break
    if rodata:
        rdata = bytes(rodata.content)
        idx = 0
        while True:
            idx = rdata.find(float_266, idx)
            if idx == -1:
                break
            va = rodata.virtual_address + idx
            print(f"  Float 266.0 @ 0x{va:08X} (.rodata)")
            # Show surrounding floats
            for off in range(-16, 20, 4):
                f_idx = idx + off
                if 0 <= f_idx < len(rdata) - 3:
                    f_bytes = rdata[f_idx:f_idx+4]
                    f_val = struct.unpack('<f', f_bytes)[0]
                    f_va = rodata.virtual_address + f_idx
                    marker = " <<<" if off == 0 else ""
                    print(f"    0x{f_va:08X}: {' '.join(f'{b:02X}' for b in f_bytes)}  {f_val:.1f}{marker}")
            idx += 4
    
    # Step 5: Broad search for ALL float targets
    print(f"\n[*] Step 5: Search .rodata for float constants (FOV, FPS related)")
    for float_hex, desc in FLOAT_TARGETS.items():
        float_bytes = struct.pack('<I', float_hex)
        if rodata:
            idx = 0
            found = 0
            while found < 3:
                idx = rdata.find(float_bytes, idx)
                if idx == -1:
                    break
                va = rodata.virtual_address + idx
                print(f"  {desc:30s} (0x{float_hex:08X}) @ 0x{va:08X}")
                found += 1
                idx += 4
    
    # Step 6: Search for MOV W0, #0 (for zeroing) near weapon-related code
    # MOV W0, #0 = 0x52800000
    # MOV W1, #0 = 0x52800020
    # STR W0, [Xn, #off] patterns
    print(f"\n[*] Step 6: Check 120FPS function siblings for HDR")
    
    # The 32-bit has 120FPS at 0x322C9C0 and HDR at 0x322C5C8
    # These are close together (0x3F0 apart). In 64-bit, we expect similar.
    # Our FPS function is at 0xBC2BF38. Let's check what's nearby.
    
    print(f"    Scanning 0xBC2B000 - 0xBC2D000 for function entries...")
    for i in range(0xBC2B000 - text_va, 0xBC2D000 - text_va, 4):
        w = struct.unpack('<I', text_bytes[i:i+4])[0]
        va = text_va + i
        # Check for MOV W0, #5 pattern (HDR)
        if w == 0x528000A0:
            print(f"  MOV W0, #5 @ 0x{va:08X} (possible HDR)")
        # MOV W0, #120
        if w == 0x52800780:
            print(f"  MOV W0, #120 @ 0x{va:08X} (possible FPS)")
        # Check for function prologues
        if (w & 0x7FC00000) == 0x29800000 and (w >> 31) & 1:
            opc = (w >> 22) & 3
            if opc == 2:
                rt = w & 0x1F
                rt2 = (w >> 10) & 0x1F
                rn = (w >> 5) & 0x1F
                if rn == 31 and rt in (29, 30) and rt2 in (29, 30) and rt != rt2:
                    dec = list(md.disasm(text_bytes[i:i+4], va))
                    if dec:
                        print(f"  Func entry @ 0x{va:08X}: {dec[0].mnemonic} {dec[0].op_str}")
    
    print(f"\n[+] Done in {time.time()-start:.1f}s")

if __name__ == "__main__":
    main()
