# MinGW-w64 Toolchain file for cpp-core
# Usage:
#   cmake -B build-win -DCMAKE_TOOLCHAIN_FILE=cmake/mingw64-toolchain.cmake ..
#   cmake --build build-win

# Target system
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# MinGW-w64 compilers
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)

# Optional, useful for Windows resources
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

# Search behavior for cross compiling
set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Compiler-specific flags
set(CMAKE_CXX_FLAGS_INIT "-Wall -Wextra -Wpedantic")
set(CMAKE_C_FLAGS_INIT "-Wall -Wextra -Wpedantic")

set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g -O0")
set(CMAKE_C_FLAGS_DEBUG_INIT "-g -O0")

set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3 -DNDEBUG")
set(CMAKE_C_FLAGS_RELEASE_INIT "-O3 -DNDEBUG")

# Recommended for Deno-loaded DLLs:
# statically link MinGW runtime parts, but keep Windows system DLLs dynamic.
set(CMAKE_SHARED_LINKER_FLAGS_INIT "-static-libgcc -static-libstdc++")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-static-libgcc -static-libstdc++")
