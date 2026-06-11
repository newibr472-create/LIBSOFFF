cmake_minimum_required(VERSION 3.10)
project(bgmi_bypass CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ARM64 cross compilation
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

add_library(bypass SHARED main.cpp)

target_compile_options(bypass PRIVATE
    -O2
    -fPIC
    -fvisibility=hidden
    -fno-rtti
    -fno-exceptions
    -DANDROID
    -D__ANDROID__
    -D__LP64__
    -Wall
    -Wno-unused-variable
    -Wno-unused-function
)

target_link_options(bypass PRIVATE
    -s
    -Wl,--gc-sections
    -Wl,-z,noexecstack
    -nostdlib++
)

target_link_libraries(bypass
    log
    dl
    c
    m
)
