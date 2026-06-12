/*
 * core/bypass.cpp — Anti-cheat bypass patches
 * BGMI 4.4.0 ARM64
 * 
 * Strategy:
 * - libanogs.so: RET or RET0 reporting/detection functions
 * - libtersafe2.so: RET key detection entry points
 * - libUE4.so: NOP specific check instructions (never RET entire functions)
 */
#include "bypass.h"
#include "memory.h"
#include <unistd.h>
#include <android/log.h>

#define TAG "BP"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

void bypass::Apply() {
    // Wait for AC libs to be fully loaded
    while (!mem::GetBase("libanogs.so")) usleep(200000);
    while (!mem::GetBase("libUE4.so")) usleep(200000);
    
    // =========================================================================
    // libanogs.so — AnoSDK report/detect functions → RET0 or RET
    // These are reporting functions that send cheat detection data to server
    // RET0 = "MOV W0, #0 + RET" (return false/no-detection)
    // RET  = just return immediately (void functions)
    // =========================================================================
    
    // AnoSDK detection report functions
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
    
    LOGI("[+] libanogs patched (18)");

    // =========================================================================
    // libtersafe2.so — Tencent AntiCheat secondary module
    // =========================================================================
    if (mem::GetBase("libtersafe2.so")) {
        mem::Ret0("libtersafe2.so", 0x3C10);   // TP_Init
        mem::Ret0("libtersafe2.so", 0x3E48);   // TP_Report
        mem::Ret ("libtersafe2.so", 0x4120);   // TP_OnTimer
        mem::Ret0("libtersafe2.so", 0xBE80);   // Memory scan entry
        mem::Ret0("libtersafe2.so", 0xC2F4);   // Process scan
        LOGI("[+] libtersafe2 patched (5)");
    }

    // =========================================================================
    // libUE4.so — NOP anti-cheat check instructions ONLY
    // NEVER RET these functions — they're game-critical
    // NOP = 0x1F2003D5 (ARM64 NOP instruction)
    // =========================================================================
    
    // Speed/Teleport detection checks
    mem::Nop("libUE4.so", 0x5B2F240);
    mem::Nop("libUE4.so", 0x5AB8298);
    mem::Nop("libUE4.so", 0x5AC01AC);
    mem::Nop("libUE4.so", 0x5ABB11C);
    mem::Nop("libUE4.so", 0x5ABB5F4);
    mem::Nop("libUE4.so", 0x5ABB6BC);
    mem::Nop("libUE4.so", 0x5ABA394);
    mem::Nop("libUE4.so", 0x5ABA9AC);
    mem::Nop("libUE4.so", 0x5AC0780);
    
    // Aim verification checks
    mem::Nop("libUE4.so", 0x5A9C868);
    mem::Nop("libUE4.so", 0x5A9D32C);
    mem::Nop("libUE4.so", 0x5A9D5E4);
    mem::Nop("libUE4.so", 0x5C9A1AC);
    mem::Nop("libUE4.so", 0x5C9A380);
    mem::Nop("libUE4.so", 0x5C9A3EC);
    
    // Memory integrity checks
    mem::Nop("libUE4.so", 0x5ABD2E8);
    mem::Nop("libUE4.so", 0x5ABDE18);
    mem::Nop("libUE4.so", 0x5ABE400);
    mem::Nop("libUE4.so", 0x5ABE498);
    mem::Nop("libUE4.so", 0x5ABE7D8);
    mem::Nop("libUE4.so", 0x5ABE868);
    mem::Nop("libUE4.so", 0x5ABECCC);
    mem::Nop("libUE4.so", 0x5ABED70);
    mem::Nop("libUE4.so", 0x5ABF110);
    mem::Nop("libUE4.so", 0x5ABF4F4);
    
    // Movement validation
    mem::Nop("libUE4.so", 0x5BD03E4);
    mem::Nop("libUE4.so", 0x5BE83B8);
    mem::Nop("libUE4.so", 0x5BE8320);
    mem::Nop("libUE4.so", 0x5BEA684);
    mem::Nop("libUE4.so", 0x5BFDA8C);
    mem::Nop("libUE4.so", 0x5C52588);
    mem::Nop("libUE4.so", 0x5C5AAC0);
    mem::Nop("libUE4.so", 0x5C603A0);
    
    // Weapon/fire rate checks
    mem::Nop("libUE4.so", 0x58716F4);
    mem::Nop("libUE4.so", 0x5872634);
    mem::Nop("libUE4.so", 0x58739E4);
    mem::Nop("libUE4.so", 0x587BD5C);
    mem::Nop("libUE4.so", 0x58806C0);
    mem::Nop("libUE4.so", 0x58AD510);
    mem::Nop("libUE4.so", 0x58DD608);
    mem::Nop("libUE4.so", 0x59AA0BC);
    mem::Nop("libUE4.so", 0x59B8DD8);
    mem::Nop("libUE4.so", 0x5A30CD8);
    
    LOGI("[+] libUE4 patched (42 NOPs)");
}
