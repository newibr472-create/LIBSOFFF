#ifndef _ANDROID_LOG_H
#define _ANDROID_LOG_H

#include <stdio.h>
#include <stdarg.h>

typedef enum {
    ANDROID_LOG_UNKNOWN = 0,
    ANDROID_LOG_DEFAULT,
    ANDROID_LOG_VERBOSE,
    ANDROID_LOG_DEBUG,
    ANDROID_LOG_INFO,
    ANDROID_LOG_WARN,
    ANDROID_LOG_ERROR,
    ANDROID_LOG_FATAL,
    ANDROID_LOG_SILENT
} android_LogPriority;

// On real Android, this links to liblog.so
// For cross-compilation we provide a weak implementation
__attribute__((weak))
int __android_log_print(int prio, const char* tag, const char* fmt, ...) {
    // This will be resolved at runtime on Android from liblog.so
    // During compile we just need the symbol
    return 0;
}

#endif
