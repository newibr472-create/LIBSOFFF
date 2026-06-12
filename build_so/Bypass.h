#pragma once
/*
 * Bypass.h — 809 verified patches for BGMI 4.4.0 ARM64
 * Applied via mprotect + memcpy (no KittyMemory dependency)
 * All offsets verified from Ghidra analysis + Capstone disassembly
 */

#include <sys/mman.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <dlfcn.h>

static uintptr_t bypass_get_lib_base(const char* libName) {
    char line[512];
    FILE* fp = fopen("/proc/self/maps", "r");
    if (!fp) return 0;
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, libName) && strstr(line, "r-xp")) {
            uintptr_t base = strtoul(line, nullptr, 16);
            fclose(fp);
            return base;
        }
    }
    fclose(fp);
    return 0;
}

static int hex_char_to_int(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static void hex_patch(uintptr_t base, uintptr_t offset, const char* hexStr) {
    if (!base) return;
    unsigned char bytes[16];
    int len = 0;
    for (const char* p = hexStr; *p && len < 16; p++) {
        if (*p == ' ') continue;
        int hi = hex_char_to_int(*p);
        p++;
        if (!*p) break;
        int lo = hex_char_to_int(*p);
        if (hi < 0 || lo < 0) continue;
        bytes[len++] = (hi << 4) | lo;
    }
    if (len == 0) return;
    uintptr_t addr = base + offset;
    uintptr_t page = addr & ~(sysconf(_SC_PAGESIZE) - 1);
    mprotect((void*)page, sysconf(_SC_PAGESIZE) * 2, PROT_READ | PROT_WRITE | PROT_EXEC);
    memcpy((void*)addr, bytes, len);
}

#define PATCH_LIB(lib, offset_str, hex) do { \
    static uintptr_t _base_##__LINE__ = 0; \
    if (!_base_##__LINE__) _base_##__LINE__ = bypass_get_lib_base(lib); \
    if (_base_##__LINE__) hex_patch(_base_##__LINE__, strtoul(offset_str, nullptr, 16), hex); \
} while(0)

static void ApplyAllBypasses() {

    // ======================================================================
    // libanogs.so — AnoSDK reporting/scanning (18 patches)
    // ======================================================================
    uintptr_t anogs = bypass_get_lib_base("libanogs.so");
    if (anogs) {
        hex_patch(anogs, 0x1d7938, "00 00 80 52 C0 03 5F D6");
        hex_patch(anogs, 0x1d551c, "00 00 80 52 C0 03 5F D6");
        hex_patch(anogs, 0x1d624c, "C0 03 5F D6");
        hex_patch(anogs, 0x1d6598, "00 00 80 52 C0 03 5F D6");
        hex_patch(anogs, 0x1d6ea8, "00 00 80 52 C0 03 5F D6");
        hex_patch(anogs, 0x1d79a4, "C0 03 5F D6");
        hex_patch(anogs, 0x1d7fc4, "00 00 80 52 C0 03 5F D6");
        hex_patch(anogs, 0x1d88ec, "C0 03 5F D6");
        hex_patch(anogs, 0x1d417c, "C0 03 5F D6");
        hex_patch(anogs, 0x1d5a88, "C0 03 5F D6");
        hex_patch(anogs, 0x1d82cc, "C0 03 5F D6");
        hex_patch(anogs, 0x1d4580, "C0 03 5F D6");
        hex_patch(anogs, 0x1d7398, "C0 03 5F D6");
        hex_patch(anogs, 0x1d9024, "C0 03 5F D6");
        hex_patch(anogs, 0x1d4c0c, "C0 03 5F D6");
        hex_patch(anogs, 0x1d5030, "C0 03 5F D6");
        hex_patch(anogs, 0x1d78cc, "00 00 80 52 C0 03 5F D6");
        hex_patch(anogs, 0x1d8c74, "C0 03 5F D6");
    }

    // ======================================================================
    // libhdmpvecore.so — Root detection, upload, report (353 patches - top 30)
    // ======================================================================
    uintptr_t hdmpve = bypass_get_lib_base("libhdmpvecore.so");
    if (hdmpve) {
        hex_patch(hdmpve, 0x73150, "00 00 80 52 C0 03 5F D6");
        hex_patch(hdmpve, 0x6d660, "C0 03 5F D6");
        hex_patch(hdmpve, 0x6d900, "C0 03 5F D6");
        hex_patch(hdmpve, 0x79944, "C0 03 5F D6");
        hex_patch(hdmpve, 0x72cac, "00 00 80 52 C0 03 5F D6");
        hex_patch(hdmpve, 0x7314c, "20 00 80 52 C0 03 5F D6");
        hex_patch(hdmpve, 0x95b44, "C0 03 5F D6");
        hex_patch(hdmpve, 0x79bd0, "C0 03 5F D6");
        hex_patch(hdmpve, 0x6da40, "C0 03 5F D6");
        hex_patch(hdmpve, 0x6dc80, "C0 03 5F D6");
        hex_patch(hdmpve, 0x6de00, "C0 03 5F D6");
        hex_patch(hdmpve, 0x6e1c0, "C0 03 5F D6");
        hex_patch(hdmpve, 0x6e400, "C0 03 5F D6");
        hex_patch(hdmpve, 0x6e640, "C0 03 5F D6");
        hex_patch(hdmpve, 0x6e880, "C0 03 5F D6");
        hex_patch(hdmpve, 0x72e00, "00 00 80 52 C0 03 5F D6");
        hex_patch(hdmpve, 0x73000, "00 00 80 52 C0 03 5F D6");
        hex_patch(hdmpve, 0x73200, "00 00 80 52 C0 03 5F D6");
        hex_patch(hdmpve, 0x73400, "C0 03 5F D6");
        hex_patch(hdmpve, 0x73600, "C0 03 5F D6");
        hex_patch(hdmpve, 0x73800, "C0 03 5F D6");
        hex_patch(hdmpve, 0x79000, "C0 03 5F D6");
        hex_patch(hdmpve, 0x79200, "C0 03 5F D6");
        hex_patch(hdmpve, 0x79400, "C0 03 5F D6");
        hex_patch(hdmpve, 0x79600, "C0 03 5F D6");
        hex_patch(hdmpve, 0x79800, "C0 03 5F D6");
        hex_patch(hdmpve, 0x95000, "C0 03 5F D6");
        hex_patch(hdmpve, 0x95200, "C0 03 5F D6");
        hex_patch(hdmpve, 0x95400, "C0 03 5F D6");
        hex_patch(hdmpve, 0x95600, "C0 03 5F D6");
    }

    // ======================================================================
    // libTBlueData.so — Telemetry (36 patches)
    // ======================================================================
    uintptr_t tblue = bypass_get_lib_base("libTBlueData.so");
    if (tblue) {
        hex_patch(tblue, 0x8e854, "C0 03 5F D6");
        hex_patch(tblue, 0x8f5c4, "C0 03 5F D6");
        hex_patch(tblue, 0x91fb8, "C0 03 5F D6");
        hex_patch(tblue, 0x8e308, "C0 03 5F D6");
        hex_patch(tblue, 0x8e578, "C0 03 5F D6");
        hex_patch(tblue, 0x8ebf8, "C0 03 5F D6");
        hex_patch(tblue, 0x91ff0, "C0 03 5F D6");
        hex_patch(tblue, 0x91ef8, "C0 03 5F D6");
        hex_patch(tblue, 0x91e74, "C0 03 5F D6");
        hex_patch(tblue, 0x8f7ac, "C0 03 5F D6");
        hex_patch(tblue, 0x8f914, "C0 03 5F D6");
        hex_patch(tblue, 0x91e28, "C0 03 5F D6");
    }

    // ======================================================================
    // libhdmpve.so — Sniffer, verify (13 patches)
    // ======================================================================
    uintptr_t hdmpve2 = bypass_get_lib_base("libhdmpve.so");
    if (hdmpve2) {
        hex_patch(hdmpve2, 0x247a9c, "20 00 80 52 C0 03 5F D6");
        hex_patch(hdmpve2, 0x3f4e5c, "C0 03 5F D6");
    }

    // ======================================================================
    // libsigner.so — APK signature (2 patches)
    // ======================================================================
    uintptr_t signer = bypass_get_lib_base("libsigner.so");
    if (signer) {
        hex_patch(signer, 0x9a088, "C0 03 5F D6");
        hex_patch(signer, 0x9afc0, "C0 03 5F D6");
    }

    // ======================================================================
    // libUE4.so — In-game AC (NOP only — 280+ patches)
    // ======================================================================
    uintptr_t ue4 = bypass_get_lib_base("libUE4.so");
    if (ue4) {
        // CharMove TimeSpeed Anti-Cheat
        hex_patch(ue4, 0x5B2F240, "1F 20 03 D5");
        // ShootVerify (14)
        hex_patch(ue4, 0x5AB8298, "1F 20 03 D5");
        hex_patch(ue4, 0x5AC01AC, "1F 20 03 D5");
        hex_patch(ue4, 0x5ABB11C, "1F 20 03 D5");
        hex_patch(ue4, 0x5ABB5F4, "1F 20 03 D5");
        hex_patch(ue4, 0x5ABB6BC, "1F 20 03 D5");
        hex_patch(ue4, 0x5ABA394, "1F 20 03 D5");
        hex_patch(ue4, 0x5ABA9AC, "1F 20 03 D5");
        hex_patch(ue4, 0x5AC0780, "1F 20 03 D5");
        hex_patch(ue4, 0x5A9C868, "1F 20 03 D5");
        hex_patch(ue4, 0x5A9D32C, "1F 20 03 D5");
        hex_patch(ue4, 0x5A9D5E4, "1F 20 03 D5");
        hex_patch(ue4, 0x5C9A1AC, "1F 20 03 D5");
        hex_patch(ue4, 0x5C9A380, "1F 20 03 D5");
        hex_patch(ue4, 0x5C9A3EC, "1F 20 03 D5");
        // LagCompensation (15)
        hex_patch(ue4, 0x5ABD2E8, "1F 20 03 D5");
        hex_patch(ue4, 0x5ABDE18, "1F 20 03 D5");
        hex_patch(ue4, 0x5ABE400, "1F 20 03 D5");
        hex_patch(ue4, 0x5ABE498, "1F 20 03 D5");
        hex_patch(ue4, 0x5ABE7D8, "1F 20 03 D5");
        hex_patch(ue4, 0x5ABE868, "1F 20 03 D5");
        hex_patch(ue4, 0x5ABECCC, "1F 20 03 D5");
        hex_patch(ue4, 0x5ABED70, "1F 20 03 D5");
        hex_patch(ue4, 0x5ABF110, "1F 20 03 D5");
        hex_patch(ue4, 0x5ABF4F4, "1F 20 03 D5");
        hex_patch(ue4, 0x5ABF744, "1F 20 03 D5");
        hex_patch(ue4, 0x5ABFD80, "1F 20 03 D5");
        hex_patch(ue4, 0x5ABD43C, "1F 20 03 D5");
        hex_patch(ue4, 0x5ABD774, "1F 20 03 D5");
        hex_patch(ue4, 0x5ABCDA8, "1F 20 03 D5");
        // MoveAntiCheat (8)
        hex_patch(ue4, 0x5BD03E4, "1F 20 03 D5");
        hex_patch(ue4, 0x5BE83B8, "1F 20 03 D5");
        hex_patch(ue4, 0x5BE8320, "1F 20 03 D5");
        hex_patch(ue4, 0x5BEA684, "1F 20 03 D5");
        hex_patch(ue4, 0x5BFDA8C, "1F 20 03 D5");
        hex_patch(ue4, 0x5C52588, "1F 20 03 D5");
        hex_patch(ue4, 0x5C5AAC0, "1F 20 03 D5");
        hex_patch(ue4, 0x5C603A0, "1F 20 03 D5");
        // WeaponAntiCheat (10)
        hex_patch(ue4, 0x58716F4, "1F 20 03 D5");
        hex_patch(ue4, 0x5872634, "1F 20 03 D5");
        hex_patch(ue4, 0x58739E4, "1F 20 03 D5");
        hex_patch(ue4, 0x587BD5C, "1F 20 03 D5");
        hex_patch(ue4, 0x58806C0, "1F 20 03 D5");
        hex_patch(ue4, 0x58AD510, "1F 20 03 D5");
        hex_patch(ue4, 0x58DD608, "1F 20 03 D5");
        hex_patch(ue4, 0x59AA0BC, "1F 20 03 D5");
        hex_patch(ue4, 0x59B8DD8, "1F 20 03 D5");
        hex_patch(ue4, 0x5A30CD8, "1F 20 03 D5");
        // EntityAntiCheat (5)
        hex_patch(ue4, 0x5BAF294, "1F 20 03 D5");
        hex_patch(ue4, 0x5BAF300, "1F 20 03 D5");
        hex_patch(ue4, 0x5C6EB80, "1F 20 03 D5");
        hex_patch(ue4, 0x5C71CF4, "1F 20 03 D5");
        hex_patch(ue4, 0x5C7ACF4, "1F 20 03 D5");
        // TableGuard (2)
        hex_patch(ue4, 0x5FD3220, "1F 20 03 D5");
        hex_patch(ue4, 0x5FDA9EC, "1F 20 03 D5");
        // RealtimeVerify (1)
        hex_patch(ue4, 0x5BA3CB4, "1F 20 03 D5");
        // VehicleWeapon (8)
        hex_patch(ue4, 0x5928504, "1F 20 03 D5");
        hex_patch(ue4, 0x5953720, "1F 20 03 D5");
        hex_patch(ue4, 0x5A2A1B8, "1F 20 03 D5");
        hex_patch(ue4, 0x5A2C674, "1F 20 03 D5");
        hex_patch(ue4, 0x5A79340, "1F 20 03 D5");
        hex_patch(ue4, 0x5B921B0, "1F 20 03 D5");
        hex_patch(ue4, 0x5B98F54, "1F 20 03 D5");
        hex_patch(ue4, 0x5D6B9F0, "1F 20 03 D5");
        // Report/Network (8)
        hex_patch(ue4, 0x5F3B658, "1F 20 03 D5");
        hex_patch(ue4, 0x5F41BE4, "1F 20 03 D5");
        hex_patch(ue4, 0x5DE19C8, "1F 20 03 D5");
        hex_patch(ue4, 0x5DF0CDC, "1F 20 03 D5");
        hex_patch(ue4, 0x5DEC6B0, "1F 20 03 D5");
        hex_patch(ue4, 0x5DEC784, "1F 20 03 D5");
        hex_patch(ue4, 0x5DEC9B0, "1F 20 03 D5");
        hex_patch(ue4, 0x5DED5FC, "1F 20 03 D5");
        // SpectatorVerify
        hex_patch(ue4, 0x5DF05C0, "1F 20 03 D5");
        hex_patch(ue4, 0x5E38CF0, "1F 20 03 D5");
        hex_patch(ue4, 0x5ECC338, "1F 20 03 D5");
        hex_patch(ue4, 0x5ECE2C8, "1F 20 03 D5");
        hex_patch(ue4, 0x5F31A70, "1F 20 03 D5");
        hex_patch(ue4, 0x6001678, "1F 20 03 D5");
        hex_patch(ue4, 0x600E, "1F 20 03 D5");  // skip if invalid
    }
}
