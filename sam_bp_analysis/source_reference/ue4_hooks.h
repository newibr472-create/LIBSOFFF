#pragma once
#include "../Includes/Logger.h"
#include "../Includes/Utils.h"

extern DWORD libUE4Base;

// ============= libUE4.so hook functions =============

int (**(*osub_4CD1AD8)(int a1, _DWORD *a2))();
int (**hsub_4CD1AD8(int a1, _DWORD *a2))() { return 0; }

int (*osub_4CD33B4)(int a1);
int hsub_4CD33B4(int a1) { return a1; }

void (*osub_6B3DED0)(int a1, int a2, int a3, int a4, ...);
void hsub_6B3DED0(int a1, int a2, int a3, int a4, ...) { return; }

int (*osub_6B500F8)(int a1, int *a2);
int hsub_6B500F8(int a1, int *a2) {
    if (!a2) return a1;
    *a2 = 0;
    *((uint8_t*)a2 + 4) = 0;
    *((uint8_t*)a2 + 5) = 0;
    *((uint8_t*)a2 + 6) = 0;
    *((uint8_t*)a2 + 7) = 0;
    *((_WORD*)(a2 + 2)) = 0;
    *(a2 + 5) = 0;
    return a1;
}

int (*osub_6B505C8)(int a1, uint8_t *a2);
int hsub_6B505C8(int a1, uint8_t *a2) {
    if (!a2) return a1;
    a2[12] = 0; a2[13] = 0; a2[14] = 0; a2[15] = 0;
    *((_WORD *)a2 + 8) = 0;
    a2[16] = 0; a2[17] = 0;
    return a1;
}

int (*osub_6D064B0)(
    int (**a1)(char *, int, int, _DWORD, int, int, int, int, void *, int, int),
    int a2, int a3, _DWORD *a4, int a5, int a6, int a7, int a8,
    void *dest, int a10, int a11, int a12, int a13, int a14, int a15,
    int a16, int a17, int a18, int a19, void *a20, int a21, int a22,
    int a23, int a24, int a25, int a26, int a27, int a28, int a29,
    int a30, int a31, int a32, int a33, int a34, int a35, int a36);
int hsub_6D064B0(
    int (**a1)(char *, int, int, _DWORD, int, int, int, int, void *, int, int),
    int a2, int a3, _DWORD *a4, int a5, int a6, int a7, int a8,
    void *dest, int a10, int a11, int a12, int a13, int a14, int a15,
    int a16, int a17, int a18, int a19, void *a20, int a21, int a22,
    int a23, int a24, int a25, int a26, int a27, int a28, int a29,
    int a30, int a31, int a32, int a33, int a34, int a35, int a36) {
    return 0;
}

_DWORD *(*osub_79DFEEC)(_DWORD *a1);
_DWORD *hsub_79DFEEC(_DWORD *a1) {
    ((void(*)(_DWORD *))(libUE4Base + 0x79DFC80))(a1);
    a1[85] = 51;  a1[84] = (uintptr_t)"LinearDamping";
    a1[86] = (uintptr_t)(libUE4Base + 0x79DEE34);
    a1[89] = 52;  a1[88] = (uintptr_t)"AngularDamping";
    a1[87] = (uintptr_t)(libUE4Base + 0x79DEE28);
    a1[90] = (uintptr_t)(libUE4Base + 0x79DEE4C);
    a1[93] = 53;
    a1[91] = (uintptr_t)(libUE4Base + 0x79DEE40);
    a1[92] = (uintptr_t)"MaxAngularVelocity";
    a1[94] = (uintptr_t)(libUE4Base + 0x79DEE64);
    a1[96] = (uintptr_t)(libUE4Base + 0x238FF3C);
    a1[97] = 54;  a1[95] = (uintptr_t)(libUE4Base + 0x79DEE58);
    a1[100] = 55; a1[98] = (uintptr_t)(libUE4Base + 0x79DEE70);
    a1[99] = (uintptr_t)"SleepThreshold";
    a1[101] = (uintptr_t)(libUE4Base + 0x79DEE88);
    a1[104] = 56; a1[103] = (uintptr_t)"StabilizationThreshold";
    a1[102] = (uintptr_t)(libUE4Base + 0x79DEE7C);
    a1[105] = (uintptr_t)(libUE4Base + 0x79DEE94);
    a1[106] = (uintptr_t)(libUE4Base + 0x7867A24);
    a1[108] = 57; a1[107] = (uintptr_t)"RigidDynamicLockFlags";
    a1[109] = (uintptr_t)(libUE4Base + 0x79DEEC8);
    a1[111] = (uintptr_t)"WakeCounter";
    a1[112] = 58; a1[110] = (uintptr_t)(libUE4Base + 0x79DEEA0);
    a1[113] = (uintptr_t)(libUE4Base + 0x79DEEF0);
    a1[116] = 59; a1[117] = (uintptr_t)(libUE4Base + 0x238F5A0);
    a1[115] = (uintptr_t)(libUE4Base + 0x238EB7C);
    a1[114] = (uintptr_t)(libUE4Base + 0x79DEEE4);
    a1[118] = (uintptr_t)"minVelocityIters";
    a1[119] = (uintptr_t)(libUE4Base + 0x79DEEFC);
    a1[122] = 60; a1[121] = (uintptr_t)"ContactReportThreshold";
    a1[120] = (uintptr_t)(libUE4Base + 0x79DEF08);
    a1[123] = (uintptr_t)(libUE4Base + 0x79DEF20);
    a1[125] = (uintptr_t)"ConcreteTypeName";
    a1[126] = 61; a1[124] = (uintptr_t)(libUE4Base + 0x79DEF14);
    a1[127] = (uintptr_t)(libUE4Base + 0x79DEBA8);
    return a1;
}
