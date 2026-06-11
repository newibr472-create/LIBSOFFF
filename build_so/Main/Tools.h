#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/mman.h>
#include <dlfcn.h>

namespace Tools {
    static inline bool IsPtrValid(void* ptr) {
        return ptr != nullptr && (uintptr_t)ptr > 0x1000 && (uintptr_t)ptr < 0x7FFFFFFFFFF;
    }
    
    static inline uintptr_t GetLibBase(const char* lib) {
        char line[512];
        FILE* fp = fopen("/proc/self/maps", "r");
        if (!fp) return 0;
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, lib) && strstr(line, "r-xp")) {
                uintptr_t base = 0;
                sscanf(line, "%lx", &base);
                fclose(fp);
                return base;
            }
        }
        fclose(fp);
        return 0;
    }
    
    static inline uintptr_t GetLibEnd(const char* lib) {
        char line[512];
        uintptr_t end = 0;
        FILE* fp = fopen("/proc/self/maps", "r");
        if (!fp) return 0;
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, lib)) {
                uintptr_t s, e;
                sscanf(line, "%lx-%lx", &s, &e);
                if (e > end) end = e;
            }
        }
        fclose(fp);
        return end;
    }
}
