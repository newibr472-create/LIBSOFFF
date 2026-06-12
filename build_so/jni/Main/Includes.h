#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <cstdint>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <dlfcn.h>
#include <android/log.h>
#include <pthread.h>
#include <libgen.h>
#include <inttypes.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "BGMI", __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "BGMI", __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, "BGMI", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "BGMI", __VA_ARGS__)

typedef unsigned long DWORD;

inline bool isLibraryLoaded(const char* libraryName) {
    char line[512] = {0};
    FILE* fp = fopen("/proc/self/maps", "r");
    if (!fp) return false;
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, libraryName)) {
            fclose(fp);
            return true;
        }
    }
    fclose(fp);
    return false;
}

inline uintptr_t findLibrary(const char* libraryName) {
    char line[512] = {0};
    FILE* fp = fopen("/proc/self/maps", "r");
    if (!fp) return 0;
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, libraryName)) {
            uintptr_t base = 0;
            sscanf(line, "%" PRIXPTR, &base);
            fclose(fp);
            return base;
        }
    }
    fclose(fp);
    return 0;
}

inline uintptr_t string2Offset(const char* c) {
    int base = 16;
    if (c[0] == '0' && (c[1] == 'x' || c[1] == 'X'))
        return (uintptr_t)strtoul(c + 2, nullptr, base);
    return (uintptr_t)strtoul(c, nullptr, base);
}

#define OBFUSCATE(x) x
