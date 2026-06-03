// ============================================================
// BYPASS.cpp — WOLVES PVT BYPASS v4.0 (ARM64)
// Target: PUBG Mobile 4.4.0 / BGMI
// Architecture: arm64-v8a + armeabi-v7a
// Multi-Emulator Support: GameLoop, LDPlayer, MEmu, Nox, BlueStacks
// ============================================================
//
// CRITICAL FIXES from v3.8.1:
// 1. Android.mk now compiles THIS file (not the old root version)
// 2. libc hooks are NOW INSTALLED (were defined but never called)
// 3. Shadow memory CRC evasion hooks are NOW INSTALLED
// 4. All pointer types use uintptr_t (ARM64 safe — no truncation)
// 5. All offsets updated to PUBG 4.4.0 ARM64
// 6. Multi-emulator support (not just GameLoop)
// ============================================================

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <pthread.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <sys/system_properties.h>
#include <dirent.h>
#include <elf.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <android/log.h>
#include <vector>
#include <string>
#include <algorithm>

// Dobby inline hooking framework
#include "Dobby_inc/dobby.h"

// KittyMemory for memory patching
#include "KittyMemory/MemoryPatch.h"

// Project headers
#include "Includes/Logger.h"
#include "Includes/Macros.h"
#include "Includes/obfuscate.h"
#include "Includes/Utils.h"
#include "hooks/anogs_hooks.h"
#include "hooks/ue4_hooks.h"
#include "hooks/tdm_hooks.h"

// ============================================================
// Type definitions (ARM64-safe)
// ============================================================
typedef uint8_t  _BYTE;
typedef uint16_t _WORD;
typedef uint32_t _DWORD;
typedef uint64_t _QWORD;

// ============================================================
// Global variables (all uintptr_t for ARM64)
// ============================================================
uintptr_t libanogsBase = 0, libanogsAlloc = 0, libanogsSize = 0;
uintptr_t libUE4Base = 0, libUE4Alloc = 0, libUE4Size = 0;

// Patch monitor data
static std::vector<PatchInfo> g_patches;

// FD cleanup
static std::vector<int> g_openFds;

// ============================================================
// FD Snapshot & Cleanup
// ============================================================
static void SnapshotOpenFds() {
    g_openFds.clear();
    DIR *d = opendir("/proc/self/fd");
    if (d) {
        struct dirent *de;
        while ((de = readdir(d))) {
            int fd = atoi(de->d_name);
            if (fd > 2) g_openFds.push_back(fd);
        }
        closedir(d);
    }
    LOGI("FD snapshot: %zu fds", g_openFds.size());
}

static void CloseNewFds() {
    std::vector<int> currentFds;
    DIR *d = opendir("/proc/self/fd");
    if (!d) return;
    struct dirent *de;
    while ((de = readdir(d))) {
        int fd = atoi(de->d_name);
        if (fd > 2) currentFds.push_back(fd);
    }
    closedir(d);

    for (int fd : currentFds) {
        if (std::find(g_openFds.begin(), g_openFds.end(), fd) == g_openFds.end()) {
            // Check if it's an ashmem/memfd from Dobby trampolines
            char link[256] = {0};
            char path[256] = {0};
            snprintf(link, sizeof(link), "/proc/self/fd/%d", fd);
            ssize_t len = readlink(link, path, sizeof(path) - 1);
            if (len > 0) {
                path[len] = '\0';
                if (strstr(path, "/dev/ashmem") || strstr(path, "/memfd:") ||
                    strstr(path, "dobby") || strstr(path, "dobby")) {
                    // Don't close MMKV fds
                    if (!strstr(path, "MMKV") && !strstr(path, "mmkv")) {
                        close(fd);
                        LOGI("Closed FD %d: %s", fd, path);
                    }
                }
            }
        }
    }
}

// ============================================================
// Shadow Memory (CRC Evasion)
// ============================================================
static uintptr_t CreateShadowCopy(const char *libName, uintptr_t base, size_t size) {
    if (!base || !size) return 0;
    void *shadow = malloc(size);
    if (shadow) {
        memcpy(shadow, reinterpret_cast<void *>(base), size);
        LOGI("Shadow copy of %s: %p (%zu bytes)", libName, shadow, size);
    }
    return reinterpret_cast<uintptr_t>(shadow);
}

