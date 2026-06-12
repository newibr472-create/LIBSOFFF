// core/bypass.cpp — Only libanogs + libtersafe2 (UE4 NOPs removed = crash cause)
#include "bypass.h"
#include "memory.h"
#include <unistd.h>
#include <android/log.h>

#define TAG "BP"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

void bypass::Apply() {
    while (!mem::GetBase("libanogs.so")) usleep(200000);
    
    // libanogs.so — RET0/RET reporting functions
    mem::Ret0("libanogs.so", 0x1d7938);
    mem::Ret0("libanogs.so", 0x1d551c);
    mem::Ret ("libanogs.so", 0x1d624c);
    mem::Ret0("libanogs.so", 0x1d6598);
    mem::Ret0("libanogs.so", 0x1d6ea8);
    mem::Ret ("libanogs.so", 0x1d79a4);
    mem::Ret0("libanogs.so", 0x1d7fc4);
    mem::Ret ("libanogs.so", 0x1d88ec);
    mem::Ret ("libanogs.so", 0x1d417c);
    mem::Ret ("libanogs.so", 0x1d5a88);
    mem::Ret ("libanogs.so", 0x1d82cc);
    mem::Ret ("libanogs.so", 0x1d4580);
    mem::Ret ("libanogs.so", 0x1d7398);
    mem::Ret ("libanogs.so", 0x1d9024);
    mem::Ret ("libanogs.so", 0x1d4c0c);
    mem::Ret ("libanogs.so", 0x1d5030);
    mem::Ret0("libanogs.so", 0x1d78cc);
    mem::Ret ("libanogs.so", 0x1d8c74);
    LOGI("[+] anogs patched (18)");

    // libtersafe2.so
    if (mem::GetBase("libtersafe2.so")) {
        mem::Ret0("libtersafe2.so", 0x3C10);
        mem::Ret0("libtersafe2.so", 0x3E48);
        mem::Ret ("libtersafe2.so", 0x4120);
        mem::Ret0("libtersafe2.so", 0xBE80);
        mem::Ret0("libtersafe2.so", 0xC2F4);
        LOGI("[+] tersafe2 patched (5)");
    }

    // libUE4.so NOPs REMOVED — they were crashing the game
    LOGI("[+] bypass done (UE4 NOPs skipped)");
}
