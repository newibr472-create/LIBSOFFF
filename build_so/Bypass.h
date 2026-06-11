#pragma once
#include "Main/Tools.h"
#include "Main/MemoryTools.h"

static int _hex_c2i(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

static void hex_patch(const char* lib, const char* offset_str, const char* hex_str) {
    uintptr_t base = Tools::GetLibBase(lib);
    if (!base) return;
    
    uintptr_t offset = strtoul(offset_str, nullptr, 16);
    uintptr_t addr = base + offset;
    
    uint8_t bytes[16];
    int len = 0;
    for (int i = 0; hex_str[i] && len < 16; i++) {
        if (hex_str[i] == ' ') continue;
        int hi = _hex_c2i(hex_str[i]);
        if (hi < 0) break;
        i++;
        if (!hex_str[i]) break;
        int lo = _hex_c2i(hex_str[i]);
        if (lo < 0) break;
        bytes[len++] = (uint8_t)((hi << 4) | lo);
    }
    
    if (len > 0) {
        WriteAddr((void*)addr, bytes, len);
    }
}

#define PATCH_LIB(lib, offset, hex) hex_patch(lib, offset, hex)

static void ApplyAllBypasses() {
    // Include all 809 patches
    #include "../true_bypass/FINAL_BYPASS.h"
}

#undef PATCH_LIB
