# MSVC Toolchain file for cpp-core
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=cmake/msvc-toolchain.cmake ..

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR ${CMAKE_HOST_SYSTEM_PROCESSOR})

# Set the C++ compiler
set(CMAKE_CXX_COMPILER "cl")
set(CMAKE_C_COMPILER "cl")

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable C++23 module support
set(CMAKE_CXX_MODULE_STD 23)
set(CMAKE_CXX_MODULE_EXTENSIONS OFF)

# MSVC-specific flags
# /W4 = Enable all warnings
# /permissive- = Conformance mode
# /Zc:__cplusplus = Enable updated __cplusplus macro
# /std:c++latest = Use latest C++ standard (C++23)
set(CMAKE_CXX_FLAGS_INIT "/W4 /permissive- /Zc:__cplusplus /std:c++latest")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "/Zi /Od /MDd")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "/O2 /DNDEBUG /MD")

# MSVC-specific definitions
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
