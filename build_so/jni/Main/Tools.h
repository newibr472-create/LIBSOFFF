#pragma once
#include "Includes.h"
#include "Dobby/dobby.h"

namespace Tools {
    void Hook(void *target, void *replace, void **backup);
    bool Read(void *addr, void *buffer, size_t length);
    bool Write(void *addr, void *buffer, size_t length);
    bool ReadAddr(void *addr, void *buffer, size_t length);
    bool WriteAddr(void *addr, void *buffer, size_t length);
    bool PVM_ReadAddr(void *addr, void *buffer, size_t length);
    bool PVM_WriteAddr(void *addr, void *buffer, size_t length);
    bool IsPtrValid(void *addr);
    uintptr_t GetBaseAddress(const char *name);
    uintptr_t GetEndAddress(const char *name);
    uintptr_t FindPattern(const char *lib, const char* pattern);
}

// Global read/write (used by SDK functions that call WriteAddr directly)
inline bool ReadAddr(void *addr, void *buffer, size_t length) {
    return Tools::PVM_ReadAddr(addr, buffer, length);
}
inline bool WriteAddr(void *addr, void *buffer, size_t length) {
    return Tools::PVM_WriteAddr(addr, buffer, length);
}
