#!/bin/bash
set -e

# Find cross-compiler
CC=""
CXX=""

if which aarch64-linux-gnu-g++ >/dev/null 2>&1; then
    CC="aarch64-linux-gnu-gcc"
    CXX="aarch64-linux-gnu-g++"
elif which aarch64-linux-android31-clang++ >/dev/null 2>&1; then
    CC="aarch64-linux-android31-clang"
    CXX="aarch64-linux-android31-clang++"
else
    echo "[*] No cross-compiler found, searching NDK..."
    NDK_PATH=$(find /opt /usr/local -name "aarch64-linux-android*-clang++" 2>/dev/null | head -1)
    if [ -n "$NDK_PATH" ]; then
        CXX="$NDK_PATH"
        CC="${NDK_PATH/clang++/clang}"
    fi
fi

if [ -z "$CXX" ]; then
    echo "[!] No ARM64 cross-compiler found, trying system clang with target"
    CC="clang"
    CXX="clang++"
    EXTRA_FLAGS="--target=aarch64-linux-android26 --sysroot=/dev/null"
    
    # Last resort: use g++ if it can target ARM64
    if ! $CXX --target=aarch64-linux-android26 -x c++ -c /dev/null -o /dev/null 2>/dev/null; then
        echo "[*] Trying aarch64-linux-gnu-g++..."
        CXX="aarch64-linux-gnu-g++"
        CC="aarch64-linux-gnu-gcc"
        EXTRA_FLAGS="-static-libgcc -static-libstdc++"
    fi
fi

echo "[*] Using compiler: $CXX"

mkdir -p libs/arm64-v8a

$CXX -std=c++17 \
    -O2 -fPIC -shared \
    -fvisibility=hidden \
    -fno-rtti -fno-exceptions \
    -DANDROID -D__ANDROID__ \
    $EXTRA_FLAGS \
    -I. \
    -o libs/arm64-v8a/libbypass.so \
    main.cpp \
    -llog -ldl 2>&1 || {
    
    # If linking -llog -ldl fails (cross-compile without Android sysroot),
    # compile without linking those (they'll resolve at runtime)
    echo "[*] Retrying without explicit -llog -ldl..."
    $CXX -std=c++17 \
        -O2 -fPIC -shared \
        -fvisibility=hidden \
        -fno-rtti -fno-exceptions \
        -DANDROID -D__ANDROID__ \
        $EXTRA_FLAGS \
        -I. \
        -Wl,--unresolved-symbols=ignore-all \
        -o libs/arm64-v8a/libbypass.so \
        main.cpp
}

echo "[+] Built: libs/arm64-v8a/libbypass.so"
file libs/arm64-v8a/libbypass.so
ls -la libs/arm64-v8a/libbypass.so
