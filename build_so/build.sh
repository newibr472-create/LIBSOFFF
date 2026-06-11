#!/bin/bash
set -e

echo "=== Building libbypass.so (ARM64) ==="

# Find NDK
NDK=""
for path in /opt/android-ndk /opt/android-ndk-* /usr/local/android-ndk* $ANDROID_NDK_HOME $NDK_HOME; do
    if [ -d "$path" ] && [ -f "$path/build/cmake/android.toolchain.cmake" ]; then
        NDK="$path"
        break
    fi
done

if [ -n "$NDK" ]; then
    echo "Using NDK at: $NDK"
    mkdir -p build && cd build
    cmake .. \
        -DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake \
        -DANDROID_ABI=arm64-v8a \
        -DANDROID_PLATFORM=android-26 \
        -DANDROID_STL=c++_static \
        -DCMAKE_BUILD_TYPE=Release
    make -j$(nproc)
    mkdir -p ../libs/arm64-v8a
    cp libbypass.so ../libs/arm64-v8a/
    echo "✅ Built with NDK: ../libs/arm64-v8a/libbypass.so"
else
    echo "No Android NDK found. Using aarch64-linux-gnu cross-compiler..."
    
    # Check for cross compiler
    if ! command -v aarch64-linux-gnu-g++ &>/dev/null; then
        echo "ERROR: No aarch64-linux-gnu-g++ found. Install with:"
        echo "  sudo apt-get install g++-aarch64-linux-gnu"
        exit 1
    fi
    
    mkdir -p libs/arm64-v8a
    
    # Compile with cross-compiler
    # Note: Without NDK we don't have Android headers, so we use stub JNI headers
    aarch64-linux-gnu-g++ \
        -shared -fPIC \
        -O2 \
        -std=c++17 \
        -fvisibility=hidden \
        -ffunction-sections -fdata-sections \
        -Wl,--gc-sections \
        -Wl,--strip-all \
        -DANDROID \
        -I./stubs \
        -o libs/arm64-v8a/libbypass.so \
        main.cpp \
        -ldl -lpthread \
        2>&1
    
    echo "✅ Built with cross-compiler: libs/arm64-v8a/libbypass.so"
fi

ls -la libs/arm64-v8a/libbypass.so
file libs/arm64-v8a/libbypass.so
echo "=== Done ==="
