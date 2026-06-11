#!/bin/bash
set -e

echo "[*] Building libbypass.so (ARM64)"

cd "$(dirname "$0")"

# Clean previous build
rm -rf build_arm64 libs
mkdir -p build_arm64 libs/arm64-v8a

# Compile with aarch64-linux-gnu cross compiler
# Using direct compilation (no cmake needed for single file)
# Build fully static (no external dependencies except bionic which is always there)
# All C++ stdlib linked statically so no .so.6 dependencies
aarch64-linux-gnu-g++ \
    -shared \
    -o libs/arm64-v8a/libbypass.so \
    main.cpp \
    -I. \
    -std=c++17 \
    -O2 \
    -fPIC \
    -fvisibility=hidden \
    -fno-rtti \
    -fno-exceptions \
    -DANDROID \
    -D__ANDROID__ \
    -D__LP64__ \
    -Wall \
    -Wno-unused-variable \
    -Wno-unused-function \
    -Wno-format \
    -s \
    -Wl,--gc-sections \
    -Wl,--hash-style=sysv \
    -static-libstdc++ \
    -static-libgcc \
    -Wl,-Bstatic -lstdc++ -lgcc -Wl,-Bdynamic \
    -lm -ldl -lpthread -lc

# Post-process: patch SONAME and fix library names for Android compatibility  
# The .so will be loaded via dlopen on Android where bionic provides libc/libm/libdl
# On Android these are just libc.so, libm.so, libdl.so (no .6 suffix)
# Use patchelf if available, otherwise the Zygisk loader handles this
if command -v patchelf &> /dev/null; then
    patchelf --replace-needed libm.so.6 libm.so libs/arm64-v8a/libbypass.so 2>/dev/null || true
    patchelf --replace-needed libc.so.6 libc.so libs/arm64-v8a/libbypass.so 2>/dev/null || true
    patchelf --replace-needed libdl.so.2 libdl.so libs/arm64-v8a/libbypass.so 2>/dev/null || true
    patchelf --replace-needed libpthread.so.0 libc.so libs/arm64-v8a/libbypass.so 2>/dev/null || true
    patchelf --remove-needed ld-linux-aarch64.so.1 libs/arm64-v8a/libbypass.so 2>/dev/null || true
fi

echo "[+] Build successful!"
echo "[+] Output: libs/arm64-v8a/libbypass.so"
ls -la libs/arm64-v8a/libbypass.so
file libs/arm64-v8a/libbypass.so
