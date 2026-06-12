LOCAL_PATH := $(call my-dir)
MAIN_LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libdobby
LOCAL_SRC_FILES := Main/Dobby/libraries/$(TARGET_ARCH_ABI)/libdobby.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/Main/Dobby/
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := And64InlineHook
LOCAL_SRC_FILES := Main/And64InlineHook/And64InlineHook.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := bypass

LOCAL_CFLAGS := -Wno-error=format-security -fvisibility=hidden -ffunction-sections -fdata-sections -w
LOCAL_CFLAGS += -fno-rtti -fno-exceptions -fpermissive
LOCAL_CPPFLAGS := -Wno-error=format-security -fvisibility=hidden -ffunction-sections -fdata-sections -w -Werror -s -std=c++17
LOCAL_CPPFLAGS += -Wno-error=c++11-narrowing -fms-extensions -fno-rtti -fno-exceptions -fpermissive
LOCAL_LDFLAGS += -Wl,--gc-sections,--strip-all, -llog
LOCAL_ARM_MODE := arm

LOCAL_C_INCLUDES += $(MAIN_LOCAL_PATH)

LOCAL_SRC_FILES := main.cpp \
        Main/tools.cpp \
        Main/KittyMemory/KittyMemory.cpp \
        Main/KittyMemory/MemoryPatch.cpp \
        Main/KittyMemory/MemoryBackup.cpp \
        Main/KittyMemory/KittyUtils.cpp \
        Offset/SDK/NIKON_Basic.cpp \
        Offset/SDK/NIKON_Basic_functions.cpp \
        Offset/SDK/NIKON_CoreUObject_functions.cpp \
        Offset/SDK/NIKON_Engine_functions.cpp \
        Offset/SDK/NIKON_ShadowTrackerExtra_functions.cpp \
        Offset/SDK/NIKON_Client_functions.cpp

LOCAL_SRC_FILES += Main/syscall.s

LOCAL_CPP_FEATURES := exceptions
LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv2 -lGLESv3 -lGLESv1_CM -lz
LOCAL_STATIC_LIBRARIES := libdobby And64InlineHook
include $(BUILD_SHARED_LIBRARY)
