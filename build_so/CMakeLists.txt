cmake_minimum_required(VERSION 3.10)
project(bypass LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optimization and security flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -fPIC -fvisibility=hidden -ffunction-sections -fdata-sections")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections -Wl,--strip-all")

add_library(bypass SHARED main.cpp)

# Link Android system libraries
target_link_libraries(bypass
    log     # Android logging
    dl      # dlopen/dlsym
)

# Strip symbols in release
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set_target_properties(bypass PROPERTIES LINK_FLAGS "-s")
endif()