// ============================================================
// Libc Hooks — Emulator Detection Bypass
// FIXED: Now actually installed (was dead code in v3.8.1)
// ============================================================

// Blocked emulator indicator file paths
static const char *g_blockedPaths[] = {
    "/dev/goldfish_pipe", "/dev/qemu_pipe", "/dev/socket/genyd",
    "/dev/socket/qemud", "/dev/vboxguest", "/dev/vboxuser",
    "/system/lib/libvboxjvm.so", "/system/lib64/libvboxjvm.so",
    "/system/bin/qemu-props", "/system/bin/androVM-prop",
    "/system/bin/genymotion-shell", "/system/lib/libc_malloc_debug_qemu.so",
    "/sys/class/thermal/thermal_zone0/type", "/sys/qemu_trace",
    "/system/lib/libbluestacks.so", "/system/lib64/libbluestacks.so",
    "/preload/shell/init.nox", "/data/init.nox",
    "/system/bin/nox", "/system/lib/libnox.so",
    "/dev/memtrack", "/proc/irq", nullptr
};

// Spoofed properties — Samsung Galaxy S23 Ultra (SM-S918B)
static const struct { const char *key; const char *val; } g_spoofedProps[] = {
    {"ro.product.model", "SM-S918B"},
    {"ro.product.brand", "samsung"},
    {"ro.product.manufacturer", "samsung"},
    {"ro.product.device", "dm3q"},
    {"ro.product.board", "s5e9925"},
    {"ro.board.platform", "s5e9925"},
    {"ro.hardware", "s5e9925"},
    {"ro.hardware.chipname", "exynos2400"},
    {"ro.product.name", "dm3qxxx"},
    {"ro.build.display.id", "TP1A.220624.014"},
    {"ro.build.description", "dm3qxxx-user 13 TP1A.220624.014 release-keys"},
    {"ro.build.fingerprint", "samsung/dm3qxxx/dm3q:13/TP1A.220624.014:user/release-keys"},
    {"ro.build.tags", "release-keys"},
    {"ro.build.type", "user"},
    {"ro.debuggable", "0"},
    {"ro.secure", "1"},
    {"persist.sys.timezone", "Asia/Calcutta"},
    {nullptr, nullptr}
};

// Original libc function pointers
static int (*orig_system_property_get)(const char *, char *) = nullptr;
static int (*orig_access)(const char *, int) = nullptr;
static FILE *(*orig_fopen)(const char *, const char *) = nullptr;
static int (*orig_stat)(const char *, struct stat *) = nullptr;

// Hook: __system_property_get — Spoof device properties
static int hooked_system_property_get(const char *name, char *value) {
    if (!name || !value) return orig_system_property_get(name, value);

    for (int i = 0; g_spoofedProps[i].key; i++) {
        if (strcmp(name, g_spoofedProps[i].key) == 0) {
            strcpy(value, g_spoofedProps[i].val);
            return strlen(value);
        }
    }

    // Block qemu/ranchu/goldfish properties
    if (strstr(name, "qemu") || strstr(name, "goldfish") ||
        strstr(name, "ranchu") || strstr(name, "nox") ||
        strstr(name, "vbox") || strstr(name, "bluestacks") ||
        strstr(name, "ttvm")) {
        value[0] = '\0';
        return 0;
    }

    return orig_system_property_get(name, value);
}

// Hook: access — Block emulator file paths
static int hooked_access(const char *pathname, int mode) {
    if (pathname) {
        for (int i = 0; g_blockedPaths[i]; i++) {
            if (strstr(pathname, g_blockedPaths[i])) {
                errno = ENOENT;
                return -1;
            }
        }
    }
    return orig_access(pathname, mode);
}

