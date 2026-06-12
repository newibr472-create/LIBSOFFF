// BGMI 4.4.0 — v3: Bypass ONLY (crash diagnosis)
// ZERO game pointer access. Only NOP/RET patches on known offsets.
// If this still crashes = bypass offsets themselves are wrong.
// If stable = game pointer access was the issue (need correct UWorld path).
#include <pthread.h>
#include <unistd.h>
#include <android/log.h>
#include "core/memory.h"
#include "core/bypass.h"

#define TAG "M"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

static void *worker(void *) {
    LOGI("[*] Waiting for libs");
    
    // Wait for game libs
    int w = 0;
    while (!mem::GetBase("libUE4.so") && w < 120) { usleep(500000); w++; }
    if (!mem::GetBase("libUE4.so")) { LOGI("[-] UE4 not found"); return nullptr; }
    LOGI("[+] UE4 = 0x%lx", (unsigned long)mem::GetBase("libUE4.so"));
    
    w = 0;
    while (!mem::GetBase("libanogs.so") && w < 60) { usleep(500000); w++; }
    if (!mem::GetBase("libanogs.so")) { LOGI("[-] anogs not found"); return nullptr; }
    LOGI("[+] anogs found");
    
    // Wait for libs to fully init their constructors
    sleep(15);
    
    // Apply bypass patches
    bypass::Apply();
    LOGI("[+] BYPASS DONE — game should be safe now");
    
    // That's it. No game pointer access. No hooks. Just patches.
    return nullptr;
}

__attribute__((constructor))
void __attribute__((used)) entry() {
    pthread_t t;
    pthread_attr_t a;
    pthread_attr_init(&a);
    pthread_attr_setdetachstate(&a, PTHREAD_CREATE_DETACHED);
    pthread_create(&t, &a, worker, nullptr);
    pthread_attr_destroy(&a);
}
