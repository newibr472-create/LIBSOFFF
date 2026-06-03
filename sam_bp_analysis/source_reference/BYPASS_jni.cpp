// ====== BYPASS.cpp Ã¢â‚¬â€ Part 1: Includes, Defines, Globals, FD Cleanup ======
#include <list>
#include <vector>
#include <string.h>
#include <pthread.h>
#include <thread>
#include <cstring>
#include <jni.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <dlfcn.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <chrono>

#include "Includes/Logger.h"
#include "Includes/obfuscate.h"
#include "Includes/Utils.h"
#include "Includes/Macros.h"
#include "KittyMemory/MemoryPatch.h"

std::vector<PatchInfo> g_patches;

#define targetLibName OBFUSCATE("libanogs.so")

#define _BYTE  uint8_t
#define _WORD  uint16_t
#define _DWORD uint32_t
#define _QWORD uint64_t

// ============= Hook function headers =============
#include "hooks/anogs_hooks.h"
#include "hooks/ue4_hooks.h"
#include "hooks/tdm_hooks.h"

// ============= UE4 Emulator Detection Bypass =============
// Stub function that returns 0 (false/not_emulator) for UE4 4.3 emulator-detect
static int ue4_emulator_detect_stub() {
    return 0; // Return 0 = not an emulator
}
static void* orig_ue4_emulator_detect = nullptr;

// ============= AetherHook-style libc hooks for emulator bypass =============
typedef int (*sys_prop_get_t)(const char*, char*);
static sys_prop_get_t orig_system_property_get = nullptr;

static int hooked_system_property_get(const char* name, char* value) {
    if (name && value) {
        // Always say we are NOT an emulator
        if (strcmp(name, "ro.kernel.qemu") == 0)               { strcpy(value, "0");           return 1; }
        if (strcmp(name, "ro.kernel.qemu.gles") == 0)          { strcpy(value, "0");           return 1; }
        if (strcmp(name, "ro.hardware") == 0)                  { strcpy(value, "qcom");        return 4; }
        if (strcmp(name, "ro.product.model") == 0)             { strcpy(value, "SM-G973F");    return 8; }
        if (strcmp(name, "ro.product.brand") == 0)             { strcpy(value, "samsung");     return 7; }
        if (strcmp(name, "ro.product.manufacturer") == 0)      { strcpy(value, "samsung");     return 7; }
        if (strcmp(name, "ro.product.device") == 0)            { strcpy(value, "beyond1");     return 7; }
        if (strcmp(name, "ro.build.fingerprint") == 0)         { strcpy(value, "samsung/beyond1qltesq/beyond1q:11/RP1A.200720.012/G973USQU8HUL3:user/release-keys"); return 90; }
        if (strcmp(name, "ro.bootloader") == 0)                { strcpy(value, "G973USQU8HUL3"); return 13; }
        if (strcmp(name, "ro.boot.hardware") == 0)             { strcpy(value, "qcom");        return 4; }
        if (strcmp(name, "ro.boot.serialno") == 0)             { strcpy(value, "R58M724D2HM"); return 11; }
        if (strcmp(name, "init.svc.qemu-props") == 0)          { value[0] = 0;                 return 0; }
        if (strcmp(name, "init.svc.qemud") == 0)               { value[0] = 0;                 return 0; }
        if (strcmp(name, "qemu.hw.mainkeys") == 0)             { value[0] = 0;                 return 0; }
        if (strcmp(name, "qemu.sf.fake_camera") == 0)          { value[0] = 0;                 return 0; }
        if (strcmp(name, "ro.boot.qemu") == 0)                 { strcpy(value, "0");           return 1; }
        if (strcmp(name, "ro.boot.serialno") == 0)             { strcpy(value, "R58M724D2HM"); return 11; }
        // Tencent/GameLoop specific
        if (strstr(name, "qemu") || strstr(name, "ttvm") ||
            strstr(name, "ranchu") || strstr(name, "goldfish")) { value[0] = 0; return 0; }
    }
    if (orig_system_property_get) return orig_system_property_get(name, value);
    if (value) value[0] = 0;
    return 0;
}

