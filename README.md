# C++ Core

Header-only C++ helper library that provides small, cross-platform utilities and a centrally maintained **Version** struct shared by the *cpp-bindings-linux*, *cpp-bindings-windows* and *cpp-bindings-macos* repositories.

> [!NOTE]
>
> This repository contains **headers only**. To obtain a working native library (SO / DLL / dylib) build one of the platform-specific projects instead:
> - [C++ bindings (Windows)](https://github.com/Serial-IO/cpp-bindings-windows)
> - [C++ bindings (Linux)](https://github.com/Serial-IO/cpp-bindings-linux)
> 
> These repositories compile *cpp-core* for you and provide the ready-to-use shared library.

## Documentation

* [Overview](docs/overview.md): Architecture, Build & Quick Start.
* C++23, zero runtime dependencies
* Delivered as an INTERFACE target `cpp_core::cpp_core`
* Fetchable via [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake) or regular `find_package`

## Requirements

* CMake ≥ 3.14
* A C++23 compatible compiler (GCC 13+, Clang 16+, MSVC 2022+)

## Quick Start

1. Add *cpp-core* with CPM.cmake (recommended way)

```cmake
# Set the project version once via **cache variables** –
# ensures the values end up in cpp-core regardless of when CPM configures.
set(CPP_CORE_VERSION_MAJOR 1 CACHE STRING "")
set(CPP_CORE_VERSION_MINOR 0 CACHE STRING "")
set(CPP_CORE_VERSION_PATCH 3 CACHE STRING "")

CPMAddPackage(
  NAME cpp_core
  GITHUB_REPOSITORY Serial-IO/cpp-core    # Fork / Upstream
  GIT_TAG main                            # or e.g. v0.1.0
)

add_executable(my_app src/main.cpp)
target_link_libraries(my_app PRIVATE cpp_core::cpp_core)
```

> Why cache variables?  
> They guarantee the values exist **before** the `CPMAddPackage()` call. If the variables are set later (or via  
> `OPTIONS ... "CPP_CORE_VERSION_MAJOR=${FOO}"`) and `FOO` is empty at that moment, *cpp-core* falls back to its default version (0.1.0).

2. Use in code

```cpp
#include <cpp_core/version.h>
#include <cpp_core/helpers.h>

int main() {
    constexpr auto v = cpp_core::VERSION;            // {1,4,0}
}
```

## Alternative: add_subdirectory

If you include the source code directly as a sub-repository, simply do:

```cmake
add_subdirectory(externals/cpp-core)
# the same version variables can be set before the call
```

## Using the package with `find_package`

After running `cmake --install` (or `make install`) you can locate the package system-wide:

```cmake
find_package(cpp_core REQUIRED)
add_executable(my_app src/main.cpp)
target_link_libraries(my_app PRIVATE cpp_core::cpp_core)
```

## License
Apache-2.0 – see [LICENSE](LICENSE).
