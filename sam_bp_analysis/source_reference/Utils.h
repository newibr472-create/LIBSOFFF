#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <unordered_map>
#include <unistd.h>
#include <dlfcn.h>

#include "KittyMemory_repo/KittyMemory/KittyInclude.hpp"
#include "Logger.h"
#include "obfuscate.h"

// Windows-style typedefs used throughout PUBG bypass sources
typedef uint32_t DWORD;
typedef uint64_t QWORD;

// Check if a given .so is currently mapped into our process
inline bool isLibraryLoaded(const char* libName) {
    if (!libName) return false;
    return ElfScanner::findElf(libName).isValid();
}

// Get base address of a loaded .so (0 if not loaded)
inline uintptr_t getLibBase(const char* libName) {
    if (!libName) return 0;
    ElfScanner e = ElfScanner::findElf(libName);
    return e.isValid() ? e.base() : 0;
}

// Alias used by Hooks.cpp / BYPASS.cpp
inline uintptr_t findLibrary(const char* libName) {
    return getLibBase(libName);
}