static const char* g_blocked_paths[] = {
    "/system/lib/libhoudini.so",        // Intel ARM translator (BlueStacks/MEmu)
    "/system/lib/libndk_translation.so",
    "/system/lib/arm/nb",
    "/dev/socket/qemud",
    "/dev/qemu_pipe",
    "/system/bin/qemu-props",
    "/sys/qemu_trace",
    "/proc/tty/drivers",                // emulator markers
    "/dev/socket/genyd",
    "/dev/socket/baseband_genyd",
    "/system/lib/egl/libGLES_emulation.so",
    "/dev/socket/microvirt-vbk-daemon", // MEmu
    "/data/.bluestacks.prop",
    "/system/bin/microvirtd",           // MEmu
    "/system/lib/libttvmJiff.so",       // TTVM (cloud emulator)
    "/system/lib/libttvm.so",
    "/system/lib/libGameVMP.so",        // VMP-ARM
    nullptr
};

typedef int (*access_t)(const char*, int);
static access_t orig_access = nullptr;

static int hooked_access(const char* pathname, int mode) {
    if (pathname) {
        for (const char** p = g_blocked_paths; *p; ++p) {
            if (strcmp(pathname, *p) == 0) {
                errno = ENOENT;
                return -1;  // pretend file doesn't exist
            }
        }
    }
    if (orig_access) return orig_access(pathname, mode);
    return -1;
}

typedef FILE* (*fopen_t)(const char*, const char*);
static fopen_t orig_fopen = nullptr;

static FILE* hooked_fopen(const char* path, const char* mode) {
    if (path) {
        for (const char** p = g_blocked_paths; *p; ++p) {
            if (strcmp(path, *p) == 0) { errno = ENOENT; return nullptr; }
        }
    }
    if (orig_fopen) return orig_fopen(path, mode);
    return nullptr;
}

typedef int (*stat_t)(const char*, struct stat*);
static stat_t orig_stat = nullptr;

static int hooked_stat(const char* path, struct stat* buf) {
    if (path) {
        for (const char** p = g_blocked_paths; *p; ++p) {
            if (strcmp(path, *p) == 0) { errno = ENOENT; return -1; }
        }
    }
    if (orig_stat) return orig_stat(path, buf);
    return -1;
}

// ============= FD Cleanup (Fix B) =============
#define MAX_SNAPSHOT_FDS 512
static int  g_initial_fds[MAX_SNAPSHOT_FDS];
static int  g_initial_fd_count = 0;

static void SnapshotOpenFds() {
    g_initial_fd_count = 0;
    DIR* dir = opendir("/proc/self/fd");
    if (!dir) return;
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL && g_initial_fd_count < MAX_SNAPSHOT_FDS) {
        if (entry->d_name[0] == '.') continue;
        int fd = atoi(entry->d_name);
        if (fd >= 0) g_initial_fds[g_initial_fd_count++] = fd;
    }
    closedir(dir);
}

static bool IsInitialFd(int fd) {
    for (int i = 0; i < g_initial_fd_count; i++) {
        if (g_initial_fds[i] == fd) return true;
    }
    return false;
}

static void CloseNewFds() {
    DIR* dir = opendir("/proc/self/fd");
    if (!dir) return;
    int dir_fd = dirfd(dir);
    int to_close[MAX_SNAPSHOT_FDS];
    int to_close_count = 0;
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL && to_close_count < MAX_SNAPSHOT_FDS) {
        if (entry->d_name[0] == '.') continue;
        int fd = atoi(entry->d_name);
        if (fd < 3 || fd == dir_fd || IsInitialFd(fd)) continue;
        char path[256];
        snprintf(path, sizeof(path), "/proc/self/fd/%d", fd);
        char target[256];
        ssize_t len = readlink(path, target, sizeof(target) - 1);
        if (len > 0) {
            target[len] = '\0';
            // Only close memory allocation FDs created by Dobby for trampolines.
            // MUST explicitly ignore MMKV ashmem regions which Tencent uses legitimately.
            if ((strstr(target, "ashmem") != NULL || strstr(target, "memfd") != NULL) && 
                 strstr(target, "mmkv") == NULL) {
                to_close[to_close_count++] = fd;
            }
        }
    }
    closedir(dir);
    int closed = 0;
    for (int i = 0; i < to_close_count; i++) {
        if (close(to_close[i]) == 0) closed++;
    }
    LOGI("FD_CLEANUP: snapshot=%d new_fds=%d closed=%d", g_initial_fd_count, to_close_count, closed);
}
// ====== BYPASS.cpp Ã¢â‚¬â€ Part 3: anogs_thread ======

