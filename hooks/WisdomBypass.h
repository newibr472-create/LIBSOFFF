#pragma once
#include <cstdint>

Xynnacore(int64, sub_2F4F08, (int64 a1, int64 a2))
{
    if (a1) return 0;
    return osub_2F4F08(a1, a2);
}
HOOK_LIB("libanogs.so", "0x2F4F08", hsub_2F4F08, osub_2F4F08);

Xynnacore(int64, sub_330498, (int64 a1, int a2))
{
    return 0;
}
HOOK_LIB("libanogs.so", "0x330498", hsub_330498, osub_330498);

Xynnacore(int64, sub_335D20, (int64 a1, int64 a2, int64 a3, int64 a4))
{
    return 0;
}
HOOK_LIB("libanogs.so", "0x335D20", hsub_335D20, osub_335D20);

Xynnacore(int64, sub_369658, (int64 a1))
{
    if (a1) *(_BYTE *)(a1 + 0x38) = 0;
    return 0;
}
HOOK_LIB("libanogs.so", "0x369658", hsub_369658, osub_369658);

Xynnacore(int64, sub_39F570, (int64 a1, int a2))
{
    return 0;
}
HOOK_LIB("libanogs.so", "0x39F570", hsub_39F570, osub_39F570);

Xynnacore(int64, sub_3A5650, (int64 a1, int a2))
{
    return 0;
}
HOOK_LIB("libanogs.so", "0x3A5650", hsub_3A5650, osub_3A5650);

Xynnacore(int64, sub_436950, (int64 a1, int a2, int64 a3))
{
    return 0;
}
HOOK_LIB("libanogs.so", "0x436950", hsub_436950, osub_436950);

Xynnacore(int, sub_4D4C98, (int a1, int a2))
{
    return 0;
}
HOOK_LIB("libanogs.so", "0x4D4C98", hsub_4D4C98, osub_4D4C98);

Xynnacore(int, sub_4DCBE0, (int64 a1, int64 a2, int64 a3, int64 a4, int a5))
{
    return 0;
}
HOOK_LIB("libanogs.so", "0x4DCBE0", hsub_4DCBE0, osub_4DCBE0);

Xynnacore(int, sub_749F538, (int64 a1, int64 a2))
{
    return 0;
}
HOOK_LIB("libUE4.so", "0x749F538", hsub_749F538, osub_749F538);

Xynnacore(int64, sub_7AD8950, (int64 a1, int64 a2))
{
    return 0;
}
HOOK_LIB("libUE4.so", "0x7AD8950", hsub_7AD8950, osub_7AD8950);

Xynnacore(int, sub_A4FBA10, (int64 a1, int64 a2, int a3, int64 a4))
{
    return 0;
}
HOOK_LIB("libUE4.so", "0xA4FBA10", hsub_A4FBA10, osub_A4FBA10);

FULL_PATCH("libUE4.so", "0x5ACC184", 4, 0xD503201F);

FULL_PATCH("libUE4.so", "0x62E286C", 4, 0xD503201F);

PARTIAL_PATCH("libUE4.so", "0x68CE7EC", 4, 0xD503201F);

PARTIAL_PATCH("libUE4.so", "0x68CE7F4", 4, 0xD503201F);

FULL_PATCH("libUE4.so", "0x7A649A8", 4, 0xD503201F);