// Hook: fopen — Block emulator file paths
static FILE *fopen_t(const char *pathname, const char *mode) {
    if (pathname) {
        for (int i = 0; g_blockedPaths[i]; i++) {
            if (strstr(pathname, g_blockedPaths[i])) {
                errno = ENOENT;
                return nullptr;
            }
        }
    }
    return orig_fopen(pathname, mode);
}

// Hook: stat — Block emulator file paths
static int stat_t(const char *pathname, struct stat *statbuf) {
    if (pathname) {
        for (int i = 0; g_blockedPaths[i]; i++) {
            if (strstr(pathname, g_blockedPaths[i])) {
                errno = ENOENT;
                return -1;
            }
        }
    }
    return orig_stat(pathname, statbuf);
}

// ============================================================
// CRC Evasion Hooks — Redirect reads to shadow memory
// FIXED: Now actually installed (was defined but not called in v3.8.1)
// ============================================================
static void *(*orig_memcpy)(void *, const void *, size_t) = nullptr;
static int (*orig_memcmp)(const void *, const void *, size_t) = nullptr;

// Hook: memcpy — If source is in a hooked .so region, read from shadow instead
static void *hooked_memcpy(void *dest, const void *src, size_t n) {
    uintptr_t srcAddr = reinterpret_cast<uintptr_t>(src);

    // Check if reading from libanogs.so region
    if (libanogsBase && libanogsAlloc && srcAddr >= libanogsBase &&
        srcAddr < libanogsBase + libanogsSize) {
        uintptr_t shadowOffset = srcAddr - libanogsBase;
        const void *shadowSrc = reinterpret_cast<const void *>(libanogsAlloc + shadowOffset);
        return orig_memcpy(dest, shadowSrc, n);
    }

    // Check if reading from libUE4.so region
    if (libUE4Base && libUE4Alloc && srcAddr >= libUE4Base &&
        srcAddr < libUE4Base + libUE4Size) {
        uintptr_t shadowOffset = srcAddr - libUE4Base;
        const void *shadowSrc = reinterpret_cast<const void *>(libUE4Alloc + shadowOffset);
        return orig_memcpy(dest, shadowSrc, n);
    }

    return orig_memcpy(dest, src, n);
}

// Hook: memcmp — If comparing bytes from hooked .so region, use shadow
static int hooked_memcmp(const void *s1, const void *s2, size_t n) {
    uintptr_t s1Addr = reinterpret_cast<uintptr_t>(s1);
    uintptr_t s2Addr = reinterpret_cast<uintptr_t>(s2);

    // Redirect s1 if in hooked region
    if (libanogsBase && libanogsAlloc && s1Addr >= libanogsBase &&
        s1Addr < libanogsBase + libanogsSize) {
        s1 = reinterpret_cast<const void *>(libanogsAlloc + (s1Addr - libanogsBase));
    }
    if (libUE4Base && libUE4Alloc && s1Addr >= libUE4Base &&
        s1Addr < libUE4Base + libUE4Size) {
        s1 = reinterpret_cast<const void *>(libUE4Alloc + (s1Addr - libUE4Base));
    }

    // Redirect s2 if in hooked region
    if (libanogsBase && libanogsAlloc && s2Addr >= libanogsBase &&
        s2Addr < libanogsBase + libanogsSize) {
        s2 = reinterpret_cast<const void *>(libanogsAlloc + (s2Addr - libanogsBase));
    }
    if (libUE4Base && libUE4Alloc && s2Addr >= libUE4Base &&
        s2Addr < libUE4Base + libUE4Size) {
        s2 = reinterpret_cast<const void *>(libUE4Alloc + (s2Addr - libUE4Base));
    }

    return orig_memcmp(s1, s2, n);
}

