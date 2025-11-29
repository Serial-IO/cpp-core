# GCC Toolchain file for cpp-core
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=cmake/gcc-toolchain.cmake ..

# Set the C++ compiler
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_C_COMPILER "gcc")

# Compiler-specific flags
set(CMAKE_CXX_FLAGS_INIT "-Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3 -DNDEBUG")
