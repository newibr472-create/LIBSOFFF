LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := bypass
LOCAL_CFLAGS    := -Wall -Wno-unused -fvisibility=hidden -fno-rtti -fno-exceptions -O2 -DNDEBUG
LOCAL_CPPFLAGS  := -std=c++17 -fno-rtti -fno-exceptions -fvisibility=hidden
LOCAL_LDFLAGS   := -Wl,-z,now,-z,relro
LOCAL_ARM_MODE  := arm

LOCAL_SRC_FILES := \
    main.cpp \
    core/memory.cpp \
    core/bypass.cpp \
    core/syscalls.S \
    features/esp.cpp \
    features/aimbot.cpp

LOCAL_LDLIBS := -llog -landroid -lz

include $(BUILD_SHARED_LIBRARY)
