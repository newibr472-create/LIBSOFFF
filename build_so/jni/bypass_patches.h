// AUTO-GENERATED from FINAL_BYPASS.h — 808 patches
#pragma once
#include "stealth.h"

// libanogs.so: 18 patches
static const uint8_t b_anogs_0[] = {0x00, 0x00, 0x80, 0x52, 0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_anogs_1[] = {0x00, 0x00, 0x80, 0x52, 0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_anogs_2[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_anogs_3[] = {0x00, 0x00, 0x80, 0x52, 0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_anogs_4[] = {0x00, 0x00, 0x80, 0x52, 0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_anogs_5[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_anogs_6[] = {0x00, 0x00, 0x80, 0x52, 0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_anogs_7[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_anogs_8[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_anogs_9[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_anogs_10[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_anogs_11[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_anogs_12[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_anogs_13[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_anogs_14[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_anogs_15[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_anogs_16[] = {0x00, 0x00, 0x80, 0x52, 0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_anogs_17[] = {0xC0, 0x03, 0x5F, 0xD6};

static stealth::Patch patches_anogs[] = {
    {"libanogs.so", 0x1d7938, b_anogs_0, 8, 0},
    {"libanogs.so", 0x1d551c, b_anogs_1, 8, 1},
    {"libanogs.so", 0x1d624c, b_anogs_2, 4, 2},
    {"libanogs.so", 0x1d6598, b_anogs_3, 8, 3},
    {"libanogs.so", 0x1d6ea8, b_anogs_4, 8, 4},
    {"libanogs.so", 0x1d79a4, b_anogs_5, 4, 5},
    {"libanogs.so", 0x1d7fc4, b_anogs_6, 8, 6},
    {"libanogs.so", 0x1d88ec, b_anogs_7, 4, 7},
    {"libanogs.so", 0x1d417c, b_anogs_8, 4, 8},
    {"libanogs.so", 0x1d5a88, b_anogs_9, 4, 9},
    {"libanogs.so", 0x1d82cc, b_anogs_10, 4, 10},
    {"libanogs.so", 0x1d4580, b_anogs_11, 4, 11},
    {"libanogs.so", 0x1d7398, b_anogs_12, 4, 12},
    {"libanogs.so", 0x1d9024, b_anogs_13, 4, 13},
    {"libanogs.so", 0x1d4c0c, b_anogs_14, 4, 14},
    {"libanogs.so", 0x1d5030, b_anogs_15, 4, 15},
    {"libanogs.so", 0x1d78cc, b_anogs_16, 8, 16},
    {"libanogs.so", 0x1d8c74, b_anogs_17, 4, 17},
};

// libsigner.so: 2 patches
static const uint8_t b_signer_0[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_signer_1[] = {0xC0, 0x03, 0x5F, 0xD6};

static stealth::Patch patches_signer[] = {
    {"libsigner.so", 0x9a088, b_signer_0, 4, 18},
    {"libsigner.so", 0x9afc0, b_signer_1, 4, 19},
};

// libhdmpvecore.so: 381 patches
static const uint8_t b_hdmpvecore_0[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_1[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_2[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_3[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_4[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_5[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_6[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_7[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_8[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_9[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_10[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_11[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_12[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_13[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_14[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_15[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_16[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_17[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_18[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_19[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_20[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_21[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_22[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_23[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_24[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_25[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_26[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_27[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_28[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_29[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_30[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_31[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_32[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_33[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_34[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_35[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_36[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_37[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_38[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_39[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_40[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_41[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_42[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_43[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_44[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_45[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_46[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_47[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_48[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_49[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_50[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_51[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_52[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_53[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_54[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_55[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_56[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_57[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_58[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_59[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_60[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_61[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_62[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_63[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_64[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_65[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_66[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_67[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_68[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_69[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_70[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_71[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_72[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_73[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_74[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_75[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_76[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_77[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_78[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_79[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_80[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_81[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_82[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_83[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_84[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_85[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_86[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_87[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_88[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_89[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_90[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_91[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_92[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_93[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_94[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_95[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_96[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_97[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_98[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_99[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_100[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_101[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_102[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_103[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_104[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_105[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_106[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_107[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_108[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_109[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_110[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_111[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_112[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_113[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_114[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_115[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_116[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_117[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_118[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_119[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_120[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_121[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_122[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_123[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_124[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_125[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_126[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_127[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_128[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_129[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_130[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_131[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_132[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_133[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_134[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_135[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_136[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_137[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_138[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_139[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_140[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_141[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_142[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_143[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_144[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_145[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_146[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_147[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_148[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_149[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_150[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_151[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_152[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_153[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_154[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_155[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_156[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_157[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_158[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_159[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_160[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_161[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_162[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_163[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_164[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_165[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_166[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_167[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_168[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_169[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_170[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_171[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_172[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_173[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_174[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_175[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_176[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_177[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_178[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_179[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_180[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_181[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_182[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_183[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_184[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_185[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_186[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_187[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_188[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_189[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_190[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_191[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_192[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_193[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_194[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_195[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_196[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_197[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_198[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_199[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_200[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_201[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_202[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_203[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_204[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_205[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_206[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_207[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_208[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_209[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_210[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_211[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_212[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_213[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_214[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_215[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_216[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_217[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_218[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_219[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_220[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_221[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_222[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_223[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_224[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_225[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_226[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_227[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_228[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_229[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_230[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_231[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_232[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_233[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_234[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_235[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_236[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_237[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_238[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_239[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_240[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_241[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_242[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_243[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_244[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_245[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_246[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_247[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_248[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_249[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_250[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_251[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_252[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_253[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_254[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_255[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_256[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_257[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_258[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_259[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_260[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_261[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_262[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_263[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_264[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_265[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_266[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_267[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_268[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_269[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_270[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_271[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_272[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_273[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_274[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_275[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_276[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_277[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_278[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_279[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_280[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_281[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_282[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_283[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_284[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_285[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_286[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_287[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_288[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_289[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_290[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_291[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_292[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_293[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_294[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_295[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_296[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_297[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_298[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_299[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_300[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_301[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_302[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_303[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_304[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_305[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_306[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_307[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_308[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_309[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_310[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_311[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_312[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_313[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_314[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_315[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_316[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_317[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_318[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_319[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_320[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_321[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_322[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_323[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_324[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_325[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_326[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_327[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_328[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_329[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_330[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_331[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_332[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_333[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_334[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_335[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_336[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_337[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_338[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_339[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_340[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_341[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_342[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_343[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_344[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_345[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_346[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_347[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_348[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_349[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_350[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_351[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_352[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_353[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_354[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_355[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_356[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_357[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_358[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_359[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_360[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_361[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_362[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_363[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_364[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_365[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_366[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_367[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_368[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_369[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_370[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_371[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_372[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_373[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_374[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_375[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_376[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_377[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_378[] = {0x00, 0x00, 0x80, 0x52, 0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_379[] = {0x00, 0x00, 0x80, 0x52, 0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpvecore_380[] = {0x20, 0x00, 0x80, 0x52, 0xC0, 0x03, 0x5F, 0xD6};

static stealth::Patch patches_hdmpvecore[] = {
    {"libhdmpvecore.so", 0x7cc84, b_hdmpvecore_0, 4, 20},
    {"libhdmpvecore.so", 0x68058, b_hdmpvecore_1, 4, 21},
    {"libhdmpvecore.so", 0x9abd8, b_hdmpvecore_2, 4, 22},
    {"libhdmpvecore.so", 0x642b4, b_hdmpvecore_3, 4, 23},
    {"libhdmpvecore.so", 0x642e8, b_hdmpvecore_4, 4, 24},
    {"libhdmpvecore.so", 0x6d70c, b_hdmpvecore_5, 4, 25},
    {"libhdmpvecore.so", 0x547e8, b_hdmpvecore_6, 4, 26},
    {"libhdmpvecore.so", 0x6fc88, b_hdmpvecore_7, 4, 27},
    {"libhdmpvecore.so", 0x75b7c, b_hdmpvecore_8, 4, 28},
    {"libhdmpvecore.so", 0x7eaa8, b_hdmpvecore_9, 4, 29},
    {"libhdmpvecore.so", 0x69074, b_hdmpvecore_10, 4, 30},
    {"libhdmpvecore.so", 0x6df7c, b_hdmpvecore_11, 4, 31},
    {"libhdmpvecore.so", 0x7ecbc, b_hdmpvecore_12, 4, 32},
    {"libhdmpvecore.so", 0x7b080, b_hdmpvecore_13, 4, 33},
    {"libhdmpvecore.so", 0x6d14c, b_hdmpvecore_14, 4, 34},
    {"libhdmpvecore.so", 0x665b4, b_hdmpvecore_15, 4, 35},
    {"libhdmpvecore.so", 0x6db78, b_hdmpvecore_16, 4, 36},
    {"libhdmpvecore.so", 0x7436c, b_hdmpvecore_17, 4, 37},
    {"libhdmpvecore.so", 0x63078, b_hdmpvecore_18, 4, 38},
    {"libhdmpvecore.so", 0x6e514, b_hdmpvecore_19, 4, 39},
    {"libhdmpvecore.so", 0x942b4, b_hdmpvecore_20, 4, 40},
    {"libhdmpvecore.so", 0x94c50, b_hdmpvecore_21, 4, 41},
    {"libhdmpvecore.so", 0x709c8, b_hdmpvecore_22, 4, 42},
    {"libhdmpvecore.so", 0x74d30, b_hdmpvecore_23, 4, 43},
    {"libhdmpvecore.so", 0x94178, b_hdmpvecore_24, 4, 44},
    {"libhdmpvecore.so", 0x7d58c, b_hdmpvecore_25, 4, 45},
    {"libhdmpvecore.so", 0x54a34, b_hdmpvecore_26, 4, 46},
    {"libhdmpvecore.so", 0x6f69c, b_hdmpvecore_27, 4, 47},
    {"libhdmpvecore.so", 0x65298, b_hdmpvecore_28, 4, 48},
    {"libhdmpvecore.so", 0x97420, b_hdmpvecore_29, 4, 49},
    {"libhdmpvecore.so", 0x627b8, b_hdmpvecore_30, 4, 50},
    {"libhdmpvecore.so", 0x765e4, b_hdmpvecore_31, 4, 51},
    {"libhdmpvecore.so", 0x97b08, b_hdmpvecore_32, 4, 52},
    {"libhdmpvecore.so", 0x75904, b_hdmpvecore_33, 4, 53},
    {"libhdmpvecore.so", 0x7579c, b_hdmpvecore_34, 4, 54},
    {"libhdmpvecore.so", 0x5e318, b_hdmpvecore_35, 4, 55},
    {"libhdmpvecore.so", 0x6d660, b_hdmpvecore_36, 4, 56},
    {"libhdmpvecore.so", 0x63208, b_hdmpvecore_37, 4, 57},
    {"libhdmpvecore.so", 0x7b6e4, b_hdmpvecore_38, 4, 58},
    {"libhdmpvecore.so", 0x9a460, b_hdmpvecore_39, 4, 59},
    {"libhdmpvecore.so", 0x626ac, b_hdmpvecore_40, 4, 60},
    {"libhdmpvecore.so", 0x9703c, b_hdmpvecore_41, 4, 61},
    {"libhdmpvecore.so", 0x6f8b0, b_hdmpvecore_42, 4, 62},
    {"libhdmpvecore.so", 0x80040, b_hdmpvecore_43, 4, 63},
    {"libhdmpvecore.so", 0x6d900, b_hdmpvecore_44, 4, 64},
    {"libhdmpvecore.so", 0x9a330, b_hdmpvecore_45, 4, 65},
    {"libhdmpvecore.so", 0x98804, b_hdmpvecore_46, 4, 66},
    {"libhdmpvecore.so", 0x79944, b_hdmpvecore_47, 4, 67},
    {"libhdmpvecore.so", 0x9a1e8, b_hdmpvecore_48, 4, 68},
    {"libhdmpvecore.so", 0x78628, b_hdmpvecore_49, 4, 69},
    {"libhdmpvecore.so", 0x844d8, b_hdmpvecore_50, 4, 70},
    {"libhdmpvecore.so", 0x97300, b_hdmpvecore_51, 4, 71},
    {"libhdmpvecore.so", 0x4acc4, b_hdmpvecore_52, 4, 72},
    {"libhdmpvecore.so", 0x66648, b_hdmpvecore_53, 4, 73},
    {"libhdmpvecore.so", 0x654d4, b_hdmpvecore_54, 4, 74},
    {"libhdmpvecore.so", 0x7cd34, b_hdmpvecore_55, 4, 75},
    {"libhdmpvecore.so", 0x65d30, b_hdmpvecore_56, 4, 76},
    {"libhdmpvecore.so", 0x7580c, b_hdmpvecore_57, 4, 77},
    {"libhdmpvecore.so", 0x63fd8, b_hdmpvecore_58, 4, 78},
    {"libhdmpvecore.so", 0x66e78, b_hdmpvecore_59, 4, 79},
    {"libhdmpvecore.so", 0x680dc, b_hdmpvecore_60, 4, 80},
    {"libhdmpvecore.so", 0x7c8b4, b_hdmpvecore_61, 4, 81},
    {"libhdmpvecore.so", 0x62530, b_hdmpvecore_62, 4, 82},
    {"libhdmpvecore.so", 0x6d72c, b_hdmpvecore_63, 4, 83},
    {"libhdmpvecore.so", 0x6fc40, b_hdmpvecore_64, 4, 84},
    {"libhdmpvecore.so", 0x664b4, b_hdmpvecore_65, 4, 85},
    {"libhdmpvecore.so", 0x66220, b_hdmpvecore_66, 4, 86},
    {"libhdmpvecore.so", 0x7ed04, b_hdmpvecore_67, 4, 87},
    {"libhdmpvecore.so", 0x62534, b_hdmpvecore_68, 4, 88},
    {"libhdmpvecore.so", 0x6fea8, b_hdmpvecore_69, 4, 89},
    {"libhdmpvecore.so", 0x66dec, b_hdmpvecore_70, 4, 90},
    {"libhdmpvecore.so", 0x785e4, b_hdmpvecore_71, 4, 91},
    {"libhdmpvecore.so", 0x7eb1c, b_hdmpvecore_72, 4, 92},
    {"libhdmpvecore.so", 0x980b8, b_hdmpvecore_73, 4, 93},
    {"libhdmpvecore.so", 0x7daf4, b_hdmpvecore_74, 4, 94},
    {"libhdmpvecore.so", 0x75c8c, b_hdmpvecore_75, 4, 95},
    {"libhdmpvecore.so", 0x6db80, b_hdmpvecore_76, 4, 96},
    {"libhdmpvecore.so", 0x958dc, b_hdmpvecore_77, 4, 97},
    {"libhdmpvecore.so", 0x666ec, b_hdmpvecore_78, 4, 98},
    {"libhdmpvecore.so", 0x63ebc, b_hdmpvecore_79, 4, 99},
    {"libhdmpvecore.so", 0x61610, b_hdmpvecore_80, 4, 100},
    {"libhdmpvecore.so", 0x65680, b_hdmpvecore_81, 4, 101},
    {"libhdmpvecore.so", 0x752a8, b_hdmpvecore_82, 4, 102},
    {"libhdmpvecore.so", 0x7eb04, b_hdmpvecore_83, 4, 103},
    {"libhdmpvecore.so", 0x9a120, b_hdmpvecore_84, 4, 104},
    {"libhdmpvecore.so", 0x6f304, b_hdmpvecore_85, 4, 105},
    {"libhdmpvecore.so", 0x6fb40, b_hdmpvecore_86, 4, 106},
    {"libhdmpvecore.so", 0x94bb4, b_hdmpvecore_87, 4, 107},
    {"libhdmpvecore.so", 0x7f918, b_hdmpvecore_88, 4, 108},
    {"libhdmpvecore.so", 0x6252c, b_hdmpvecore_89, 4, 109},
    {"libhdmpvecore.so", 0x80c14, b_hdmpvecore_90, 4, 110},
    {"libhdmpvecore.so", 0x84664, b_hdmpvecore_91, 4, 111},
    {"libhdmpvecore.so", 0x6e580, b_hdmpvecore_92, 4, 112},
    {"libhdmpvecore.so", 0x9a990, b_hdmpvecore_93, 4, 113},
    {"libhdmpvecore.so", 0x65b5c, b_hdmpvecore_94, 4, 114},
    {"libhdmpvecore.so", 0x63df4, b_hdmpvecore_95, 4, 115},
    {"libhdmpvecore.so", 0x78488, b_hdmpvecore_96, 4, 116},
    {"libhdmpvecore.so", 0x66cb4, b_hdmpvecore_97, 4, 117},
    {"libhdmpvecore.so", 0x5e348, b_hdmpvecore_98, 4, 118},
    {"libhdmpvecore.so", 0x5e8a8, b_hdmpvecore_99, 4, 119},
    {"libhdmpvecore.so", 0x6de10, b_hdmpvecore_100, 4, 120},
    {"libhdmpvecore.so", 0x7588c, b_hdmpvecore_101, 4, 121},
    {"libhdmpvecore.so", 0x768a4, b_hdmpvecore_102, 4, 122},
    {"libhdmpvecore.so", 0x7b738, b_hdmpvecore_103, 4, 123},
    {"libhdmpvecore.so", 0x64bf4, b_hdmpvecore_104, 4, 124},
    {"libhdmpvecore.so", 0x71484, b_hdmpvecore_105, 4, 125},
    {"libhdmpvecore.so", 0x7d38c, b_hdmpvecore_106, 4, 126},
    {"libhdmpvecore.so", 0x9a2dc, b_hdmpvecore_107, 4, 127},
    {"libhdmpvecore.so", 0x7eb10, b_hdmpvecore_108, 4, 128},
    {"libhdmpvecore.so", 0x7bd24, b_hdmpvecore_109, 4, 129},
    {"libhdmpvecore.so", 0x925b8, b_hdmpvecore_110, 4, 130},
    {"libhdmpvecore.so", 0x80108, b_hdmpvecore_111, 4, 131},
    {"libhdmpvecore.so", 0x73368, b_hdmpvecore_112, 4, 132},
    {"libhdmpvecore.so", 0x9a15c, b_hdmpvecore_113, 4, 133},
    {"libhdmpvecore.so", 0x76a6c, b_hdmpvecore_114, 4, 134},
    {"libhdmpvecore.so", 0x71ea4, b_hdmpvecore_115, 4, 135},
    {"libhdmpvecore.so", 0x7579c, b_hdmpvecore_116, 4, 136},
    {"libhdmpvecore.so", 0x7570c, b_hdmpvecore_117, 4, 137},
    {"libhdmpvecore.so", 0x944c0, b_hdmpvecore_118, 4, 138},
    {"libhdmpvecore.so", 0x664e4, b_hdmpvecore_119, 4, 139},
    {"libhdmpvecore.so", 0x69cb4, b_hdmpvecore_120, 4, 140},
    {"libhdmpvecore.so", 0x66464, b_hdmpvecore_121, 4, 141},
    {"libhdmpvecore.so", 0x7ed4c, b_hdmpvecore_122, 4, 142},
    {"libhdmpvecore.so", 0x7cd80, b_hdmpvecore_123, 4, 143},
    {"libhdmpvecore.so", 0x7eb28, b_hdmpvecore_124, 4, 144},
    {"libhdmpvecore.so", 0x9a1b0, b_hdmpvecore_125, 4, 145},
    {"libhdmpvecore.so", 0x7c9a8, b_hdmpvecore_126, 4, 146},
    {"libhdmpvecore.so", 0x6b5e0, b_hdmpvecore_127, 4, 147},
    {"libhdmpvecore.so", 0x763e4, b_hdmpvecore_128, 4, 148},
    {"libhdmpvecore.so", 0x658b8, b_hdmpvecore_129, 4, 149},
    {"libhdmpvecore.so", 0x94198, b_hdmpvecore_130, 4, 150},
    {"libhdmpvecore.so", 0x73848, b_hdmpvecore_131, 4, 151},
    {"libhdmpvecore.so", 0x9a5fc, b_hdmpvecore_132, 4, 152},
    {"libhdmpvecore.so", 0x7d42c, b_hdmpvecore_133, 4, 153},
    {"libhdmpvecore.so", 0x7f910, b_hdmpvecore_134, 4, 154},
    {"libhdmpvecore.so", 0x6fdf4, b_hdmpvecore_135, 4, 155},
    {"libhdmpvecore.so", 0x5527c, b_hdmpvecore_136, 4, 156},
    {"libhdmpvecore.so", 0x84788, b_hdmpvecore_137, 4, 157},
    {"libhdmpvecore.so", 0x9a614, b_hdmpvecore_138, 4, 158},
    {"libhdmpvecore.so", 0x915e4, b_hdmpvecore_139, 4, 159},
    {"libhdmpvecore.so", 0x6d854, b_hdmpvecore_140, 4, 160},
    {"libhdmpvecore.so", 0x616c0, b_hdmpvecore_141, 4, 161},
    {"libhdmpvecore.so", 0x7568c, b_hdmpvecore_142, 4, 162},
    {"libhdmpvecore.so", 0x62a40, b_hdmpvecore_143, 4, 163},
    {"libhdmpvecore.so", 0x94300, b_hdmpvecore_144, 4, 164},
    {"libhdmpvecore.so", 0x78628, b_hdmpvecore_145, 4, 165},
    {"libhdmpvecore.so", 0x6f660, b_hdmpvecore_146, 4, 166},
    {"libhdmpvecore.so", 0x94d24, b_hdmpvecore_147, 4, 167},
    {"libhdmpvecore.so", 0x6de5c, b_hdmpvecore_148, 4, 168},
    {"libhdmpvecore.so", 0x75904, b_hdmpvecore_149, 4, 169},
    {"libhdmpvecore.so", 0x7580c, b_hdmpvecore_150, 4, 170},
    {"libhdmpvecore.so", 0x5ad50, b_hdmpvecore_151, 4, 171},
    {"libhdmpvecore.so", 0x6d8f8, b_hdmpvecore_152, 4, 172},
    {"libhdmpvecore.so", 0x7ab1c, b_hdmpvecore_153, 4, 173},
    {"libhdmpvecore.so", 0x9a5f8, b_hdmpvecore_154, 4, 174},
    {"libhdmpvecore.so", 0x7c250, b_hdmpvecore_155, 4, 175},
    {"libhdmpvecore.so", 0x6a8bc, b_hdmpvecore_156, 4, 176},
    {"libhdmpvecore.so", 0x6df44, b_hdmpvecore_157, 4, 177},
    {"libhdmpvecore.so", 0x6e53c, b_hdmpvecore_158, 4, 178},
    {"libhdmpvecore.so", 0x74f98, b_hdmpvecore_159, 4, 179},
    {"libhdmpvecore.so", 0x6e544, b_hdmpvecore_160, 4, 180},
    {"libhdmpvecore.so", 0x66984, b_hdmpvecore_161, 4, 181},
    {"libhdmpvecore.so", 0x7c8bc, b_hdmpvecore_162, 4, 182},
    {"libhdmpvecore.so", 0x73a94, b_hdmpvecore_163, 4, 183},
    {"libhdmpvecore.so", 0x98a30, b_hdmpvecore_164, 4, 184},
    {"libhdmpvecore.so", 0x94190, b_hdmpvecore_165, 4, 185},
    {"libhdmpvecore.so", 0x6f0b4, b_hdmpvecore_166, 4, 186},
    {"libhdmpvecore.so", 0x6d8cc, b_hdmpvecore_167, 4, 187},
    {"libhdmpvecore.so", 0x7ce50, b_hdmpvecore_168, 4, 188},
    {"libhdmpvecore.so", 0x6db14, b_hdmpvecore_169, 4, 189},
    {"libhdmpvecore.so", 0x735cc, b_hdmpvecore_170, 4, 190},
    {"libhdmpvecore.so", 0x9a974, b_hdmpvecore_171, 4, 191},
    {"libhdmpvecore.so", 0x6d7d0, b_hdmpvecore_172, 4, 192},
    {"libhdmpvecore.so", 0x6252c, b_hdmpvecore_173, 4, 193},
    {"libhdmpvecore.so", 0x7a2a4, b_hdmpvecore_174, 4, 194},
    {"libhdmpvecore.so", 0x79d84, b_hdmpvecore_175, 4, 195},
    {"libhdmpvecore.so", 0x5e378, b_hdmpvecore_176, 4, 196},
    {"libhdmpvecore.so", 0x64e74, b_hdmpvecore_177, 4, 197},
    {"libhdmpvecore.so", 0x6f2ec, b_hdmpvecore_178, 4, 198},
    {"libhdmpvecore.so", 0x6d92c, b_hdmpvecore_179, 4, 199},
    {"libhdmpvecore.so", 0x6d898, b_hdmpvecore_180, 4, 200},
    {"libhdmpvecore.so", 0x7eb34, b_hdmpvecore_181, 4, 201},
    {"libhdmpvecore.so", 0x95b44, b_hdmpvecore_182, 4, 202},
    {"libhdmpvecore.so", 0x800b8, b_hdmpvecore_183, 4, 203},
    {"libhdmpvecore.so", 0x94184, b_hdmpvecore_184, 4, 204},
    {"libhdmpvecore.so", 0x98510, b_hdmpvecore_185, 4, 205},
    {"libhdmpvecore.so", 0x71f1c, b_hdmpvecore_186, 4, 206},
    {"libhdmpvecore.so", 0x7aafc, b_hdmpvecore_187, 4, 207},
    {"libhdmpvecore.so", 0x6b054, b_hdmpvecore_188, 4, 208},
    {"libhdmpvecore.so", 0x71c64, b_hdmpvecore_189, 4, 209},
    {"libhdmpvecore.so", 0x664b8, b_hdmpvecore_190, 4, 210},
    {"libhdmpvecore.so", 0x6f370, b_hdmpvecore_191, 4, 211},
    {"libhdmpvecore.so", 0x7aabc, b_hdmpvecore_192, 4, 212},
    {"libhdmpvecore.so", 0x7baac, b_hdmpvecore_193, 4, 213},
    {"libhdmpvecore.so", 0x7ec98, b_hdmpvecore_194, 4, 214},
    {"libhdmpvecore.so", 0x75950, b_hdmpvecore_195, 4, 215},
    {"libhdmpvecore.so", 0x523b4, b_hdmpvecore_196, 4, 216},
    {"libhdmpvecore.so", 0x6d84c, b_hdmpvecore_197, 4, 217},
    {"libhdmpvecore.so", 0x68260, b_hdmpvecore_198, 4, 218},
    {"libhdmpvecore.so", 0x7ce58, b_hdmpvecore_199, 4, 219},
    {"libhdmpvecore.so", 0x668b8, b_hdmpvecore_200, 4, 220},
    {"libhdmpvecore.so", 0x6dbe8, b_hdmpvecore_201, 4, 221},
    {"libhdmpvecore.so", 0x628e8, b_hdmpvecore_202, 4, 222},
    {"libhdmpvecore.so", 0x78588, b_hdmpvecore_203, 4, 223},
    {"libhdmpvecore.so", 0x7c9a8, b_hdmpvecore_204, 4, 224},
    {"libhdmpvecore.so", 0x91724, b_hdmpvecore_205, 4, 225},
    {"libhdmpvecore.so", 0x7eaf8, b_hdmpvecore_206, 4, 226},
    {"libhdmpvecore.so", 0x6f374, b_hdmpvecore_207, 4, 227},
    {"libhdmpvecore.so", 0x6d668, b_hdmpvecore_208, 4, 228},
    {"libhdmpvecore.so", 0x6e54c, b_hdmpvecore_209, 4, 229},
    {"libhdmpvecore.so", 0x6da38, b_hdmpvecore_210, 4, 230},
    {"libhdmpvecore.so", 0x9a20c, b_hdmpvecore_211, 4, 231},
    {"libhdmpvecore.so", 0x6f8f8, b_hdmpvecore_212, 4, 232},
    {"libhdmpvecore.so", 0x66984, b_hdmpvecore_213, 4, 233},
    {"libhdmpvecore.so", 0x7503c, b_hdmpvecore_214, 4, 234},
    {"libhdmpvecore.so", 0x6fd48, b_hdmpvecore_215, 4, 235},
    {"libhdmpvecore.so", 0x6de28, b_hdmpvecore_216, 4, 236},
    {"libhdmpvecore.so", 0x6fc80, b_hdmpvecore_217, 4, 237},
    {"libhdmpvecore.so", 0x70730, b_hdmpvecore_218, 4, 238},
    {"libhdmpvecore.so", 0x7eb74, b_hdmpvecore_219, 4, 239},
    {"libhdmpvecore.so", 0x7ed28, b_hdmpvecore_220, 4, 240},
    {"libhdmpvecore.so", 0x94c2c, b_hdmpvecore_221, 4, 241},
    {"libhdmpvecore.so", 0x757e8, b_hdmpvecore_222, 4, 242},
    {"libhdmpvecore.so", 0x6d71c, b_hdmpvecore_223, 4, 243},
    {"libhdmpvecore.so", 0x88370, b_hdmpvecore_224, 4, 244},
    {"libhdmpvecore.so", 0x7cd80, b_hdmpvecore_225, 4, 245},
    {"libhdmpvecore.so", 0x93330, b_hdmpvecore_226, 4, 246},
    {"libhdmpvecore.so", 0x6fa94, b_hdmpvecore_227, 4, 247},
    {"libhdmpvecore.so", 0x6272c, b_hdmpvecore_228, 4, 248},
    {"libhdmpvecore.so", 0x7ae3c, b_hdmpvecore_229, 4, 249},
    {"libhdmpvecore.so", 0x7ec50, b_hdmpvecore_230, 4, 250},
    {"libhdmpvecore.so", 0x69574, b_hdmpvecore_231, 4, 251},
    {"libhdmpvecore.so", 0x9536c, b_hdmpvecore_232, 4, 252},
    {"libhdmpvecore.so", 0x944f4, b_hdmpvecore_233, 4, 253},
    {"libhdmpvecore.so", 0x725e8, b_hdmpvecore_234, 4, 254},
    {"libhdmpvecore.so", 0x6fb38, b_hdmpvecore_235, 4, 255},
    {"libhdmpvecore.so", 0x71b2c, b_hdmpvecore_236, 4, 256},
    {"libhdmpvecore.so", 0x943cc, b_hdmpvecore_237, 4, 257},
    {"libhdmpvecore.so", 0x6d6cc, b_hdmpvecore_238, 4, 258},
    {"libhdmpvecore.so", 0x6f2f4, b_hdmpvecore_239, 4, 259},
    {"libhdmpvecore.so", 0x7aadc, b_hdmpvecore_240, 4, 260},
    {"libhdmpvecore.so", 0x6f374, b_hdmpvecore_241, 4, 261},
    {"libhdmpvecore.so", 0x6fdfc, b_hdmpvecore_242, 4, 262},
    {"libhdmpvecore.so", 0x78bc8, b_hdmpvecore_243, 4, 263},
    {"libhdmpvecore.so", 0x6d044, b_hdmpvecore_244, 4, 264},
    {"libhdmpvecore.so", 0x6d8c4, b_hdmpvecore_245, 4, 265},
    {"libhdmpvecore.so", 0x6f400, b_hdmpvecore_246, 4, 266},
    {"libhdmpvecore.so", 0x7fe04, b_hdmpvecore_247, 4, 267},
    {"libhdmpvecore.so", 0x7ece0, b_hdmpvecore_248, 4, 268},
    {"libhdmpvecore.so", 0x5a98c, b_hdmpvecore_249, 4, 269},
    {"libhdmpvecore.so", 0x6e588, b_hdmpvecore_250, 4, 270},
    {"libhdmpvecore.so", 0x6dfb4, b_hdmpvecore_251, 4, 271},
    {"libhdmpvecore.so", 0x4abcc, b_hdmpvecore_252, 4, 272},
    {"libhdmpvecore.so", 0x7447c, b_hdmpvecore_253, 4, 273},
    {"libhdmpvecore.so", 0x7536c, b_hdmpvecore_254, 4, 274},
    {"libhdmpvecore.so", 0x9a638, b_hdmpvecore_255, 4, 275},
    {"libhdmpvecore.so", 0x6e52c, b_hdmpvecore_256, 4, 276},
    {"libhdmpvecore.so", 0x7ec74, b_hdmpvecore_257, 4, 277},
    {"libhdmpvecore.so", 0x5e24c, b_hdmpvecore_258, 4, 278},
    {"libhdmpvecore.so", 0x947e8, b_hdmpvecore_259, 4, 279},
    {"libhdmpvecore.so", 0x64018, b_hdmpvecore_260, 4, 280},
    {"libhdmpvecore.so", 0x7568c, b_hdmpvecore_261, 4, 281},
    {"libhdmpvecore.so", 0x9877c, b_hdmpvecore_262, 4, 282},
    {"libhdmpvecore.so", 0x95888, b_hdmpvecore_263, 4, 283},
    {"libhdmpvecore.so", 0x62a58, b_hdmpvecore_264, 4, 284},
    {"libhdmpvecore.so", 0x7ce7c, b_hdmpvecore_265, 4, 285},
    {"libhdmpvecore.so", 0x9a974, b_hdmpvecore_266, 4, 286},
    {"libhdmpvecore.so", 0x7af78, b_hdmpvecore_267, 4, 287},
    {"libhdmpvecore.so", 0x6fc08, b_hdmpvecore_268, 4, 288},
    {"libhdmpvecore.so", 0x98510, b_hdmpvecore_269, 4, 289},
    {"libhdmpvecore.so", 0x9a608, b_hdmpvecore_270, 4, 290},
    {"libhdmpvecore.so", 0x626bc, b_hdmpvecore_271, 4, 291},
    {"libhdmpvecore.so", 0x53330, b_hdmpvecore_272, 4, 292},
    {"libhdmpvecore.so", 0x9a1b0, b_hdmpvecore_273, 4, 293},
    {"libhdmpvecore.so", 0x65678, b_hdmpvecore_274, 4, 294},
    {"libhdmpvecore.so", 0x6de38, b_hdmpvecore_275, 4, 295},
    {"libhdmpvecore.so", 0x5b82c, b_hdmpvecore_276, 4, 296},
    {"libhdmpvecore.so", 0x66564, b_hdmpvecore_277, 4, 297},
    {"libhdmpvecore.so", 0x95c1c, b_hdmpvecore_278, 4, 298},
    {"libhdmpvecore.so", 0x79bd0, b_hdmpvecore_279, 4, 299},
    {"libhdmpvecore.so", 0x97f44, b_hdmpvecore_280, 4, 300},
    {"libhdmpvecore.so", 0x984cc, b_hdmpvecore_281, 4, 301},
    {"libhdmpvecore.so", 0x71f70, b_hdmpvecore_282, 4, 302},
    {"libhdmpvecore.so", 0x9a108, b_hdmpvecore_283, 4, 303},
    {"libhdmpvecore.so", 0x76d90, b_hdmpvecore_284, 4, 304},
    {"libhdmpvecore.so", 0x7b0e4, b_hdmpvecore_285, 4, 305},
    {"libhdmpvecore.so", 0x6e520, b_hdmpvecore_286, 4, 306},
    {"libhdmpvecore.so", 0x6d890, b_hdmpvecore_287, 4, 307},
    {"libhdmpvecore.so", 0x78dcc, b_hdmpvecore_288, 4, 308},
    {"libhdmpvecore.so", 0x751e8, b_hdmpvecore_289, 4, 309},
    {"libhdmpvecore.so", 0x6f8f0, b_hdmpvecore_290, 4, 310},
    {"libhdmpvecore.so", 0x7abe0, b_hdmpvecore_291, 4, 311},
    {"libhdmpvecore.so", 0x6dc54, b_hdmpvecore_292, 4, 312},
    {"libhdmpvecore.so", 0x66468, b_hdmpvecore_293, 4, 313},
    {"libhdmpvecore.so", 0x96f98, b_hdmpvecore_294, 4, 314},
    {"libhdmpvecore.so", 0x65610, b_hdmpvecore_295, 4, 315},
    {"libhdmpvecore.so", 0x6fd0c, b_hdmpvecore_296, 4, 316},
    {"libhdmpvecore.so", 0x78704, b_hdmpvecore_297, 4, 317},
    {"libhdmpvecore.so", 0x6f3b8, b_hdmpvecore_298, 4, 318},
    {"libhdmpvecore.so", 0x6dbf0, b_hdmpvecore_299, 4, 319},
    {"libhdmpvecore.so", 0x9a5f8, b_hdmpvecore_300, 4, 320},
    {"libhdmpvecore.so", 0x92730, b_hdmpvecore_301, 4, 321},
    {"libhdmpvecore.so", 0x64f20, b_hdmpvecore_302, 4, 322},
    {"libhdmpvecore.so", 0x6f0a0, b_hdmpvecore_303, 4, 323},
    {"libhdmpvecore.so", 0x97844, b_hdmpvecore_304, 4, 324},
    {"libhdmpvecore.so", 0x9877c, b_hdmpvecore_305, 4, 325},
    {"libhdmpvecore.so", 0x9266c, b_hdmpvecore_306, 4, 326},
    {"libhdmpvecore.so", 0x7b8a0, b_hdmpvecore_307, 4, 327},
    {"libhdmpvecore.so", 0x99c08, b_hdmpvecore_308, 4, 328},
    {"libhdmpvecore.so", 0x7b7b8, b_hdmpvecore_309, 4, 329},
    {"libhdmpvecore.so", 0x62fb4, b_hdmpvecore_310, 4, 330},
    {"libhdmpvecore.so", 0x9a864, b_hdmpvecore_311, 4, 331},
    {"libhdmpvecore.so", 0x9a154, b_hdmpvecore_312, 4, 332},
    {"libhdmpvecore.so", 0x650d8, b_hdmpvecore_313, 4, 333},
    {"libhdmpvecore.so", 0x7b97c, b_hdmpvecore_314, 4, 334},
    {"libhdmpvecore.so", 0x5b600, b_hdmpvecore_315, 4, 335},
    {"libhdmpvecore.so", 0x654a8, b_hdmpvecore_316, 4, 336},
    {"libhdmpvecore.so", 0x63c9c, b_hdmpvecore_317, 4, 337},
    {"libhdmpvecore.so", 0x653b0, b_hdmpvecore_318, 4, 338},
    {"libhdmpvecore.so", 0x63fd8, b_hdmpvecore_319, 4, 339},
    {"libhdmpvecore.so", 0x97834, b_hdmpvecore_320, 4, 340},
    {"libhdmpvecore.so", 0x615b8, b_hdmpvecore_321, 4, 341},
    {"libhdmpvecore.so", 0x626b4, b_hdmpvecore_322, 4, 342},
    {"libhdmpvecore.so", 0x6d764, b_hdmpvecore_323, 4, 343},
    {"libhdmpvecore.so", 0x74edc, b_hdmpvecore_324, 4, 344},
    {"libhdmpvecore.so", 0x97300, b_hdmpvecore_325, 4, 345},
    {"libhdmpvecore.so", 0x6fea0, b_hdmpvecore_326, 4, 346},
    {"libhdmpvecore.so", 0x6285c, b_hdmpvecore_327, 4, 347},
    {"libhdmpvecore.so", 0x98bfc, b_hdmpvecore_328, 4, 348},
    {"libhdmpvecore.so", 0x948e8, b_hdmpvecore_329, 4, 349},
    {"libhdmpvecore.so", 0x6f2fc, b_hdmpvecore_330, 4, 350},
    {"libhdmpvecore.so", 0x984cc, b_hdmpvecore_331, 4, 351},
    {"libhdmpvecore.so", 0x97a14, b_hdmpvecore_332, 4, 352},
    {"libhdmpvecore.so", 0x65d30, b_hdmpvecore_333, 4, 353},
    {"libhdmpvecore.so", 0x6fd58, b_hdmpvecore_334, 4, 354},
    {"libhdmpvecore.so", 0x656bc, b_hdmpvecore_335, 4, 355},
    {"libhdmpvecore.so", 0x9536c, b_hdmpvecore_336, 4, 356},
    {"libhdmpvecore.so", 0x7d47c, b_hdmpvecore_337, 4, 357},
    {"libhdmpvecore.so", 0x9a940, b_hdmpvecore_338, 4, 358},
    {"libhdmpvecore.so", 0x6ff4c, b_hdmpvecore_339, 4, 359},
    {"libhdmpvecore.so", 0x79a5c, b_hdmpvecore_340, 4, 360},
    {"libhdmpvecore.so", 0x6f3dc, b_hdmpvecore_341, 4, 361},
    {"libhdmpvecore.so", 0x6f370, b_hdmpvecore_342, 4, 362},
    {"libhdmpvecore.so", 0x6b690, b_hdmpvecore_343, 4, 363},
    {"libhdmpvecore.so", 0x6f30c, b_hdmpvecore_344, 4, 364},
    {"libhdmpvecore.so", 0x6da9c, b_hdmpvecore_345, 4, 365},
    {"libhdmpvecore.so", 0x6f084, b_hdmpvecore_346, 4, 366},
    {"libhdmpvecore.so", 0x9a114, b_hdmpvecore_347, 4, 367},
    {"libhdmpvecore.so", 0x63ebc, b_hdmpvecore_348, 4, 368},
    {"libhdmpvecore.so", 0x76988, b_hdmpvecore_349, 4, 369},
    {"libhdmpvecore.so", 0x767e0, b_hdmpvecore_350, 4, 370},
    {"libhdmpvecore.so", 0x5343c, b_hdmpvecore_351, 4, 371},
    {"libhdmpvecore.so", 0x98758, b_hdmpvecore_352, 4, 372},
    {"libhdmpvecore.so", 0x6f934, b_hdmpvecore_353, 4, 373},
    {"libhdmpvecore.so", 0x65938, b_hdmpvecore_354, 4, 374},
    {"libhdmpvecore.so", 0x5b0f4, b_hdmpvecore_355, 4, 375},
    {"libhdmpvecore.so", 0x62534, b_hdmpvecore_356, 4, 376},
    {"libhdmpvecore.so", 0x711c4, b_hdmpvecore_357, 4, 377},
    {"libhdmpvecore.so", 0x7ec2c, b_hdmpvecore_358, 4, 378},
    {"libhdmpvecore.so", 0x94bb4, b_hdmpvecore_359, 4, 379},
    {"libhdmpvecore.so", 0x6cc88, b_hdmpvecore_360, 4, 380},
    {"libhdmpvecore.so", 0x737a0, b_hdmpvecore_361, 4, 381},
    {"libhdmpvecore.so", 0x66468, b_hdmpvecore_362, 4, 382},
    {"libhdmpvecore.so", 0x6d7a8, b_hdmpvecore_363, 4, 383},
    {"libhdmpvecore.so", 0x8b6c0, b_hdmpvecore_364, 4, 384},
    {"libhdmpvecore.so", 0x785e4, b_hdmpvecore_365, 4, 385},
    {"libhdmpvecore.so", 0x76d14, b_hdmpvecore_366, 4, 386},
    {"libhdmpvecore.so", 0x6de1c, b_hdmpvecore_367, 4, 387},
    {"libhdmpvecore.so", 0x6f400, b_hdmpvecore_368, 4, 388},
    {"libhdmpvecore.so", 0x6e570, b_hdmpvecore_369, 4, 389},
    {"libhdmpvecore.so", 0x7ac4c, b_hdmpvecore_370, 4, 390},
    {"libhdmpvecore.so", 0x6de6c, b_hdmpvecore_371, 4, 391},
    {"libhdmpvecore.so", 0x74310, b_hdmpvecore_372, 4, 392},
    {"libhdmpvecore.so", 0x9a20c, b_hdmpvecore_373, 4, 393},
    {"libhdmpvecore.so", 0x9735c, b_hdmpvecore_374, 4, 394},
    {"libhdmpvecore.so", 0x66210, b_hdmpvecore_375, 4, 395},
    {"libhdmpvecore.so", 0x7eaec, b_hdmpvecore_376, 4, 396},
    {"libhdmpvecore.so", 0x7ccd8, b_hdmpvecore_377, 4, 397},
    {"libhdmpvecore.so", 0x73150, b_hdmpvecore_378, 8, 398},
    {"libhdmpvecore.so", 0x72cac, b_hdmpvecore_379, 8, 399},
    {"libhdmpvecore.so", 0x7314c, b_hdmpvecore_380, 8, 400},
};

// libUE4.so: 344 patches
static const uint8_t b_UE4_0[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_1[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_2[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_3[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_4[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_5[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_6[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_7[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_8[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_9[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_10[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_11[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_12[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_13[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_14[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_15[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_16[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_17[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_18[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_19[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_20[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_21[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_22[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_23[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_24[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_25[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_26[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_27[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_28[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_29[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_30[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_31[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_32[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_33[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_34[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_35[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_36[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_37[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_38[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_39[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_40[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_41[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_42[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_43[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_44[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_45[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_46[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_47[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_48[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_49[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_50[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_51[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_52[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_53[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_54[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_55[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_56[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_57[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_58[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_59[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_60[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_61[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_62[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_63[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_64[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_65[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_66[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_67[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_68[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_69[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_70[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_71[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_72[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_73[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_74[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_75[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_76[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_77[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_78[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_79[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_80[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_81[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_82[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_83[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_84[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_85[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_86[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_87[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_88[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_89[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_90[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_91[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_92[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_93[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_94[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_95[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_96[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_97[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_98[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_99[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_100[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_101[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_102[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_103[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_104[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_105[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_106[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_107[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_108[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_109[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_110[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_111[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_112[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_113[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_114[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_115[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_116[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_117[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_118[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_119[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_120[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_121[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_122[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_123[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_124[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_125[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_126[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_127[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_128[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_129[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_130[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_131[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_132[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_133[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_134[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_135[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_136[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_137[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_138[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_139[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_140[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_141[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_142[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_143[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_144[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_145[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_146[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_147[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_148[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_149[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_150[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_151[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_152[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_153[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_154[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_155[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_156[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_157[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_158[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_159[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_160[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_161[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_162[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_163[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_164[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_165[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_166[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_167[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_168[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_169[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_170[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_171[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_172[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_173[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_174[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_175[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_176[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_177[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_178[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_179[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_180[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_181[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_182[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_183[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_184[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_185[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_186[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_187[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_188[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_189[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_190[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_191[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_192[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_193[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_194[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_195[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_196[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_197[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_198[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_199[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_200[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_201[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_202[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_203[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_204[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_205[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_206[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_207[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_208[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_209[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_210[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_211[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_212[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_213[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_214[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_215[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_216[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_217[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_218[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_219[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_220[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_221[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_222[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_223[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_224[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_225[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_226[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_227[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_228[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_229[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_230[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_231[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_232[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_233[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_234[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_235[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_236[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_237[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_238[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_239[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_240[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_241[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_242[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_243[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_244[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_245[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_246[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_247[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_248[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_249[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_250[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_251[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_252[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_253[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_254[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_255[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_256[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_257[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_258[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_259[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_260[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_261[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_262[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_263[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_264[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_265[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_266[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_267[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_268[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_269[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_270[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_271[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_272[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_273[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_274[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_275[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_276[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_277[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_278[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_279[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_280[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_281[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_282[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_283[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_284[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_285[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_286[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_287[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_288[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_289[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_290[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_291[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_292[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_293[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_294[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_295[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_296[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_297[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_298[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_299[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_300[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_301[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_302[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_303[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_304[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_305[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_306[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_307[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_308[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_309[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_310[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_311[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_312[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_313[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_314[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_315[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_316[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_317[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_318[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_319[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_320[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_321[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_322[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_323[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_324[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_325[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_326[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_327[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_328[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_329[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_330[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_331[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_332[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_333[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_334[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_335[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_336[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_337[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_338[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_339[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_340[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_341[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_342[] = {0x1F, 0x20, 0x03, 0xD5};
static const uint8_t b_UE4_343[] = {0x1F, 0x20, 0x03, 0xD5};

static stealth::Patch patches_UE4[] = {
    {"libUE4.so", 0x64a3d28, b_UE4_0, 4, 401},
    {"libUE4.so", 0x649f060, b_UE4_1, 4, 402},
    {"libUE4.so", 0x649ed48, b_UE4_2, 4, 403},
    {"libUE4.so", 0x62592e4, b_UE4_3, 4, 404},
    {"libUE4.so", 0x713a794, b_UE4_4, 4, 405},
    {"libUE4.so", 0x6e98710, b_UE4_5, 4, 406},
    {"libUE4.so", 0x6f31de4, b_UE4_6, 4, 407},
    {"libUE4.so", 0x6ac3eec, b_UE4_7, 4, 408},
    {"libUE4.so", 0x78e0020, b_UE4_8, 4, 409},
    {"libUE4.so", 0x6b7bec4, b_UE4_9, 4, 410},
    {"libUE4.so", 0x6b73b08, b_UE4_10, 4, 411},
    {"libUE4.so", 0x6b73a48, b_UE4_11, 4, 412},
    {"libUE4.so", 0x6aadbdc, b_UE4_12, 4, 413},
    {"libUE4.so", 0x6ac3d58, b_UE4_13, 4, 414},
    {"libUE4.so", 0x649f410, b_UE4_14, 4, 415},
    {"libUE4.so", 0x69c1280, b_UE4_15, 4, 416},
    {"libUE4.so", 0x69c1378, b_UE4_16, 4, 417},
    {"libUE4.so", 0x69c1444, b_UE4_17, 4, 418},
    {"libUE4.so", 0x69c13fc, b_UE4_18, 4, 419},
    {"libUE4.so", 0x85818bc, b_UE4_19, 4, 420},
    {"libUE4.so", 0xaf8ad9c, b_UE4_20, 4, 421},
    {"libUE4.so", 0x7f51f7c, b_UE4_21, 4, 422},
    {"libUE4.so", 0x6cdbaa8, b_UE4_22, 4, 423},
    {"libUE4.so", 0x6cdbbac, b_UE4_23, 4, 424},
    {"libUE4.so", 0x650e6d0, b_UE4_24, 4, 425},
    {"libUE4.so", 0x69da274, b_UE4_25, 4, 426},
    {"libUE4.so", 0x69d9e20, b_UE4_26, 4, 427},
    {"libUE4.so", 0x69d9ff8, b_UE4_27, 4, 428},
    {"libUE4.so", 0x69da0ac, b_UE4_28, 4, 429},
    {"libUE4.so", 0x69da1b4, b_UE4_29, 4, 430},
    {"libUE4.so", 0x69d9c24, b_UE4_30, 4, 431},
    {"libUE4.so", 0x7e504e0, b_UE4_31, 4, 432},
    {"libUE4.so", 0x7e4d9e8, b_UE4_32, 4, 433},
    {"libUE4.so", 0x7e4fdb8, b_UE4_33, 4, 434},
    {"libUE4.so", 0x7e4e854, b_UE4_34, 4, 435},
    {"libUE4.so", 0x7e4f230, b_UE4_35, 4, 436},
    {"libUE4.so", 0x815a930, b_UE4_36, 4, 437},
    {"libUE4.so", 0x7e4f1ec, b_UE4_37, 4, 438},
    {"libUE4.so", 0x7e4d514, b_UE4_38, 4, 439},
    {"libUE4.so", 0x7e48684, b_UE4_39, 4, 440},
    {"libUE4.so", 0x7e4e1f8, b_UE4_40, 4, 441},
    {"libUE4.so", 0x7e4a0f0, b_UE4_41, 4, 442},
    {"libUE4.so", 0x7e4d558, b_UE4_42, 4, 443},
    {"libUE4.so", 0x7e4e244, b_UE4_43, 4, 444},
    {"libUE4.so", 0x7e4e8a0, b_UE4_44, 4, 445},
    {"libUE4.so", 0x7e48638, b_UE4_45, 4, 446},
    {"libUE4.so", 0x7e4fd74, b_UE4_46, 4, 447},
    {"libUE4.so", 0x7e50494, b_UE4_47, 4, 448},
    {"libUE4.so", 0x7e493c8, b_UE4_48, 4, 449},
    {"libUE4.so", 0x7e505a8, b_UE4_49, 4, 450},
    {"libUE4.so", 0x7e50694, b_UE4_50, 4, 451},
    {"libUE4.so", 0x7e505f4, b_UE4_51, 4, 452},
    {"libUE4.so", 0x7e504d8, b_UE4_52, 4, 453},
    {"libUE4.so", 0x7e50490, b_UE4_53, 4, 454},
    {"libUE4.so", 0x7e4a0e8, b_UE4_54, 4, 455},
    {"libUE4.so", 0x7e49f4c, b_UE4_55, 4, 456},
    {"libUE4.so", 0x7e4937c, b_UE4_56, 4, 457},
    {"libUE4.so", 0x7e493c4, b_UE4_57, 4, 458},
    {"libUE4.so", 0x7e4867c, b_UE4_58, 4, 459},
    {"libUE4.so", 0x7e48634, b_UE4_59, 4, 460},
    {"libUE4.so", 0x7e4fd6c, b_UE4_60, 4, 461},
    {"libUE4.so", 0x7e4fdb4, b_UE4_61, 4, 462},
    {"libUE4.so", 0x7e4f1e4, b_UE4_62, 4, 463},
    {"libUE4.so", 0x7e4f22c, b_UE4_63, 4, 464},
    {"libUE4.so", 0x7e4e898, b_UE4_64, 4, 465},
    {"libUE4.so", 0x7e4e850, b_UE4_65, 4, 466},
    {"libUE4.so", 0x7e4e23c, b_UE4_66, 4, 467},
    {"libUE4.so", 0x7e4e1f4, b_UE4_67, 4, 468},
    {"libUE4.so", 0x7e4d9e4, b_UE4_68, 4, 469},
    {"libUE4.so", 0x7b9925c, b_UE4_69, 4, 470},
    {"libUE4.so", 0x7b99424, b_UE4_70, 4, 471},
    {"libUE4.so", 0x7b9ba8c, b_UE4_71, 4, 472},
    {"libUE4.so", 0x7bcbcd0, b_UE4_72, 4, 473},
    {"libUE4.so", 0x7bcbc54, b_UE4_73, 4, 474},
    {"libUE4.so", 0x7b9b100, b_UE4_74, 4, 475},
    {"libUE4.so", 0x5b2f240, b_UE4_75, 4, 476},
    {"libUE4.so", 0x7881184, b_UE4_76, 4, 477},
    {"libUE4.so", 0x5abf744, b_UE4_77, 4, 478},
    {"libUE4.so", 0x5abfd80, b_UE4_78, 4, 479},
    {"libUE4.so", 0x5abd2e8, b_UE4_79, 4, 480},
    {"libUE4.so", 0x5abde18, b_UE4_80, 4, 481},
    {"libUE4.so", 0x5abe400, b_UE4_81, 4, 482},
    {"libUE4.so", 0x5c99574, b_UE4_82, 4, 483},
    {"libUE4.so", 0x5abf110, b_UE4_83, 4, 484},
    {"libUE4.so", 0x5abe498, b_UE4_84, 4, 485},
    {"libUE4.so", 0x5abf4f4, b_UE4_85, 4, 486},
    {"libUE4.so", 0x5abe7d8, b_UE4_86, 4, 487},
    {"libUE4.so", 0x5abd43c, b_UE4_87, 4, 488},
    {"libUE4.so", 0x5abeccc, b_UE4_88, 4, 489},
    {"libUE4.so", 0x5abed70, b_UE4_89, 4, 490},
    {"libUE4.so", 0x5aecab0, b_UE4_90, 4, 491},
    {"libUE4.so", 0x5abe868, b_UE4_91, 4, 492},
    {"libUE4.so", 0x5aecab8, b_UE4_92, 4, 493},
    {"libUE4.so", 0x5abd774, b_UE4_93, 4, 494},
    {"libUE4.so", 0x650e75c, b_UE4_94, 4, 495},
    {"libUE4.so", 0x5a9942c, b_UE4_95, 4, 496},
    {"libUE4.so", 0x71fd684, b_UE4_96, 4, 497},
    {"libUE4.so", 0x71fd27c, b_UE4_97, 4, 498},
    {"libUE4.so", 0x71fd1b8, b_UE4_98, 4, 499},
    {"libUE4.so", 0x6810b54, b_UE4_99, 4, 500},
    {"libUE4.so", 0x5a98324, b_UE4_100, 4, 501},
    {"libUE4.so", 0x5a994b0, b_UE4_101, 4, 502},
    {"libUE4.so", 0x5abd6ec, b_UE4_102, 4, 503},
    {"libUE4.so", 0x5a9de04, b_UE4_103, 4, 504},
    {"libUE4.so", 0x5abe368, b_UE4_104, 4, 505},
    {"libUE4.so", 0x5c9b318, b_UE4_105, 4, 506},
    {"libUE4.so", 0x5a98258, b_UE4_106, 4, 507},
    {"libUE4.so", 0x5abfc04, b_UE4_107, 4, 508},
    {"libUE4.so", 0x5abfa2c, b_UE4_108, 4, 509},
    {"libUE4.so", 0x5abfc00, b_UE4_109, 4, 510},
    {"libUE4.so", 0x5abfd7c, b_UE4_110, 4, 511},
    {"libUE4.so", 0x5abfcf4, b_UE4_111, 4, 512},
    {"libUE4.so", 0x5abfcb4, b_UE4_112, 4, 513},
    {"libUE4.so", 0x5abff04, b_UE4_113, 4, 514},
    {"libUE4.so", 0x5ac01a8, b_UE4_114, 4, 515},
    {"libUE4.so", 0x5ac046c, b_UE4_115, 4, 516},
    {"libUE4.so", 0x5ac0780, b_UE4_116, 4, 517},
    {"libUE4.so", 0x5ac0928, b_UE4_117, 4, 518},
    {"libUE4.so", 0x5abbb10, b_UE4_118, 4, 519},
    {"libUE4.so", 0x5abc284, b_UE4_119, 4, 520},
    {"libUE4.so", 0x5abc158, b_UE4_120, 4, 521},
    {"libUE4.so", 0x5abbd38, b_UE4_121, 4, 522},
    {"libUE4.so", 0x5abbd84, b_UE4_122, 4, 523},
    {"libUE4.so", 0x5abb6b8, b_UE4_123, 4, 524},
    {"libUE4.so", 0x5abb5f0, b_UE4_124, 4, 525},
    {"libUE4.so", 0x5abb4f8, b_UE4_125, 4, 526},
    {"libUE4.so", 0x5abb184, b_UE4_126, 4, 527},
    {"libUE4.so", 0x5aba450, b_UE4_127, 4, 528},
    {"libUE4.so", 0x5aba390, b_UE4_128, 4, 529},
    {"libUE4.so", 0x5abf388, b_UE4_129, 4, 530},
    {"libUE4.so", 0x5abf4f0, b_UE4_130, 4, 531},
    {"libUE4.so", 0x5abed6c, b_UE4_131, 4, 532},
    {"libUE4.so", 0x5abe864, b_UE4_132, 4, 533},
    {"libUE4.so", 0x5abe1c8, b_UE4_133, 4, 534},
    {"libUE4.so", 0x5abe364, b_UE4_134, 4, 535},
    {"libUE4.so", 0x5abe3fc, b_UE4_135, 4, 536},
    {"libUE4.so", 0x5abe494, b_UE4_136, 4, 537},
    {"libUE4.so", 0x5abe53c, b_UE4_137, 4, 538},
    {"libUE4.so", 0x5c9b064, b_UE4_138, 4, 539},
    {"libUE4.so", 0x5c9b314, b_UE4_139, 4, 540},
    {"libUE4.so", 0x5c99ebc, b_UE4_140, 4, 541},
    {"libUE4.so", 0x5c997fc, b_UE4_141, 4, 542},
    {"libUE4.so", 0x5c997a8, b_UE4_142, 4, 543},
    {"libUE4.so", 0x5c99570, b_UE4_143, 4, 544},
    {"libUE4.so", 0x5c9a1a8, b_UE4_144, 4, 545},
    {"libUE4.so", 0x5c9a278, b_UE4_145, 4, 546},
    {"libUE4.so", 0x5c9a2fc, b_UE4_146, 4, 547},
    {"libUE4.so", 0x5c9a37c, b_UE4_147, 4, 548},
    {"libUE4.so", 0x5c9a3e8, b_UE4_148, 4, 549},
    {"libUE4.so", 0x5c9b1a0, b_UE4_149, 4, 550},
    {"libUE4.so", 0x5c9a1f0, b_UE4_150, 4, 551},
    {"libUE4.so", 0x6e2b548, b_UE4_151, 4, 552},
    {"libUE4.so", 0x6e230b0, b_UE4_152, 4, 553},
    {"libUE4.so", 0x6e22fec, b_UE4_153, 4, 554},
    {"libUE4.so", 0x5ac296c, b_UE4_154, 4, 555},
    {"libUE4.so", 0x5ac11d4, b_UE4_155, 4, 556},
    {"libUE4.so", 0x5ac3fd0, b_UE4_156, 4, 557},
    {"libUE4.so", 0x5ac1060, b_UE4_157, 4, 558},
    {"libUE4.so", 0x5ac2cf8, b_UE4_158, 4, 559},
    {"libUE4.so", 0x5ac2560, b_UE4_159, 4, 560},
    {"libUE4.so", 0x5ac2ec4, b_UE4_160, 4, 561},
    {"libUE4.so", 0x5abcda8, b_UE4_161, 4, 562},
    {"libUE4.so", 0x5ac0f04, b_UE4_162, 4, 563},
    {"libUE4.so", 0x5ac3f28, b_UE4_163, 4, 564},
    {"libUE4.so", 0x5ac25c4, b_UE4_164, 4, 565},
    {"libUE4.so", 0x5ac3a34, b_UE4_165, 4, 566},
    {"libUE4.so", 0x5ac3f1c, b_UE4_166, 4, 567},
    {"libUE4.so", 0x5ac3fcc, b_UE4_167, 4, 568},
    {"libUE4.so", 0x5ac3240, b_UE4_168, 4, 569},
    {"libUE4.so", 0x5ac2ec0, b_UE4_169, 4, 570},
    {"libUE4.so", 0x5ac255c, b_UE4_170, 4, 571},
    {"libUE4.so", 0x5ac25c0, b_UE4_171, 4, 572},
    {"libUE4.so", 0x5ac20b0, b_UE4_172, 4, 573},
    {"libUE4.so", 0x5ac1b9c, b_UE4_173, 4, 574},
    {"libUE4.so", 0x5ac11d0, b_UE4_174, 4, 575},
    {"libUE4.so", 0x5ac0f00, b_UE4_175, 4, 576},
    {"libUE4.so", 0x5ac1050, b_UE4_176, 4, 577},
    {"libUE4.so", 0x5abcda4, b_UE4_177, 4, 578},
    {"libUE4.so", 0x5ac2968, b_UE4_178, 4, 579},
    {"libUE4.so", 0x5ac2cf4, b_UE4_179, 4, 580},
    {"libUE4.so", 0x7883f4c, b_UE4_180, 4, 581},
    {"libUE4.so", 0x77e8478, b_UE4_181, 4, 582},
    {"libUE4.so", 0x792f14c, b_UE4_182, 4, 583},
    {"libUE4.so", 0x791f180, b_UE4_183, 4, 584},
    {"libUE4.so", 0x791f0c8, b_UE4_184, 4, 585},
    {"libUE4.so", 0x791ee00, b_UE4_185, 4, 586},
    {"libUE4.so", 0x791ed3c, b_UE4_186, 4, 587},
    {"libUE4.so", 0x77e80c8, b_UE4_187, 4, 588},
    {"libUE4.so", 0x77e8c7c, b_UE4_188, 4, 589},
    {"libUE4.so", 0x78414e0, b_UE4_189, 4, 590},
    {"libUE4.so", 0x7883fa0, b_UE4_190, 4, 591},
    {"libUE4.so", 0x77e81d4, b_UE4_191, 4, 592},
    {"libUE4.so", 0x7883fd4, b_UE4_192, 4, 593},
    {"libUE4.so", 0x77e8c78, b_UE4_193, 4, 594},
    {"libUE4.so", 0x77e838c, b_UE4_194, 4, 595},
    {"libUE4.so", 0x77e8474, b_UE4_195, 4, 596},
    {"libUE4.so", 0x77e80c4, b_UE4_196, 4, 597},
    {"libUE4.so", 0x77e81d0, b_UE4_197, 4, 598},
    {"libUE4.so", 0x77e8574, b_UE4_198, 4, 599},
    {"libUE4.so", 0x5ba3cb4, b_UE4_199, 4, 600},
    {"libUE4.so", 0x605db44, b_UE4_200, 4, 601},
    {"libUE4.so", 0x792096c, b_UE4_201, 4, 602},
    {"libUE4.so", 0x631275c, b_UE4_202, 4, 603},
    {"libUE4.so", 0x6f2878c, b_UE4_203, 4, 604},
    {"libUE4.so", 0x69cde0c, b_UE4_204, 4, 605},
    {"libUE4.so", 0x5c9a3ec, b_UE4_205, 4, 606},
    {"libUE4.so", 0x5ab8298, b_UE4_206, 4, 607},
    {"libUE4.so", 0x5ac01ac, b_UE4_207, 4, 608},
    {"libUE4.so", 0x5abb11c, b_UE4_208, 4, 609},
    {"libUE4.so", 0x5abb5f4, b_UE4_209, 4, 610},
    {"libUE4.so", 0x5a9d5e4, b_UE4_210, 4, 611},
    {"libUE4.so", 0x5aba394, b_UE4_211, 4, 612},
    {"libUE4.so", 0x6d2e3d8, b_UE4_212, 4, 613},
    {"libUE4.so", 0x5aba9ac, b_UE4_213, 4, 614},
    {"libUE4.so", 0x5c9a380, b_UE4_214, 4, 615},
    {"libUE4.so", 0x78de48c, b_UE4_215, 4, 616},
    {"libUE4.so", 0x5abb6bc, b_UE4_216, 4, 617},
    {"libUE4.so", 0x5c9a1ac, b_UE4_217, 4, 618},
    {"libUE4.so", 0x6f2825c, b_UE4_218, 4, 619},
    {"libUE4.so", 0x5ac0784, b_UE4_219, 4, 620},
    {"libUE4.so", 0x5a9c868, b_UE4_220, 4, 621},
    {"libUE4.so", 0x5a9d32c, b_UE4_221, 4, 622},
    {"libUE4.so", 0x78de564, b_UE4_222, 4, 623},
    {"libUE4.so", 0x7eee450, b_UE4_223, 4, 624},
    {"libUE4.so", 0x5abb188, b_UE4_224, 4, 625},
    {"libUE4.so", 0x5abb1fc, b_UE4_225, 4, 626},
    {"libUE4.so", 0x5aba454, b_UE4_226, 4, 627},
    {"libUE4.so", 0x5c9b068, b_UE4_227, 4, 628},
    {"libUE4.so", 0x5a9c5d8, b_UE4_228, 4, 629},
    {"libUE4.so", 0x5a9c76c, b_UE4_229, 4, 630},
    {"libUE4.so", 0x5abfa30, b_UE4_230, 4, 631},
    {"libUE4.so", 0x5c9a27c, b_UE4_231, 4, 632},
    {"libUE4.so", 0x5ac0470, b_UE4_232, 4, 633},
    {"libUE4.so", 0x5abbd88, b_UE4_233, 4, 634},
    {"libUE4.so", 0x5abc288, b_UE4_234, 4, 635},
    {"libUE4.so", 0x5abc160, b_UE4_235, 4, 636},
    {"libUE4.so", 0x5abc23c, b_UE4_236, 4, 637},
    {"libUE4.so", 0x5abbcd0, b_UE4_237, 4, 638},
    {"libUE4.so", 0x5abbeb4, b_UE4_238, 4, 639},
    {"libUE4.so", 0x5abbd40, b_UE4_239, 4, 640},
    {"libUE4.so", 0x6aaeb3c, b_UE4_240, 4, 641},
    {"libUE4.so", 0x6aae758, b_UE4_241, 4, 642},
    {"libUE4.so", 0x6aaed6c, b_UE4_242, 4, 643},
    {"libUE4.so", 0x69c144c, b_UE4_243, 4, 644},
    {"libUE4.so", 0x5ac092c, b_UE4_244, 4, 645},
    {"libUE4.so", 0x650ebd0, b_UE4_245, 4, 646},
    {"libUE4.so", 0x650ed54, b_UE4_246, 4, 647},
    {"libUE4.so", 0x5a9c484, b_UE4_247, 4, 648},
    {"libUE4.so", 0x5ab8168, b_UE4_248, 4, 649},
    {"libUE4.so", 0x650e9d8, b_UE4_249, 4, 650},
    {"libUE4.so", 0x6aae904, b_UE4_250, 4, 651},
    {"libUE4.so", 0x650ec48, b_UE4_251, 4, 652},
    {"libUE4.so", 0x6ac3a0c, b_UE4_252, 4, 653},
    {"libUE4.so", 0x6ac381c, b_UE4_253, 4, 654},
    {"libUE4.so", 0x69c137c, b_UE4_254, 4, 655},
    {"libUE4.so", 0x6aae36c, b_UE4_255, 4, 656},
    {"libUE4.so", 0x6aae414, b_UE4_256, 4, 657},
    {"libUE4.so", 0x6aae458, b_UE4_257, 4, 658},
    {"libUE4.so", 0x6ac37cc, b_UE4_258, 4, 659},
    {"libUE4.so", 0x6ac3b84, b_UE4_259, 4, 660},
    {"libUE4.so", 0x6ac3a08, b_UE4_260, 4, 661},
    {"libUE4.so", 0x6ac37c8, b_UE4_261, 4, 662},
    {"libUE4.so", 0x6ac3818, b_UE4_262, 4, 663},
    {"libUE4.so", 0x6aae368, b_UE4_263, 4, 664},
    {"libUE4.so", 0x6aae410, b_UE4_264, 4, 665},
    {"libUE4.so", 0x6aae454, b_UE4_265, 4, 666},
    {"libUE4.so", 0x6ac3d54, b_UE4_266, 4, 667},
    {"libUE4.so", 0x6ac3e68, b_UE4_267, 4, 668},
    {"libUE4.so", 0x6ac3ee8, b_UE4_268, 4, 669},
    {"libUE4.so", 0x69abe7c, b_UE4_269, 4, 670},
    {"libUE4.so", 0x605de9c, b_UE4_270, 4, 671},
    {"libUE4.so", 0x650c9a8, b_UE4_271, 4, 672},
    {"libUE4.so", 0x650c9f0, b_UE4_272, 4, 673},
    {"libUE4.so", 0x650df28, b_UE4_273, 4, 674},
    {"libUE4.so", 0x650dc5c, b_UE4_274, 4, 675},
    {"libUE4.so", 0x650cf8c, b_UE4_275, 4, 676},
    {"libUE4.so", 0x650cb18, b_UE4_276, 4, 677},
    {"libUE4.so", 0x650c93c, b_UE4_277, 4, 678},
    {"libUE4.so", 0x650eb18, b_UE4_278, 4, 679},
    {"libUE4.so", 0x650ecd4, b_UE4_279, 4, 680},
    {"libUE4.so", 0x6510d50, b_UE4_280, 4, 681},
    {"libUE4.so", 0x6512d7c, b_UE4_281, 4, 682},
    {"libUE4.so", 0x65112d4, b_UE4_282, 4, 683},
    {"libUE4.so", 0x65100c4, b_UE4_283, 4, 684},
    {"libUE4.so", 0x6511f0c, b_UE4_284, 4, 685},
    {"libUE4.so", 0x65133a8, b_UE4_285, 4, 686},
    {"libUE4.so", 0x65138f8, b_UE4_286, 4, 687},
    {"libUE4.so", 0x650ef80, b_UE4_287, 4, 688},
    {"libUE4.so", 0x6511180, b_UE4_288, 4, 689},
    {"libUE4.so", 0x6512fe4, b_UE4_289, 4, 690},
    {"libUE4.so", 0x650ffbc, b_UE4_290, 4, 691},
    {"libUE4.so", 0x6510148, b_UE4_291, 4, 692},
    {"libUE4.so", 0x651077c, b_UE4_292, 4, 693},
    {"libUE4.so", 0x6510de4, b_UE4_293, 4, 694},
    {"libUE4.so", 0x650fba8, b_UE4_294, 4, 695},
    {"libUE4.so", 0x6513068, b_UE4_295, 4, 696},
    {"libUE4.so", 0x6510c68, b_UE4_296, 4, 697},
    {"libUE4.so", 0x6512554, b_UE4_297, 4, 698},
    {"libUE4.so", 0x6513780, b_UE4_298, 4, 699},
    {"libUE4.so", 0x6510318, b_UE4_299, 4, 700},
    {"libUE4.so", 0x6510510, b_UE4_300, 4, 701},
    {"libUE4.so", 0x6512f9c, b_UE4_301, 4, 702},
    {"libUE4.so", 0x65101e8, b_UE4_302, 4, 703},
    {"libUE4.so", 0x5abee94, b_UE4_303, 4, 704},
    {"libUE4.so", 0x69c151c, b_UE4_304, 4, 705},
    {"libUE4.so", 0x6f02494, b_UE4_305, 4, 706},
    {"libUE4.so", 0x6f28550, b_UE4_306, 4, 707},
    {"libUE4.so", 0x5f3b658, b_UE4_307, 4, 708},
    {"libUE4.so", 0x74dcb70, b_UE4_308, 4, 709},
    {"libUE4.so", 0x74dc28c, b_UE4_309, 4, 710},
    {"libUE4.so", 0x5f3cfdc, b_UE4_310, 4, 711},
    {"libUE4.so", 0x5f3cbec, b_UE4_311, 4, 712},
    {"libUE4.so", 0x5f3caf0, b_UE4_312, 4, 713},
    {"libUE4.so", 0x6c93308, b_UE4_313, 4, 714},
    {"libUE4.so", 0x6c933c8, b_UE4_314, 4, 715},
    {"libUE4.so", 0x74dc82c, b_UE4_315, 4, 716},
    {"libUE4.so", 0x5f3ceb8, b_UE4_316, 4, 717},
    {"libUE4.so", 0x62fa224, b_UE4_317, 4, 718},
    {"libUE4.so", 0x5f3cef4, b_UE4_318, 4, 719},
    {"libUE4.so", 0x685f078, b_UE4_319, 4, 720},
    {"libUE4.so", 0x7b9a8a4, b_UE4_320, 4, 721},
    {"libUE4.so", 0x7bcbccc, b_UE4_321, 4, 722},
    {"libUE4.so", 0x7bcbc50, b_UE4_322, 4, 723},
    {"libUE4.so", 0x7b9ba80, b_UE4_323, 4, 724},
    {"libUE4.so", 0x7b99250, b_UE4_324, 4, 725},
    {"libUE4.so", 0x7b99420, b_UE4_325, 4, 726},
    {"libUE4.so", 0x7b9b0b0, b_UE4_326, 4, 727},
    {"libUE4.so", 0x7b9b0fc, b_UE4_327, 4, 728},
    {"libUE4.so", 0xafaace8, b_UE4_328, 4, 729},
    {"libUE4.so", 0xaf43804, b_UE4_329, 4, 730},
    {"libUE4.so", 0x6e22d58, b_UE4_330, 4, 731},
    {"libUE4.so", 0xaf72070, b_UE4_331, 4, 732},
    {"libUE4.so", 0xaf720e0, b_UE4_332, 4, 733},
    {"libUE4.so", 0xaf72000, b_UE4_333, 4, 734},
    {"libUE4.so", 0xaf43800, b_UE4_334, 4, 735},
    {"libUE4.so", 0x5f3b658, b_UE4_335, 4, 736},
    {"libUE4.so", 0x5f3cfdc, b_UE4_336, 4, 737},
    {"libUE4.so", 0xaf72070, b_UE4_337, 4, 738},
    {"libUE4.so", 0xaf720e0, b_UE4_338, 4, 739},
    {"libUE4.so", 0x5f3cbec, b_UE4_339, 4, 740},
    {"libUE4.so", 0x5f3caf0, b_UE4_340, 4, 741},
    {"libUE4.so", 0x5f3ceb8, b_UE4_341, 4, 742},
    {"libUE4.so", 0x5f3cef4, b_UE4_342, 4, 743},
    {"libUE4.so", 0xaf72000, b_UE4_343, 4, 744},
};

// libTBlueData.so: 36 patches
static const uint8_t b_TBlueData_0[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_1[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_2[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_3[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_4[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_5[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_6[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_7[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_8[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_9[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_10[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_11[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_12[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_13[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_14[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_15[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_16[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_17[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_18[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_19[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_20[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_21[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_22[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_23[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_24[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_25[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_26[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_27[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_28[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_29[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_30[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_31[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_32[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_33[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_34[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_TBlueData_35[] = {0xC0, 0x03, 0x5F, 0xD6};

static stealth::Patch patches_TBlueData[] = {
    {"libTBlueData.so", 0x8e854, b_TBlueData_0, 4, 745},
    {"libTBlueData.so", 0x8f5c4, b_TBlueData_1, 4, 746},
    {"libTBlueData.so", 0x91fb8, b_TBlueData_2, 4, 747},
    {"libTBlueData.so", 0x8e308, b_TBlueData_3, 4, 748},
    {"libTBlueData.so", 0x4d258, b_TBlueData_4, 4, 749},
    {"libTBlueData.so", 0x8e81c, b_TBlueData_5, 4, 750},
    {"libTBlueData.so", 0x8e7e4, b_TBlueData_6, 4, 751},
    {"libTBlueData.so", 0x92328, b_TBlueData_7, 4, 752},
    {"libTBlueData.so", 0x92380, b_TBlueData_8, 4, 753},
    {"libTBlueData.so", 0x91dd4, b_TBlueData_9, 4, 754},
    {"libTBlueData.so", 0x8e578, b_TBlueData_10, 4, 755},
    {"libTBlueData.so", 0x923c0, b_TBlueData_11, 4, 756},
    {"libTBlueData.so", 0x922e4, b_TBlueData_12, 4, 757},
    {"libTBlueData.so", 0x8ebf8, b_TBlueData_13, 4, 758},
    {"libTBlueData.so", 0x92370, b_TBlueData_14, 4, 759},
    {"libTBlueData.so", 0x8ff20, b_TBlueData_15, 4, 760},
    {"libTBlueData.so", 0x92344, b_TBlueData_16, 4, 761},
    {"libTBlueData.so", 0x92398, b_TBlueData_17, 4, 762},
    {"libTBlueData.so", 0x91ff0, b_TBlueData_18, 4, 763},
    {"libTBlueData.so", 0x91e74, b_TBlueData_19, 4, 764},
    {"libTBlueData.so", 0x923c4, b_TBlueData_20, 4, 765},
    {"libTBlueData.so", 0x91ec0, b_TBlueData_21, 4, 766},
    {"libTBlueData.so", 0x9238c, b_TBlueData_22, 4, 767},
    {"libTBlueData.so", 0x8ebb4, b_TBlueData_23, 4, 768},
    {"libTBlueData.so", 0x8f7ac, b_TBlueData_24, 4, 769},
    {"libTBlueData.so", 0x9232c, b_TBlueData_25, 4, 770},
    {"libTBlueData.so", 0x9236c, b_TBlueData_26, 4, 771},
    {"libTBlueData.so", 0x92368, b_TBlueData_27, 4, 772},
    {"libTBlueData.so", 0x923bc, b_TBlueData_28, 4, 773},
    {"libTBlueData.so", 0x8f914, b_TBlueData_29, 4, 774},
    {"libTBlueData.so", 0x92378, b_TBlueData_30, 4, 775},
    {"libTBlueData.so", 0x91e28, b_TBlueData_31, 4, 776},
    {"libTBlueData.so", 0x92324, b_TBlueData_32, 4, 777},
    {"libTBlueData.so", 0x922a4, b_TBlueData_33, 4, 778},
    {"libTBlueData.so", 0x92374, b_TBlueData_34, 4, 779},
    {"libTBlueData.so", 0x92338, b_TBlueData_35, 4, 780},
};

// libhdmpve.so: 27 patches
static const uint8_t b_hdmpve_0[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpve_1[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpve_2[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpve_3[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpve_4[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpve_5[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpve_6[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpve_7[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpve_8[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpve_9[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpve_10[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpve_11[] = {0x20, 0x00, 0x80, 0x52, 0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpve_12[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpve_13[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpve_14[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpve_15[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpve_16[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpve_17[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpve_18[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpve_19[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpve_20[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpve_21[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpve_22[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpve_23[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpve_24[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpve_25[] = {0xC0, 0x03, 0x5F, 0xD6};
static const uint8_t b_hdmpve_26[] = {0xC0, 0x03, 0x5F, 0xD6};

static stealth::Patch patches_hdmpve[] = {
    {"libhdmpve.so", 0x2e9b90, b_hdmpve_0, 4, 781},
    {"libhdmpve.so", 0x3f4ea8, b_hdmpve_1, 4, 782},
    {"libhdmpve.so", 0x10e0c8, b_hdmpve_2, 4, 783},
    {"libhdmpve.so", 0x3d95b8, b_hdmpve_3, 4, 784},
    {"libhdmpve.so", 0x10e078, b_hdmpve_4, 4, 785},
    {"libhdmpve.so", 0xba684, b_hdmpve_5, 4, 786},
    {"libhdmpve.so", 0x3f512c, b_hdmpve_6, 4, 787},
    {"libhdmpve.so", 0x158b3c, b_hdmpve_7, 4, 788},
    {"libhdmpve.so", 0x3f4fcc, b_hdmpve_8, 4, 789},
    {"libhdmpve.so", 0x3f521c, b_hdmpve_9, 4, 790},
    {"libhdmpve.so", 0xba558, b_hdmpve_10, 4, 791},
    {"libhdmpve.so", 0x247a9c, b_hdmpve_11, 8, 792},
    {"libhdmpve.so", 0x3f4e5c, b_hdmpve_12, 4, 793},
    {"libhdmpve.so", 0x3f521c, b_hdmpve_13, 4, 794},
    {"libhdmpve.so", 0x3f4ea8, b_hdmpve_14, 4, 795},
    {"libhdmpve.so", 0x3f4fcc, b_hdmpve_15, 4, 796},
    {"libhdmpve.so", 0x3f512c, b_hdmpve_16, 4, 797},
    {"libhdmpve.so", 0x3d95b8, b_hdmpve_17, 4, 798},
    {"libhdmpve.so", 0x3d95f0, b_hdmpve_18, 4, 799},
    {"libhdmpve.so", 0x3d955c, b_hdmpve_19, 4, 800},
    {"libhdmpve.so", 0x3d94d8, b_hdmpve_20, 4, 801},
    {"libhdmpve.so", 0x3d9590, b_hdmpve_21, 4, 802},
    {"libhdmpve.so", 0x10e0c8, b_hdmpve_22, 4, 803},
    {"libhdmpve.so", 0x10e078, b_hdmpve_23, 4, 804},
    {"libhdmpve.so", 0x2e9b90, b_hdmpve_24, 4, 805},
    {"libhdmpve.so", 0x1b3ac0, b_hdmpve_25, 4, 806},
    {"libhdmpve.so", 0x1b411c, b_hdmpve_26, 4, 807},
};

// Total: 808 patches