// ============================================================
// Thread: AnoGS Hook Installation
// ============================================================
static void *anogs_thread(void *arg) {
    LOGI("anogs_thread: waiting for libanogs.so...");

    while (!isLibraryLoaded(OBFUSCATE("libanogs.so"))) {
        usleep(500000); // 0.5s
    }

    auto info = ElfScanner::findElf(OBFUSCATE("libanogs.so"));
    libanogsBase = info.base;
    libanogsSize = info.size;

    LOGI("libanogs.so loaded at %p (size: 0x%zx)",
         reinterpret_cast<void *>(libanogsBase), libanogsSize);

    // Create shadow copy for CRC evasion
    libanogsAlloc = CreateShadowCopy("libanogs.so", libanogsBase, libanogsSize);

    // Hook AnoSDKGetReportData — device info reporter
    HOOK_LIB(OBFUSCATE("libanogs.so"), "0x1D551C",
             hAnoSDKGetReportData, orig_AnoSDKGetReportData);

    // Hook AnoSDKGetReportData3 — report data v3
    HOOK_LIB(OBFUSCATE("libanogs.so"), "0x1D7938",
             hAnoSDKGetReportData3, orig_AnoSDKGetReportData3);

    // Hook AnoSDKGetReportData2 — report data v2
    HOOK_LIB(OBFUSCATE("libanogs.so"), "0x1D78CC",
             hAnoSDKGetReportData2, orig_AnoSDKGetReportData2);

    LOGI("anogs_thread: %d hooks installed", 3);

    // FD cleanup after 5 seconds
    sleep(5);
    CloseNewFds();
    LOGI("anogs_thread: FD cleanup done");

    return nullptr;
}

// ============================================================
// Thread: UE4 Anti-Cheat Hook Installation
// ============================================================
static void *AntiBan_thread(void *arg) {
    LOGI("AntiBan_thread: waiting for libUE4.so...");

    while (!isLibraryLoaded(OBFUSCATE("libUE4.so"))) {
        usleep(500000);
    }

    auto info = ElfScanner::findElf(OBFUSCATE("libUE4.so"));
    libUE4Base = info.base;
    libUE4Size = info.size;

    LOGI("libUE4.so loaded at %p (size: 0x%zx)",
         reinterpret_cast<void *>(libUE4Base), libUE4Size);

    // Create shadow copy for CRC evasion
    libUE4Alloc = CreateShadowCopy("libUE4.so", libUE4Base, libUE4Size);

    // ---- Emulator Detection Hooks (from SDK dump) ----

    // Hook IsEmulator — returns false
    HOOK_LIB(OBFUSCATE("libUE4.so"), "0x7625FB0",
             hIsEmulator, orig_IsEmulator);

    // Hook IsEmulatorWhenInit — returns false
    HOOK_LIB(OBFUSCATE("libUE4.so"), "0x7625FE8",
             hIsEmulatorWhenInit, orig_IsEmulatorWhenInit);

    // Hook GetEmulatorName — returns null
    HOOK_LIB(OBFUSCATE("libUE4.so"), "0x761E918",
             hGetEmulatorName, orig_GetEmulatorName);

    // ---- Anti-Cheat Report Hooks (from SDK dump) ----

    // Hook ReportAntiCheatInfo — blocks AC info reporting
    HOOK_LIB(OBFUSCATE("libUE4.so"), "0x6FB028C",
             hReportAntiCheatInfo, orig_ReportAntiCheatInfo);

    // Hook ReportAntiCheatDetailData — blocks detailed AC reporting
    HOOK_LIB(OBFUSCATE("libUE4.so"), "0x6D95FEC",
             hReportAntiCheatDetailData, orig_ReportAntiCheatDetailData);

    // Hook CheckNeedReport — always returns false
    HOOK_LIB(OBFUSCATE("libUE4.so"), "0x6FA37C4",
             hCheckNeedReport, orig_CheckNeedReport);

    // Hook ServerHandleHitDataArray — blocks hit data
    HOOK_LIB(OBFUSCATE("libUE4.so"), "0x6FB78A0",
             hServerHandleHitDataArray, orig_ServerHandleHitDataArray);

    // Hook HandleHitDataArray — blocks hit processing
    HOOK_LIB(OBFUSCATE("libUE4.so"), "0x6FAAA44",
             hHandleHitDataArray, orig_HandleHitDataArray);

    LOGI("AntiBan_thread: %d hooks installed", 8);

    // FD cleanup
    sleep(5);
    CloseNewFds();

    return nullptr;
}

