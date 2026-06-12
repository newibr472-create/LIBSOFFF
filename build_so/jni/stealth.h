// stealth.h — Advanced bypass engine with anti-detection
#pragma once
#include <cstdint>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <android/log.h>
#include <sys/syscall.h>
#include <link.h>
#include <random>
#include <vector>
#include <algorithm>
#include <atomic>

#ifndef TAG
#define TAG "S"
#endif
#ifndef LOGI
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#endif

namespace stealth {

// Raw syscall — bypasses libc hooks
static inline long raw_syscall3(long nr, long a0, long a1, long a2) {
    register long x8 __asm__("x8") = nr;
    register long x0 __asm__("x0") = a0;
    register long x1 __asm__("x1") = a1;
    register long x2 __asm__("x2") = a2;
    __asm__ volatile("svc 0" : "+r"(x0) : "r"(x1),"r"(x2),"r"(x8) : "memory");
    return x0;
}

static inline long raw_mprotect(void* addr, size_t len, int prot) {
    return raw_syscall3(226, (long)addr, len, prot); // __NR_mprotect = 226 on arm64
}

// PRNG — xoshiro128+ (fast, non-crypto)
struct Rng {
    uint32_t s[4];
    void seed(uint32_t v) { s[0]=v; s[1]=v*6364136223846793005ULL+1; s[2]=s[1]*6364136223846793005ULL+1; s[3]=s[2]*6364136223846793005ULL+1; }
    uint32_t next() {
        uint32_t t = s[1]<<9, r = s[0]+s[3];
        s[2]^=s[0]; s[3]^=s[1]; s[1]^=s[2]; s[0]^=s[3]; s[2]^=t;
        s[3] = (s[3]<<11)|(s[3]>>21);
        return r;
    }
    int range(int a, int b) { return a + (next() % (b-a+1)); }
};

// Library base via dl_iterate_phdr (no /proc/self/maps)
struct LibInfo { const char* name; uintptr_t base; size_t size; };
static int dl_cb(struct dl_phdr_info* info, size_t, void* data) {
    auto* li = (LibInfo*)data;
    if (info->dlpi_name && strstr(info->dlpi_name, li->name)) {
        li->base = info->dlpi_addr;
        li->size = 0;
        for (int i = 0; i < info->dlpi_phnum; i++)
            if (info->dlpi_phdr[i].p_type == PT_LOAD)
                li->size += info->dlpi_phdr[i].p_memsz;
        return 1;
    }
    return 0;
}

static uintptr_t GetLib(const char* name) {
    LibInfo li{name, 0, 0};
    dl_iterate_phdr(dl_cb, &li);
    return li.base;
}

// Patch descriptor
struct Patch {
    const char* lib;
    uintptr_t offset;
    const uint8_t* bytes;
    uint8_t len;
    uint16_t id; // for logging
};

// Convert hex string "C0 03 5F D6" to bytes (compile-time friendly)
static int hexToBuf(const char* hex, uint8_t* buf, int maxLen) {
    int n = 0;
    while (*hex && n < maxLen) {
        while (*hex == ' ') hex++;
        if (!*hex) break;
        uint8_t hi = (*hex >= 'A') ? (*hex-'A'+10) : ((*hex >= 'a') ? (*hex-'a'+10) : (*hex-'0'));
        hex++;
        uint8_t lo = (*hex >= 'A') ? (*hex-'A'+10) : ((*hex >= 'a') ? (*hex-'a'+10) : (*hex-'0'));
        hex++;
        buf[n++] = (hi<<4)|lo;
    }
    return n;
}

// The core stealth patcher
class Engine {
    Rng rng;
    std::atomic<int> patchCount{0};
    std::atomic<int> failCount{0};
    
    // Apply single patch with page cloaking
    bool applyOne(const Patch& p) {
        uintptr_t base = GetLib(p.lib);
        if (!base) return false;
        
        uintptr_t target = base + p.offset;
        size_t pageSize = sysconf(_SC_PAGESIZE);
        uintptr_t pageStart = target & ~(pageSize - 1);
        
        // Phase 1: Make writable via raw syscall (bypass libc hooks)
        if (raw_mprotect((void*)pageStart, pageSize * 2, PROT_READ|PROT_WRITE|PROT_EXEC) != 0)
            return false;
        
        // Phase 2: Write patch bytes
        memcpy((void*)target, p.bytes, p.len);
        
        // Phase 3: Flush icache
        __builtin___clear_cache((char*)target, (char*)(target + p.len));
        
        // Phase 4: Restore original permissions (RX only — hide write evidence)
        raw_mprotect((void*)pageStart, pageSize * 2, PROT_READ|PROT_EXEC);
        
        patchCount++;
        return true;
    }
    
public:
    void init() {
        // Seed from address randomization (ASLR entropy)
        uintptr_t sp;
        __asm__ volatile("mov %0, sp" : "=r"(sp));
        rng.seed((uint32_t)(sp ^ (uintptr_t)&rng ^ getpid()));
    }
    
    // Apply a group of patches in RANDOMIZED order with staggered timing
    int applyGroup(std::vector<Patch>& patches, int startId, int delayMinUs = 100, int delayMaxUs = 2000) {
        // Shuffle order — AC can't fingerprint a fixed sequence
        for (int i = patches.size()-1; i > 0; i--) {
            int j = rng.range(0, i);
            std::swap(patches[i], patches[j]);
        }
        
        int ok = 0;
        for (auto& p : patches) {
            if (applyOne(p)) {
                ok++;
                LOGI("[%d] OK %s+0x%lx", p.id, p.lib, (unsigned long)p.offset);
            } else {
                failCount++;
                LOGI("[%d] FAIL %s+0x%lx", p.id, p.lib, (unsigned long)p.offset);
            }
            // Random micro-delay between patches (anti-timing-analysis)
            usleep(rng.range(delayMinUs, delayMaxUs));
        }
        return ok;
    }
    
    int total() { return patchCount.load(); }
    int failed() { return failCount.load(); }
};

} // namespace stealth
