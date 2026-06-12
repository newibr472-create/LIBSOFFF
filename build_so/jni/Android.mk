LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := bypass
LOCAL_SRC_FILES := main.cpp sdk.cpp Main/Tools.cpp \
    Main/KittyMemory/KittyMemory.cpp Main/KittyMemory/KittyUtils.cpp \
    Main/KittyMemory/MemoryBackup.cpp Main/KittyMemory/MemoryPatch.cpp

LOCAL_CPPFLAGS := -std=c++17 -O2 -fvisibility=hidden -ffunction-sections -fdata-sections -DNDEBUG
LOCAL_LDFLAGS := -Wl,--gc-sections -Wl,--strip-all
LOCAL_LDLIBS := -llog -landroid -latomic
LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/Main $(LOCAL_PATH)/Main/KittyMemory $(LOCAL_PATH)/Main/Dobby
LOCAL_STATIC_LIBRARIES := dobby

include $(BUILD_SHARED_LIBRARY)

# Dobby prebuilt
include $(CLEAR_VARS)
LOCAL_MODULE := dobby
LOCAL_SRC_FILES := Main/Dobby/libraries/$(TARGET_ARCH_ABI)/libdobby.a
include $(PREBUILT_STATIC_LIBRARY)
