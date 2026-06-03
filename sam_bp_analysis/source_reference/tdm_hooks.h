#pragma once
#include "../Includes/Logger.h"
#include "../Includes/Utils.h"
#include "../Includes/obfuscate.h"

// ============= libTDataMaster.so + libgcloud.so hooks =============

int (*osub_6E778)(int, int);
int hsub_6E778(int result, int a2) {
    if (a2) *(_BYTE *)(a2 + 16) = 1;
    if (result) *(_BYTE *)(result + 1252) = 1;
    return result;
}

_DWORD* (*osub_6CB8C)(const char*, _BYTE*, char);
_DWORD* hsub_6CB8C(const char* a1, _BYTE* a2, char a3) {
    if (a2) *a2 = 1;
    return 0;
}

int (*oTDM_GetInstance)();
int hTDM_GetInstance() {
    // Block: return 0 to disable TDM GetInstance
    return 0;
}

void (*oPluginReport_D2)();
void hPluginReport_D2() {
    // Block: suppress PluginReport D2 destructor reporting
}

void (*oPluginReport_D0)();
void hPluginReport_D0() {
    // Block: suppress PluginReport D0 destructor reporting
}

int (*oOnPostStartup)();
int hOnPostStartup() {
    // Block: suppress OnPostStartup reporting
    return 0;
}

int (*oOnPreShutdown)();
int hOnPreShutdown() {
    // Block: suppress OnPreShutdown reporting
    return 0;
}

int (*oOnShutdown)();
int hOnShutdown() {
    // Block: suppress OnShutdown reporting
    return 0;
}

int (*osub_248E0)(int);
int hsub_248E0(int result) {
    if (result) {
        *(_DWORD *)(result + 12) = 0;
        *(_DWORD *)(result + 44) = 999;
        *(_DWORD *)(result + 60) = 9999;
        *(_DWORD *)(result + 64) = 999;
        *(_DWORD *)(result + 76) = 99999;
    }
    return result;
}
