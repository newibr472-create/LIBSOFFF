# ============================================================
# Application.mk — NDK Build Config (ARM64 + ARM32)
# WOLVES PVT BYPASS v4.0
# ============================================================
APP_ABI := arm64-v8a armeabi-v7a
APP_PLATFORM := android-21
APP_STL := c++_static
APP_OPTIM := release
APP_PIE := true
APP_CFLAGS := -O2 -DNDEBUG
APP_STRIP_MODE := --strip-all
APP_LDFLAGS := -llog
