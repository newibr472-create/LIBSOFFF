/*
 * core/memory.h — Modern memory operations via raw syscalls
 * No libc wrapper dependency — AC can't hook what doesn't go through libc
 */
#pragma once
#include <cstdint>
#include <cstddef>

namespace mem {

// Base address resolution (dl_iterate_phdr, no /proc/self/maps)
uintptr_t GetBase(const char *lib);
uintptr_t GetEnd(const char *lib);
size_t    GetSize(const char *lib);

// Safe memory validation (process_vm_readv syscall)
bool IsValid(void *addr);
bool IsReadable(void *addr, size_t len);

// Memory read/write (process_vm_readv/writev syscall)
bool Read(void *addr, void *buf, size_t len);
bool Write(void *addr, const void *buf, size_t len);

// Patching (mprotect + write)
bool Patch(const char *lib, uintptr_t offset, const void *data, size_t len);
bool PatchHex(const char *lib, uintptr_t offset, const char *hexStr);
bool Nop(const char *lib, uintptr_t offset);          // ARM64 NOP (4 bytes)
bool Ret(const char *lib, uintptr_t offset);          // ARM64 RET (4 bytes)
bool Ret0(const char *lib, uintptr_t offset);         // MOV W0,#0 + RET (8 bytes)

// VTable operations
void **CloneVTable(void **original, int count);
bool   SwapVTable(void *object, void **newVtable);
bool   HookVEntry(void **vtable, int index, void *hook, void **orig);

// Protection
bool Protect(void *addr, size_t len, int prot);

// Template helpers
template <typename T>
T ReadVal(uintptr_t addr) {
    T val{};
    Read((void *)addr, &val, sizeof(T));
    return val;
}

template <typename T>
bool WriteVal(uintptr_t addr, T val) {
    return Write((void *)addr, &val, sizeof(T));
}

} // namespace mem
