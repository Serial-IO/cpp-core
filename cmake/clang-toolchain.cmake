# Clang Toolchain file for cpp-core
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=cmake/clang-toolchain.cmake ..

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ${CMAKE_HOST_SYSTEM_PROCESSOR})

# Set the C++ compiler
set(CMAKE_CXX_COMPILER "clang++-21")
set(CMAKE_C_COMPILER "clang-21")

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler-specific flags
set(CMAKE_CXX_FLAGS_INIT "-Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3 -DNDEBUG")

# Enable C++23 module support
set(CMAKE_CXX_MODULE_STD 23)
set(CMAKE_CXX_MODULE_EXTENSIONS OFF)

