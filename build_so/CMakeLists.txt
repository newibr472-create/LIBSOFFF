cmake_minimum_required(VERSION 3.10)
project(bgmi_bypass)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -fPIC -fvisibility=hidden -fno-rtti -fno-exceptions")

add_library(bypass SHARED main.cpp)
target_link_libraries(bypass log dl)
target_include_directories(bypass PRIVATE ${CMAKE_SOURCE_DIR})
