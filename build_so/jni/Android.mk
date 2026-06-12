LOCAL_PATH := $(call my-dir)

# Dobby prebuilt
include $(CLEAR_VARS)
LOCAL_MODULE := dobby
LOCAL_SRC_FILES := Main/Dobby/libraries/$(TARGET_ARCH_ABI)/libdobby.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/Main/Dobby
include $(PREBUILT_STATIC_LIBRARY)

# Main module
include $(CLEAR_VARS)
LOCAL_MODULE := bypass
LOCAL_SRC_FILES := main.cpp sdk.cpp Main/Tools.cpp \
    Main/KittyMemory/KittyMemory.cpp \
    Main/KittyMemory/KittyUtils.cpp \
    Main/KittyMemory/MemoryBackup.cpp \
    Main/KittyMemory/MemoryPatch.cpp
LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/Main $(LOCAL_PATH)/Main/KittyMemory
LOCAL_CPPFLAGS := -std=c++17 -O2 -fvisibility=hidden -DNDEBUG -w
LOCAL_LDFLAGS := -s
LOCAL_LDLIBS := -llog -ldl
LOCAL_STATIC_LIBRARIES := dobby
include $(BUILD_SHARED_LIBRARY)
