LOCAL_PATH := $(call my-dir)

# ============================================================================
# KittyMemory source files (compiled directly into our .so)
# ============================================================================
KITTY_PATH := KittyMemory_repo/KittyMemory
KITTY_SRC := \
    $(KITTY_PATH)/KittyAsm.cpp \
    $(KITTY_PATH)/KittyIOFile.cpp \
    $(KITTY_PATH)/KittyMemory.cpp \
    $(KITTY_PATH)/KittyPtrValidator.cpp \
    $(KITTY_PATH)/KittyScanner.cpp \
    $(KITTY_PATH)/KittyUtils.cpp \
    $(KITTY_PATH)/MemoryBackup.cpp \
    $(KITTY_PATH)/MemoryPatch.cpp

# ============================================================================
# Dobby prebuilt static library (for inline hooks via HOOK_LIB / DobbyHook)
# Built from Dobby_repo commit e9fe7fb using NDK r25c, armeabi-v7a
# ============================================================================
include $(CLEAR_VARS)
LOCAL_MODULE := dobby_prebuilt
LOCAL_SRC_FILES := Dobby_inc/$(TARGET_ARCH_ABI)/libdobby.a
include $(PREBUILT_STATIC_LIBRARY)

# ============================================================================
# libCrashSightPlugin.so - the actual bypass shared library
# Renamed from libGVoicePlugin.so to ensure injection into main game process
# ============================================================================
include $(CLEAR_VARS)

LOCAL_MODULE := AkAudioVisiual

LOCAL_SRC_FILES := ../BYPASS.cpp $(KITTY_SRC)

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH) \
    $(LOCAL_PATH)/KittyMemory_repo \
    $(LOCAL_PATH)/Dobby_inc

LOCAL_STATIC_LIBRARIES := dobby_prebuilt

LOCAL_LDLIBS := -llog -ldl

# -DkNO_KEYSTONE : skip keystone assembler dep (we only use hex patches)
LOCAL_CPPFLAGS := -std=c++17 -fno-rtti -fno-exceptions -fvisibility=hidden -O2 -DkNO_KEYSTONE -Wno-format-security
LOCAL_CFLAGS   := -Wall -fvisibility=hidden -O2 -DkNO_KEYSTONE

include $(BUILD_SHARED_LIBRARY)