// ============================================================
// Thread: TDataMaster + GCloud Hook Installation
// ============================================================
static void *TDataMaster_thread(void *arg) {
    LOGI("TDataMaster_thread: waiting for libTDataMaster.so...");

    while (!isLibraryLoaded(OBFUSCATE("libTDataMaster.so"))) {
        usleep(500000);
    }

    LOGI("libTDataMaster.so detected");

    // ---- TDM Hooks ----

    // Hook TDM GetInstance — return null (prevent singleton)
    HOOK_LIB(OBFUSCATE("libTDataMaster.so"), "0x4D47C",
             hTDM_GetInstance, orig_TDM_GetInstance);

    // Hook Report Send — block all TDM reports
    HOOK_LIB(OBFUSCATE("libTDataMaster.so"), "0x4E33C",
             hReportSend, orig_ReportSend);

    // Hook AC Config Init — skip config setup
    HOOK_LIB(OBFUSCATE("libTDataMaster.so"), "0x569C0",
             hACConfigInit, orig_ACConfigInit);

    // ---- PluginReport Hooks (in libTDataMaster.so on ARM64!) ----

    // OnStartup — prevent plugin registration
    HOOK_LIB(OBFUSCATE("libTDataMaster.so"), "0x92B04",
             hPluginReportOnStartup, nullptr);

    // D2/D0 destructors, OnPost/PreShutdown, OnShutdown are already no-ops in ARM64
    // (single ret instruction) — no need to hook

    LOGI("TDataMaster_thread: TDM hooks installed (4)");

    // Wait for libgcloud.so
    LOGI("TDataMaster_thread: waiting for libgcloud.so...");
    while (!isLibraryLoaded(OBFUSCATE("libgcloud.so"))) {
        usleep(500000);
    }

    LOGI("libgcloud.so detected");

    // ---- GCloud Hooks ----

    // Hook GCloud GetInstance — return null
    HOOK_LIB(OBFUSCATE("libgcloud.so"), "0x097D18",
             hTDM_GetInstance, nullptr); // Reuse null-returning hook

    // Hook gcloud_init — prevent initialization
    HOOK_LIB(OBFUSCATE("libgcloud.so"), "0xF4AEC",
             hACConfigInit, nullptr); // Reuse early-return hook

    LOGI("TDataMaster_thread: GCloud hooks installed (2)");

    return nullptr;
}

// ============================================================
// Thread: PatchMonitor — Watch for AC restoring patched bytes
// ============================================================
static void *PatchMonitor_thread(void *arg) {
    LOGI("PatchMonitor_thread: started (10s interval)");

    while (true) {
        sleep(10);

        for (auto &patch : g_patches) {
            uintptr_t base = getLibBase(patch.libName);
            if (!base) continue;

            uintptr_t addr = base + strtoull(patch.offsetStr, nullptr, 16);
            // Read current bytes and compare
            std::string currentHex;
            uint8_t *bytes = reinterpret_cast<uint8_t *>(addr);
            for (size_t i = 0; i < patch.expectedBytes.length() / 2; i++) {
                char hex[3];
                snprintf(hex, sizeof(hex), "%02X", bytes[i]);
                currentHex += hex;
            }

            if (currentHex != patch.expectedBytes) {
                LOGW("PATCH RESTORED: %s+%s — AC restored original bytes!",
                     patch.libName, patch.offsetStr);
                // Re-apply patch
                // TODO: Implement re-patching
            }
        }
    }
    return nullptr;
}

// ============================================================
// Safe byte read using mincore
// ============================================================
static bool safe_read_byte(uintptr_t addr, uint8_t *out) {
    // Check if page is mapped using mincore
    uintptr_t page = addr & ~(0xFFF);
    unsigned char vec;
    if (mincore(reinterpret_cast<void *>(page), 1, &vec) != 0) {
        return false;
    }
    *out = *reinterpret_cast<uint8_t *>(addr);
    return true;
}

