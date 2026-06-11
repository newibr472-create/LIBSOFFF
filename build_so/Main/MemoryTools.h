#pragma once
#include <sys/mman.h>
#include <cstring>
#include <cstdint>

static inline bool MemProtect(uintptr_t addr, size_t len, int prot) {
    uintptr_t page = addr & ~0xFFF;
    size_t size = (addr - page) + len;
    size = (size + 0xFFF) & ~0xFFF;
    return mprotect((void*)page, size, prot) == 0;
}

static inline bool WriteAddr(void* addr, void* value, size_t size) {
        return false;
    memcpy(addr, value, size);
    __builtin___clear_cache((char*)addr, (char*)addr + size);
    return true;
}

template<typename T>
static inline T ReadMem(uintptr_t addr) { return *(T*)addr; }

template<typename T>
static inline void WriteMem(uintptr_t addr, T value) {
    WriteAddr((void*)addr, &value, sizeof(T));
}
