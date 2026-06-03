#!/usr/bin/env python3
"""
UE4 Property Setter Finder - Advanced 64-bit Analysis
=====================================================
Uses UE4's internal FName chunk table to find property setter functions.

UE4 internally stores FNames as {ComparisonIndex, Number} pairs.
The FName chunk table stores string data in blocks. Each block has:
  - StringOffset (into block data)
  - BlockIndex
  - String length + flags

When UE4 code accesses a UProperty by name, it:
  1. Loads a FName from a global/static variable
  2. Compares it with the current property name (CMP or TBZ/TBNZ)
  3. If match, reads/writes the property at the computed offset

We can find this by:
  A) Searching for LDR Xn, [page, #off] where the loaded value
     is a pointer into a known .rodata address
  B) Then finding comparison instructions nearby

ACTUAL PRACTICAL APPROACH:
  Since Ghidra is available on Codespace and UE4 uses FName indirection,
  the FASTEST way to find 64-bit patches is:
  
  1. Search .rodata for each property string
  2. Find ALL references to that string's page in .text  
  3. For each reference, check context for float operations
  4. The function that contains float ops + string ref = setter/getter

  But ADRP only found 1 xref (Grass). This means UE4 compiled code
  uses FNAME COMPARISON (not string loading) for property access.
  
  UE4's compiled approach:
    LDR X0, =FName_for_RecoilKickADS   (from .data.rel.ro literal pool)
    LDR X1, [Object, #SuperStruct]
    LDR W2, [X1, #CurrentFNameIndex]
    CMP W2, W0  ; compare FName index
    B.EQ set_value

  We need to find FNames in .data.rel.ro or .data section.

NEW STRATEGY: Search ALL sections for our string pointers.
In UE4, there's a TNameEntryArray (FNamePool) that stores all names.
Each entry: {int32 StringOffset, int16 BlockIndex, int16 StringLen, uint32 Flags}
The actual string data is in a separate block.

We'll search for entries where StringOffset + block_data_base points to our strings.
"""

import lief
import struct
import time
import mmap
import os

BINARY = "/home/z/my-project/download/bg_extracted/lib/arm64-v8a/libUE4.so"

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
    "FOV":                     0x03D7721F,
}

