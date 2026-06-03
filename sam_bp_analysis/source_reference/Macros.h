#pragma once

#include <cstdint>
#include <cstdlib>
#include <string>

#include "KittyMemory_repo/KittyMemory/KittyInclude.hpp"
#include "dobby.h"
#include "Logger.h"

#include <vector>

struct PatchInfo {
    std::string libName;
    uintptr_t offset;
    uintptr_t targetAddr;
    std::vector<uint8_t> expectedBytes;
};
extern std::vector<PatchInfo> g_patches;

// ============================================================================
// HOOK_LIB(libName, "0xOFFSET", newFunc, origFuncPtrVar)
// Installs an inline hook at libBase+offset using Dobby's DobbyHook().
// The original function pointer is written into origFuncPtrVar so the hook
// can call through to it.
//
// NOTE: For ARM32 Thumb code, the offset should be ODD (last bit set) to mark
// Thumb mode. The original BYPASS.cpp uses odd offsets like 0x23AA58+1=0x23AA59
// implicitly. If you write 0x23AA58 but the code is Thumb, Dobby handles the
// mode detection via the last bit. The source uses addresses like 0x23AA58
// which we pass to DobbyHook - Dobby figures out Thumb/ARM mode from the
// instruction at that address.
// ============================================================================
#define HOOK_LIB(libName, offsetStr, newFunc, origFuncPtrVar)                            \
    do {                                                                                 \
        ElfScanner _elf = ElfScanner::findElf(libName);                                  \
        if (!_elf.isValid()) {                                                           \
            LOGE("HOOK_LIB: %s not loaded", libName);                                    \
            break;                                                                       \
        }                                                                                \
        uintptr_t _off = (uintptr_t)strtoull((offsetStr), nullptr, 16);                  \
        void*  _tgt = (void*)(_elf.base() + _off);                                       \
        int _rc = DobbyHook(_tgt,                                                        \
                            (dobby_dummy_func_t)(newFunc),                               \
                            (dobby_dummy_func_t*)&(origFuncPtrVar));                     \
        LOGI("HOOK_LIB %s: %s+0x%lx -> %p (rc=%d, orig=%p)",                             \
             _rc == 0 ? "OK" : "FAIL", libName, (unsigned long)_off, _tgt,               \
             _rc, (void*)(origFuncPtrVar));                                              \
    } while (0)

// ============================================================================
// PATCH_LIB(libName, "0xOFFSET", "hex bytes")
// ============================================================================
#define PATCH_LIB(libName, offsetStr, hexBytes)                                          \
    do {                                                                                 \
        ElfScanner _elf = ElfScanner::findElf(libName);                                  \
        if (!_elf.isValid()) {                                                           \
            LOGE("PATCH_LIB: %s not loaded", libName);                                   \
            break;                                                                       \
        }                                                                                \
        uintptr_t offsetInt = (uintptr_t)strtoull((offsetStr), nullptr, 16);             \
        MemoryPatch _patch = MemoryPatch::createWithHex(_elf.base() + offsetInt, hexBytes); \
        bool _ok = _patch.Modify();                                                      \
        LOGI("PATCH_LIB %s: %s+0x%lx [%s]",                                              \
             _ok ? "OK" : "FAIL", libName, (unsigned long)(offsetInt), hexBytes);        \
    } while (0)
