#pragma once
#include <cstdlib>
#include <cstring>
#include "Logger.h"

// ============================================================
// Macros.h — Hook & Patch Macros (ARM64 compatible)
// WOLVES PVT BYPASS v4.0
// ============================================================

// PatchInfo for PatchMonitor
struct PatchInfo {
    const char *libName;
    const char *offsetStr;
    uintptr_t targetAddr;
    std::string expectedBytes;
};

// HOOK_LIB — Inline hook via DobbyHook
// On ARM64: No Thumb bit handling needed (unlike ARM32)
#define HOOK_LIB(libName, offsetStr, newFunc, origFuncPtrVar)                           \
    do {                                                                                 \
        uintptr_t base = getLibBase(libName);                                            \
        if (base) {                                                                      \
            uintptr_t target = base + strtoull(offsetStr, nullptr, 16);                 \
            void *targetPtr = reinterpret_cast<void *>(target);                          \
            DobbyHook(targetPtr,                                                          \
                      reinterpret_cast<void *>(newFunc),                                 \
                      reinterpret_cast<void **>(&origFuncPtrVar));                        \
            LOGI("HOOK %s+%s = %p → %s", libName, offsetStr,                            \
                 reinterpret_cast<void *>(target), #newFunc);                            \
        } else {                                                                         \
            LOGE("HOOK FAIL: %s not loaded", libName);                                   \
        }                                                                                \
    } while (0)

// PATCH_LIB — Memory patch via KittyMemory
#define PATCH_LIB(libName, offsetStr, hexBytes)                                          \
    do {                                                                                 \
        auto patch = KittyMemory::MemoryPatch::createWithHex(libName, offsetStr, hexBytes);\
        if (patch.isValid()) {                                                           \
            patch.Modify();                                                              \
            LOGI("PATCH %s+%s [%s]", libName, offsetStr, hexBytes);                     \
        } else {                                                                         \
            LOGE("PATCH FAIL: %s+%s", libName, offsetStr);                              \
        }                                                                                \
    } while (0)