// ============================================================
// Main Entry — Constructor attribute (auto-runs on dlopen)
// ============================================================
__attribute__((constructor))
static void lib_main() {
    LOGI("========================================");
    LOGI("WOLVES PVT BYPASS v4.0 (ARM64)");
    LOGI("Target: PUBG Mobile 4.4.0");
    LOGI("Multi-Emulator: GameLoop/LDPlayer/MEmu/Nox/BlueStacks");
    LOGI("========================================");

    // Step 1: Snapshot open FDs before any hooking
    SnapshotOpenFds();

    // Step 2: Install LIBC hooks (emulator detection bypass)
    // FIXED: Was defined but never installed in v3.8.1!
    void *libc = dlopen("libc.so", RTLD_NOW);
    if (libc) {
        auto prop_get = reinterpret_cast<int(*)(const char *, char *)>(
            dlsym(libc, "__system_property_get"));
        auto access_fn = reinterpret_cast<int(*)(const char *, int)>(dlsym(libc, "access"));
        auto fopen_fn = reinterpret_cast<FILE *(*)(const char *, const char *)>(dlsym(libc, "fopen"));
        auto stat_fn = reinterpret_cast<int(*)(const char *, struct stat *)>(dlsym(libc, "stat"));

        if (prop_get) {
            DobbyHook(reinterpret_cast<void *>(prop_get),
                      reinterpret_cast<void *>(hooked_system_property_get),
                      reinterpret_cast<void **>(&orig_system_property_get));
            LOGI("Hooked __system_property_get");
        }
        if (access_fn) {
            DobbyHook(reinterpret_cast<void *>(access_fn),
                      reinterpret_cast<void *>(hooked_access),
                      reinterpret_cast<void **>(&orig_access));
            LOGI("Hooked access");
        }
        if (fopen_fn) {
            DobbyHook(reinterpret_cast<void *>(fopen_fn),
                      reinterpret_cast<void *>(fopen_t),
                      reinterpret_cast<void **>(&orig_fopen));
            LOGI("Hooked fopen");
        }
        if (stat_fn) {
            DobbyHook(reinterpret_cast<void *>(stat_fn),
                      reinterpret_cast<void *>(stat_t),
                      reinterpret_cast<void **>(&orig_stat));
            LOGI("Hooked stat");
        }
        dlclose(libc);
    }

    // Step 3: Install CRC evasion hooks (shadow memory redirect)
    // FIXED: Was defined but never installed in v3.8.1!
    void *libc2 = dlopen("libc.so", RTLD_NOW);
    if (libc2) {
        auto memcpy_fn = reinterpret_cast<void *(*)(void *, const void *, size_t)>(
            dlsym(libc2, "memcpy"));
        auto memcmp_fn = reinterpret_cast<int(*)(const void *, const void *, size_t)>(
            dlsym(libc2, "memcmp"));

        if (memcpy_fn) {
            DobbyHook(reinterpret_cast<void *>(memcpy_fn),
                      reinterpret_cast<void *>(hooked_memcpy),
                      reinterpret_cast<void **>(&orig_memcpy));
            LOGI("Hooked memcpy (CRC evasion)");
        }
        if (memcmp_fn) {
            DobbyHook(reinterpret_cast<void *>(memcmp_fn),
                      reinterpret_cast<void *>(hooked_memcmp),
                      reinterpret_cast<void **>(&orig_memcmp));
            LOGI("Hooked memcmp (CRC evasion)");
        }
        dlclose(libc2);
    }

    // Step 4: Spawn hook installation threads
    pthread_t t_anogs, t_antiban, t_tdm, t_monitor;

    pthread_create(&t_anogs, nullptr, anogs_thread, nullptr);
    pthread_create(&t_antiban, nullptr, AntiBan_thread, nullptr);
    pthread_create(&t_tdm, nullptr, TDataMaster_thread, nullptr);
    pthread_create(&t_monitor, nullptr, PatchMonitor_thread, nullptr);

    pthread_detach(t_anogs);
    pthread_detach(t_antiban);
    pthread_detach(t_tdm);
    pthread_detach(t_monitor);

    LOGI("4 threads spawned: anogs, antiban, tdm, patchmonitor");
}
