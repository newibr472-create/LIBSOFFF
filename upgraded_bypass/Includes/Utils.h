#pragma once
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <dlfcn.h>
#include <unistd.h>
#include <android/log.h>

// ARM64-safe type definitions
typedef uint32_t DWORD;   // Keep for compat (4 bytes)
typedef uint64_t QWORD;   // 8 bytes
// Use uintptr_t for all pointer-sized values (4 on ARM32, 8 on ARM64)

// ElfScanner for library base address resolution
namespace ElfScanner {
    struct ElfInfo {
        uintptr_t base = 0;
        size_t size = 0;
    };

    static ElfInfo findElf(const char *name) {
        ElfInfo info;
        FILE *f = fopen("/proc/self/maps", "r");
        if (!f) return info;

        char line[512];
        while (fgets(line, sizeof(line), f)) {
            if (strstr(line, name) && strstr(line, "r-xp")) {
                uintptr_t start, end;
                if (sscanf(line, "%lx-%lx", &start, &end) == 2) {
                    if (info.base == 0 || start < info.base) {
                        info.base = start;
                    }
                    if (end > info.size + info.base) {
                        info.size = end - info.base;
                    }
                }
            }
        }
        fclose(f);
        return info;
    }
}

static bool isLibraryLoaded(const char *name) {
    return ElfScanner::findElf(name).base != 0;
}

static uintptr_t getLibBase(const char *name) {
    return ElfScanner::findElf(name).base;
}

static uintptr_t findLibrary(const char *name) {
    return getLibBase(name);
}
