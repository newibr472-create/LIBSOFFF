# ============================================================
# Android.mk — NDK Module Config (ARM64 primary)
# WOLVES PVT BYPASS v4.0
# ============================================================
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := AkAudioVisiual
# FIX: Was ../BYPASS.cpp (compiled wrong file!) — now uses the correct source
LOCAL_SRC_FILES := ../BYPASS.cpp

# KittyMemory sources
LOCAL_SRC_FILES += ../jni/KittyMemory_repo/KittyMemory/KittyMemory.cpp
LOCAL_SRC_FILES += ../jni/KittyMemory_repo/KittyMemory/KittyScanner.cpp
LOCAL_SRC_FILES += ../jni/KittyMemory_repo/KittyMemory/KittyPtrValidator.cpp
LOCAL_SRC_FILES += ../jni/KittyMemory_repo/KittyMemory/KittyUtils.cpp
LOCAL_SRC_FILES += ../jni/KittyMemory_repo/KittyMemory/MemoryBackup.cpp
LOCAL_SRC_FILES += ../jni/KittyMemory_repo/KittyMemory/MemoryPatch.cpp
LOCAL_SRC_FILES += ../jni/KittyMemory_repo/KittyMemory/KittyAsm.cpp
LOCAL_SRC_FILES += ../jni/KittyMemory_repo/KittyMemory/KittyIOFile.cpp

# Dobby prebuilt static lib (architecture-specific)
LOCAL_STATIC_LIBRARIES := dobby
LOCAL_LDFLAGS := -L$(LOCAL_PATH)/Dobby_inc/$(TARGET_ARCH_ABI)

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/../Includes \
    $(LOCAL_PATH)/../hooks \
    $(LOCAL_PATH)/Dobby_inc \
    $(LOCAL_PATH)/KittyMemory

LOCAL_CFLAGS := -fno-rtti -fno-exceptions -fvisibility=hidden -O2 -DkNO_KEYSTONE
LOCAL_CPPFLAGS := -std=c++17

LOCAL_LDLIBS := -llog -ldl

include $(BUILD_SHARED_LIBRARY)
