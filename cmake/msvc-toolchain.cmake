# MSVC Toolchain file for cpp-core
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=cmake/msvc-toolchain.cmake ..

# Set the C++ compiler
set(CMAKE_CXX_COMPILER "cl")
set(CMAKE_C_COMPILER "cl")

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
