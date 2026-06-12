#!/bin/bash
set -e
CXX=aarch64-linux-gnu-g++
OUT=libs/arm64-v8a/libbypass.so
mkdir -p libs/arm64-v8a

# Only compile essential SDK files (ShadowTrackerExtra is too large for quick build)
$CXX -std=c++17 -shared -fPIC -O2 \
    -fno-rtti -fno-exceptions -fpermissive \
    -D__LP64__ -DANDROID -D__ANDROID__ \
    -D__forceinline="inline" \
    -D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING \
    -Wno-narrowing -Wno-deprecated \
    -include cstring -include cmath -include codecvt -include locale \
    -I. -I./Offset \
    -o $OUT \
    main.cpp \
    Offset/SDK/NIKON_Basic.cpp \
    Offset/SDK/NIKON_CoreUObject_functions.cpp \
    Offset/SDK/NIKON_Engine_functions.cpp \
    -static-libstdc++ -static-libgcc \
    -ldl -lpthread \
    -Wl,--gc-sections,-strip-all \
    -w

echo "[+] Built: $OUT ($(du -h $OUT | cut -f1))"
file $OUT