void* anogs_thread(void*) {
    LOGI(OBFUSCATE("anogs_thread started"));
    do { sleep(1); } while (!isLibraryLoaded("libanogs.so"));

    // === Shadow memory for CRC evasion - ENABLED for testing ===
    {
        ElfScanner _anogs = ElfScanner::findElf(OBFUSCATE("libanogs.so"));
        libanogsBase = _anogs.isValid() ? (DWORD)_anogs.base() : 0;
        libanogsSize = _anogs.isValid() ? (DWORD)_anogs.loadSize() : 0;
    }
    libanogsAlloc = (libanogsSize > 0) ? (DWORD)malloc(libanogsSize) : 0;
    LOGI("Shadow memory: libanogsBase=0x%x, libanogsAlloc=0x%x, libanogsSize=%u", libanogsBase, libanogsAlloc, (unsigned)libanogsSize);
    if (libanogsSize > 0 && libanogsAlloc != 0) {
        memcpy((void*)libanogsAlloc, (void*)libanogsBase, libanogsSize);
        LOGI("Shadow memory copy completed for libanogs.so");
    } else {
        LOGI("WARNING: libanogsSize=0, shadow memory will not work!");
    }
    LOGI("Shadow memory ENABLED for libanogs.so - testing if it still causes MMKV I/O errors");

    HOOK_LIB("libanogs.so", "0x3766E0", hsub_3766E0, osub_3766E0); // Added for device info block



    // FD cleanup: sleep 5s to let game threads start and use FDs, THEN close
    // (closing Dobby trampoline FDs immediately after hook install crashes game)
    LOGI("anogs_thread: sleeping 5 seconds before FD cleanup to monitor hook calls");
    sleep(5);
    CloseNewFds(); // Re-enabled: testing with anogs_thread hooks disabled

    return NULL;
}
// ====== BYPASS.cpp Ã¢â‚¬â€ Part 4: AntiBan_thread, TDataMaster_thread, lib_main ======

void* AntiBan_thread(void*) {
    LOGI(OBFUSCATE("AntiBan_thread started"));
    do { sleep(1); } while (!isLibraryLoaded("libUE4.so"));

    // Shadow memory copy for CRC evasion - ENABLED for testing
    {
        ElfScanner _ue4 = ElfScanner::findElf(OBFUSCATE("libUE4.so"));
        libUE4Base = _ue4.isValid() ? (DWORD)_ue4.base() : 0;
        libUE4Size = _ue4.isValid() ? (DWORD)_ue4.loadSize() : 0;
    }
    libUE4Alloc = (libUE4Size > 0) ? (DWORD)malloc(libUE4Size) : 0;
    LOGI("Shadow memory: libUE4Base=0x%x, libUE4Alloc=0x%x, libUE4Size=%u", libUE4Base, libUE4Alloc, (unsigned)libUE4Size);
    if (libUE4Size > 0 && libUE4Alloc != 0) {
        memcpy((void*)libUE4Alloc, (void*)libUE4Base, libUE4Size);
        LOGI("Shadow memory copy completed for libUE4.so");
    } else {
        LOGI("WARNING: libUE4Size=0, shadow memory will not work!");
    }
    LOGI("Shadow memory ENABLED for libUE4.so - testing if shadow memory causes crash");

    // === UE4 Emulator Detection Bypass (libUE4.so + 0x427AD74) ===
    // Force UE4 4.3 emulator-detect function to return 0 (false/not_emulator)
    DobbyHook((void*)(libUE4Base + 0x427AD74), (void*)ue4_emulator_detect_stub, &orig_ue4_emulator_detect);
    LOGI("UE4 emulator-detect inline hook installed at libUE4.so + 0x427AD74");

    // === Hooks.cpp: libUE4 hooks - ALL SAFE HOOKS ENABLED (7 hooks) ===
    HOOK_LIB("libUE4.so", "0x4CD1AD8", hsub_4CD1AD8, osub_4CD1AD8); // AntiCheat Struct Init Fix - SAFE
    HOOK_LIB("libUE4.so", "0x6D064B0", hsub_6D064B0, osub_6D064B0); // AntiCheat Obfuscated Fix - SAFE
    HOOK_LIB("libUE4.so", "0x6B505C8", hsub_6B505C8, osub_6B505C8); // AntiCheat Data Parse Fix - SAFE
    HOOK_LIB("libUE4.so", "0x6B500F8", hsub_6B500F8, osub_6B500F8); // AntiCheat Data Read Fix - SAFE
    HOOK_LIB("libUE4.so", "0x6B3DED0", hsub_6B3DED0, osub_6B3DED0); // Core AntiCheat Thread Fix - SAFE
    HOOK_LIB("libUE4.so", "0x4CD33B4", hsub_4CD33B4, osub_4CD33B4); // AntiCheat Full Init Fix - SAFE
    HOOK_LIB("libUE4.so", "0x79DFEEC", hsub_79DFEEC, osub_79DFEEC); // PhysX ContactReport Init Fix - SAFE

    LOGI("AntiBan_thread: 7 UE4 hooks SAFE");
    return NULL;
}

