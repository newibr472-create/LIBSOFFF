/*
 * core/memory.cpp — Raw syscall memory operations
 * Uses process_vm_readv/writev (syscall 270/271 on aarch64)
 * Uses dl_iterate_phdr instead of parsing /proc/self/maps
 */
#include "memory.h"
#include <link.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <sys/uio.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <android/log.h>

// Raw syscall — bypasses libc hooks
extern "C" long raw_syscall(long number, ...);

#define SYS_PROCESS_VM_READV  270
#define SYS_PROCESS_VM_WRITEV 271
#define SYS_MPROTECT          226

#define TAG "M"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

// ============================================================================
// dl_iterate_phdr callback for finding library base
// ============================================================================
struct LibInfo {
    const char *name;
    uintptr_t   base;
    uintptr_t   end;
    size_t      size;
    bool        found;
};

static int dl_callback(struct dl_phdr_info *info, size_t, void *data) {
    auto *li = (LibInfo *)data;
    if (!info->dlpi_name || !info->dlpi_name[0]) return 0;

    // Match by basename
    const char *bn = strrchr(info->dlpi_name, '/');
    bn = bn ? bn + 1 : info->dlpi_name;

    if (strcmp(bn, li->name) != 0) return 0;

    li->base = info->dlpi_addr;
    li->end = 0;
    for (int i = 0; i < info->dlpi_phnum; i++) {
        uintptr_t seg_end = info->dlpi_addr + info->dlpi_phdr[i].p_vaddr + info->dlpi_phdr[i].p_memsz;
        if (seg_end > li->end) li->end = seg_end;
    }
    li->size = li->end - li->base;
    li->found = true;
    return 1; // stop iteration
}

uintptr_t mem::GetBase(const char *lib) {
    LibInfo li{};
    li.name = lib;
    li.found = false;
    dl_iterate_phdr(dl_callback, &li);
    return li.found ? li.base : 0;
}

uintptr_t mem::GetEnd(const char *lib) {
    LibInfo li{};
    li.name = lib;
    li.found = false;
    dl_iterate_phdr(dl_callback, &li);
    return li.found ? li.end : 0;
}

size_t mem::GetSize(const char *lib) {
    LibInfo li{};
    li.name = lib;
    li.found = false;
    dl_iterate_phdr(dl_callback, &li);
    return li.found ? li.size : 0;
}

// ============================================================================
// Safe memory validation via process_vm_readv
// ============================================================================
static bool pvm_read(void *addr, void *buf, size_t len) {
    struct iovec local  = { buf, len };
    struct iovec remote = { addr, len };
    pid_t pid = getpid();
    ssize_t ret = raw_syscall(SYS_PROCESS_VM_READV, pid, &local, 1, &remote, 1, 0);
    return ret == (ssize_t)len;
}

static bool pvm_write(void *addr, const void *buf, size_t len) {
    struct iovec local  = { (void *)buf, len };
    struct iovec remote = { addr, len };
    pid_t pid = getpid();
    ssize_t ret = raw_syscall(SYS_PROCESS_VM_WRITEV, pid, &local, 1, &remote, 1, 0);
    return ret == (ssize_t)len;
}

bool mem::IsValid(void *addr) {
    if (!addr || (uintptr_t)addr < 0x1000) return false;
    uintptr_t tmp = 0;
    return pvm_read(addr, &tmp, sizeof(tmp));
}

bool mem::IsReadable(void *addr, size_t len) {
    if (!addr || (uintptr_t)addr < 0x1000) return false;
    char tmp[8];
    size_t check = len < 8 ? len : 8;
    return pvm_read(addr, tmp, check);
}

bool mem::Read(void *addr, void *buf, size_t len) {
    return pvm_read(addr, buf, len);
}

bool mem::Write(void *addr, const void *buf, size_t len) {
    return pvm_write(addr, buf, len);
}

// ============================================================================
// Protection change via raw syscall
// ============================================================================
bool mem::Protect(void *addr, size_t len, int prot) {
    uintptr_t page = (uintptr_t)addr & ~0xFFFUL;
    size_t aligned_len = ((uintptr_t)addr + len - page + 0xFFF) & ~0xFFFUL;
    long ret = raw_syscall(SYS_MPROTECT, page, aligned_len, prot);
    return ret == 0;
}

// ============================================================================
// Patching
// ============================================================================
static uint8_t hex_nibble(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return 0;
}

bool mem::Patch(const char *lib, uintptr_t offset, const void *data, size_t len) {
    uintptr_t base = GetBase(lib);
    if (!base) return false;
    void *target = (void *)(base + offset);
    if (!Protect(target, len, PROT_READ | PROT_WRITE | PROT_EXEC)) return false;
    memcpy(target, data, len);
    // Flush icache
    __builtin___clear_cache((char *)target, (char *)target + len);
    return true;
}

bool mem::PatchHex(const char *lib, uintptr_t offset, const char *hexStr) {
    // Parse hex string (supports spaces)
    uint8_t buf[64];
    int len = 0;
    const char *p = hexStr;
    while (*p && len < 64) {
        if (*p == ' ') { p++; continue; }
        if (!p[1]) break;
        buf[len++] = (hex_nibble(p[0]) << 4) | hex_nibble(p[1]);
        p += 2;
    }
    if (len == 0) return false;
    return Patch(lib, offset, buf, len);
}

bool mem::Nop(const char *lib, uintptr_t offset) {
    // ARM64 NOP = 0xD503201F
    static const uint8_t nop[] = {0x1F, 0x20, 0x03, 0xD5};
    return Patch(lib, offset, nop, 4);
}

bool mem::Ret(const char *lib, uintptr_t offset) {
    // ARM64 RET = 0xD65F03C0
    static const uint8_t ret[] = {0xC0, 0x03, 0x5F, 0xD6};
    return Patch(lib, offset, ret, 4);
}

bool mem::Ret0(const char *lib, uintptr_t offset) {
    // MOV W0, #0; RET
    static const uint8_t ret0[] = {0x00, 0x00, 0x80, 0x52, 0xC0, 0x03, 0x5F, 0xD6};
    return Patch(lib, offset, ret0, 8);
}

// ============================================================================
// VTable operations — Shadow VTable technique
// ============================================================================
void **mem::CloneVTable(void **original, int count) {
    if (!original || !IsValid(original)) return nullptr;
    size_t size = count * sizeof(void *);
    void **clone = (void **)malloc(size);
    if (!clone) return nullptr;
    memcpy(clone, original, size);
    return clone;
}

bool mem::SwapVTable(void *object, void **newVtable) {
    if (!object || !IsValid(object)) return false;
    // Object's first 8 bytes = vtable pointer
    void *target = object;
    if (!Protect(target, sizeof(void *), PROT_READ | PROT_WRITE)) return false;
    *(void ***)object = newVtable;
    return true;
}

bool mem::HookVEntry(void **vtable, int index, void *hook, void **orig) {
    if (!vtable || !IsValid(&vtable[index])) return false;
    if (orig) *orig = vtable[index];
    vtable[index] = hook;
    return true;
}