def main():
    start = time.time()
    
    # Memory-map the binary for fast scanning
    fd = os.open(BINARY, os.O_RDONLY)
    mm = mmap.mmap(fd, 0, access=mmap.ACCESS_READ)
    fsize = os.fstat(fd).st_size
    print(f"[*] Mapped {fsize/1024/1024:.0f}MB binary")
    
    binary = lief.parse(BINARY)
    
    # Get section info
    sections = {}
    for s in binary.sections:
        sections[s.name] = {
            'va': s.virtual_address,
            'offset': s.offset,
            'size': s.size,
        }
    
    # Step 1: Search ENTIRE binary for any references to our string addresses
    # as 64-bit pointers or 32-bit offsets
    print(f"\n[*] Scanning entire binary for references to {len(KNOWN_STRINGS)} strings...")
    
    str_addrs = {va: name for name, va in KNOWN_STRINGS.items()}
    
    # Create a set of all string addresses for fast lookup
    target_bytes_map = {}
    for name, va in KNOWN_STRINGS.items():
        # 8-byte LE pointer
        ptr8 = struct.pack('<Q', va)
        # 4-byte LE (possible index/offset)
        ptr4 = struct.pack('<I', va)
        target_bytes_map.setdefault(ptr8, []).append((name, va, 'ptr64'))
        target_bytes_map.setdefault(ptr4, []).append((name, va, 'ptr32'))
    
    for pattern, entries in target_bytes_map.items():
        idx = 0
        while True:
            idx = mm.find(pattern, idx)
            if idx == -1:
                break
            for name, va, ptype in entries:
                # Map file offset to VA
                ref_va = find_va_for_offset(idx, sections)
                if ref_va:
                    print(f"  [REF] {name:35s} (0x{va:08X}) → ref @ file_off=0x{idx:X} VA=0x{ref_va:08X} [{ptype}]")
            idx += len(pattern)
    
    # Step 2: Search for string address as high 32 bits of a 64-bit value
    # (UE4 may store block_index in low 32, string VA or chunk offset in high 32)
    print(f"\n[*] Searching for string addresses as 32-bit halves...")
    for name, va in KNOWN_STRINGS.items():
        hi = struct.pack('<I', va)  # High 32 bits
        lo = struct.pack('<I', 0)  # Low 32 bits = 0
        full = lo + hi  # LE: low first, then high
        idx = 0
        while True:
            idx = mm.find(full, idx)
            if idx == -1:
                break
            ref_va = find_va_for_offset(idx, sections)
            if ref_va:
                print(f"  [HALF] {name:35s} → ref @ VA=0x{ref_va:08X}")
            idx += 8
    
    # Step 3: Try finding FNameEntry structures
    # In UE4, FNamePool stores entries as:
    #   struct FNameEntry {
    #     int32 StringOffset;  // offset into chunk string block
    #     int16 BlockIndex;    // which block
    #     int16 StringLen;     // string length
    #     uint32 Flags;        // 0x200 = UTF8
    #   }
    # StringOffset + block_data_ptr → actual string
    
    # But we don't know the block data address. However, if the string
    # is at 0x03DB58E7 and the block data is contiguous in .rodata,
    # then StringOffset = 0x03DB58E7 - block_data_start
    
    # Alternative: search for string length as an FNameEntry marker
    print(f"\n[*] Searching for FName entries by string length...")
    for name, va in KNOWN_STRINGS.items():
        slen = len(name)
        # Search for int16 = slen in a context that could be FNameEntry
        slen_bytes = struct.pack('<h', slen)
        idx = 0
        count = 0
        while count < 5:
            idx = mm.find(slen_bytes, idx)
            if idx == -1:
                break
            # Check if this could be part of FNameEntry
            # Bytes: [4: StringOffset][2: BlockIndex][2: StringLen][4: Flags]
            # The StringLen should be at idx, BlockIndex at idx-2
            if idx >= 6:
                # Read surrounding structure
                entry_data = mm[idx-6:idx+10]
                str_off = struct.unpack('<I', entry_data[0:4])[0]
                block_idx = struct.unpack('<h', entry_data[4:6])[0]
                flags = struct.unpack('<I', entry_data[8:12])[0]
                if flags == 0x200 and 0 <= block_idx < 100:  # UTF8 flag, reasonable block
                    ref_va = find_va_for_offset(idx - 6, sections)
                    # Verify: does block_data + str_off point to our string?
                    # We can't verify without knowing block_data, but flags=0x200 is a good hint
                    if ref_va:
                        print(f"  [FName?] {name:35s} len={slen} off=0x{str_off:X} block={block_idx} flags=0x{flags:X} → VA=0x{ref_va:08X}")
                        count += 1
            idx += 2
    
    # Step 4: Brute force - search for the string content itself in ALL sections
    # (maybe there are duplicate copies in .data section for vtable/fptr tables)
    print(f"\n[*] Searching for duplicate string copies in all sections...")
    for section in binary.sections:
        content = bytes(section.content) if section.content else b''
        if section.name in ['.rodata', '.data', '.data.rel.ro', '.bss.rel.ro']:
            continue  # Already searched .rodata
        for name, orig_va in KNOWN_STRINGS.items():
            idx = content.find(name.encode())
            while idx != -1:
                va = section.virtual_address + idx
                if va != orig_va:  # Different location
                    print(f"  [DUP] {name:35s} orig=0x{orig_va:08X} dup=0x{va:08X} ({section.name})")
                idx = content.find(name.encode(), idx + 1)
    
    mm.close()
    os.close(fd)
    
    print(f"\n[+] Complete in {time.time()-start:.1f}s")

def find_va_for_offset(file_off, sections):
    """Map file offset to virtual address"""
    for name, info in sections.items():
        if info['offset'] <= file_off < info['offset'] + info['size']:
            return info['va'] + (file_off - info['offset'])
    return None

if __name__ == "__main__":
    main()