void* TDataMaster_thread(void*) {
    LOGI(OBFUSCATE("TDataMaster_thread started"));
    do { sleep(1); } while (!isLibraryLoaded("libTDataMaster.so"));

    // === Hooks.cpp: libTDataMaster hooks - ALL SAFE (4 hooks) ===
    HOOK_LIB("libTDataMaster.so", "0x6E778", hsub_6E778, osub_6E778);   // SIGPIPE handler bypass - SAFE
    HOOK_LIB("libTDataMaster.so", "0x6CB8C", hsub_6CB8C, osub_6CB8C);   // Report packet terminator - SAFE
    HOOK_LIB("libTDataMaster.so", "0x1B4CC", hTDM_GetInstance, oTDM_GetInstance); // SAFE
    HOOK_LIB("libTDataMaster.so", "0x248E0", hsub_248E0, osub_248E0);   // AC config init bypass - SAFE

    // === Hooks.cpp: libgcloud hooks - ALL SAFE (5 hooks) ===
    HOOK_LIB("libgcloud.so", "0x1B4D8", hPluginReport_D2, oPluginReport_D2); // SAFE
    HOOK_LIB("libgcloud.so", "0x1B4E4", hPluginReport_D0, oPluginReport_D0); // SAFE
    HOOK_LIB("libgcloud.so", "0x1B514", hOnPostStartup, oOnPostStartup); // SAFE
    HOOK_LIB("libgcloud.so", "0x1B520", hOnPreShutdown, oOnPreShutdown); // SAFE
    HOOK_LIB("libgcloud.so", "0x1B52C", hOnShutdown, oOnShutdown); // SAFE

    LOGI("TDataMaster_thread: ALL HOOKS SAFE - 4 TDM + 5 gcloud (9 hooks)");
    return NULL;
}

// BUG5 FIX: safe_read_byte - use mincore to check page is resident before reading
// Avoids SIGSEGV when AC unloads a section after we cached its address
static bool safe_read_byte(uintptr_t addr, uint8_t* out) {
    // mincore: check if the page containing addr is in memory
    unsigned char vec = 0;
    uintptr_t page = addr & ~(uintptr_t)(4095);
    if (mincore((void*)page, 1, &vec) != 0) return false; // page not mapped
    if (!(vec & 1)) return false; // page not resident
    *out = *(volatile uint8_t*)addr;
    return true;
}

void* PatchMonitor_thread(void*) {
    LOGI(OBFUSCATE("PatchMonitor_thread started - watching patches for anti-cheat restoration"));
    while (true) {
        sleep(10);
        if (g_patches.empty()) continue;

        for (const auto& p : g_patches) {
            bool destroyed = false;
            bool page_gone = false;
            for (size_t i = 0; i < p.expectedBytes.size(); i++) {
                uint8_t currentByte = 0;
                if (!safe_read_byte(p.targetAddr + i, &currentByte)) {
                    page_gone = true;
                    break;
                }
                if (currentByte != p.expectedBytes[i]) {
                    destroyed = true;
                    LOGE("[MONITOR] BYTE MISMATCH %s+0x%lX byte[%zu]: expected 0x%02X got 0x%02X",
                         p.libName.c_str(), (unsigned long)p.offset, i,
                         p.expectedBytes[i], currentByte);
                    break;
                }
            }
            if (page_gone) {
                LOGW("[MONITOR] PAGE UNMAPPED %s+0x%lX - library section gone",
                     p.libName.c_str(), (unsigned long)p.offset);
            } else if (destroyed) {
                LOGE("[MONITOR] PATCH DESTROYED %s+0x%lX - AC restored original bytes!",
                     p.libName.c_str(), (unsigned long)p.offset);
            }
        }
    }
    return NULL;
}

__attribute__((constructor))
void lib_main()
{
    // Fix B: snapshot FDs BEFORE any init runs
    SnapshotOpenFds();

    LOGI(OBFUSCATE("WOLVES BYPASS loaded â€” 3-thread architecture"));


    pthread_t ptid;
    pthread_create(&ptid, NULL, anogs_thread, NULL);
    pthread_create(&ptid, NULL, AntiBan_thread, NULL);
    pthread_create(&ptid, NULL, TDataMaster_thread, NULL);
    pthread_create(&ptid, NULL, PatchMonitor_thread, NULL);
}
