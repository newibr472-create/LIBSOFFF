#!/bin/bash
# Build BGMI 4.4.0 bypass .so with proper SDK integration
set -e

CXX=aarch64-linux-gnu-g++
OUT=libs/arm64-v8a/libbypass.so
mkdir -p libs/arm64-v8a

$CXX -std=c++17 -shared -fPIC -O2 \
    -fno-rtti -fno-exceptions -fpermissive \
    -D__LP64__ -DANDROID -D__ANDROID__ \
    -D__forceinline="inline" \
    -include cstring -include cmath -include locale \
    -I. -I./android \
    -o $OUT \
    main.cpp \
    SDK/NIKON_Basic.cpp \
    SDK/NIKON_CoreUObject_functions.cpp \
    sdk_functions.cpp \
    -static-libstdc++ -static-libgcc \
    -ldl -lpthread \
    -Wl,--gc-sections,-strip-all \
    -w

echo "[+] Built: $OUT ($(du -h $OUT | cut -f1))"
file $OUT
