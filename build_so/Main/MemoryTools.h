#pragma once
#include <cstdint>
#include <cstring>
#include <sys/mman.h>
#include <unistd.h>

static void WriteAddr(void *addr, void *value, size_t size) {
    uintptr_t page = (uintptr_t)addr & ~(sysconf(_SC_PAGESIZE) - 1);
    mprotect((void*)page, sysconf(_SC_PAGESIZE) * 2, PROT_READ | PROT_WRITE | PROT_EXEC);
    memcpy(addr, value, size);
}

template<typename T>
static void Write(uintptr_t addr, T value) {
    WriteAddr((void*)addr, &value, sizeof(T));
}

template<typename T>
static T Read(uintptr_t addr) {
    T val{};
    memcpy(&val, (void*)addr, sizeof(T));
    return val;
}
