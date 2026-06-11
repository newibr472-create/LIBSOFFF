#pragma once
#include <cstdint>
#include <cstring>
#include <sys/mman.h>

static inline bool A64HookFunction(void* target, void* hook, void** original) {
    uint8_t shellcode[16] = {
        0x51, 0x00, 0x00, 0x58,  // LDR X17, [PC, #8]
        0x20, 0x02, 0x1F, 0xD6,  // BR X17
        0, 0, 0, 0, 0, 0, 0, 0  // 64-bit hook address
    };
    
    if (original) {
        uint8_t* tramp = (uint8_t*)mmap(NULL, 64, PROT_READ|PROT_WRITE|PROT_EXEC,
                                         MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
        if (tramp == MAP_FAILED) return false;
        memcpy(tramp, target, 16);
        uintptr_t retAddr = (uintptr_t)target + 16;
        tramp[16] = 0x51; tramp[17] = 0x00; tramp[18] = 0x00; tramp[19] = 0x58;
        tramp[20] = 0x20; tramp[21] = 0x02; tramp[22] = 0x1F; tramp[23] = 0xD6;
        memcpy(tramp + 24, &retAddr, 8);
        __builtin___clear_cache((char*)tramp, (char*)tramp + 32);
        *original = (void*)tramp;
    }
    
    uintptr_t hookAddr = (uintptr_t)hook;
    memcpy(shellcode + 8, &hookAddr, 8);
    uintptr_t page = (uintptr_t)target & ~0xFFF;
    mprotect((void*)page, 0x2000, PROT_READ|PROT_WRITE|PROT_EXEC);
    memcpy(target, shellcode, 16);
    __builtin___clear_cache((char*)target, (char*)target + 16);
    return true;
}
