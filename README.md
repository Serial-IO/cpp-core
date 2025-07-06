# cpp-core

Header-only C++ helper library that provides small, cross-platform utilities and a centrally maintained **Version** struct shared by the *cpp-bindings-linux*, *cpp-bindings-windows* and *cpp-bindings-macos* repositories.

* C++17, zero runtime dependencies
* Delivered as an INTERFACE target `cpp_core::cpp_core`
* Fetchable via [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake) or regular `find_package`

---

## Requirements

* CMake ≥ 3.14
* A C++17 compatible compiler (GCC 10+, Clang 11+, MSVC 2019+)

---

## Quick Start

### 1. Add *cpp-core* with CPM.cmake

```cmake
# Your own project version
set(CPP_LINUX_VERSION_MAJOR 1)
set(CPP_LINUX_VERSION_MINOR 4)
set(CPP_LINUX_VERSION_PATCH 0)

CPMAddPackage(
  NAME cpp_core
  GITHUB_REPOSITORY Serial-IO/cpp-core    # Fork / upstream repo
  GIT_TAG main                            # or a release tag like v0.1.0
  OPTIONS
    "CPP_CORE_VERSION_MAJOR=${CPP_LINUX_VERSION_MAJOR}"
    "CPP_CORE_VERSION_MINOR=${CPP_LINUX_VERSION_MINOR}"
    "CPP_CORE_VERSION_PATCH=${CPP_LINUX_VERSION_PATCH}"
)

add_executable(my_app src/main.cpp)
# Link the header-only target – this only sets include paths / compile features
target_link_libraries(my_app PRIVATE cpp_core::cpp_core)
```

> Tip: If you already set the version variables as cache variables in your project (e.g. `set(CPP_CORE_VERSION_MAJOR 1 CACHE STRING "")`), you can omit the three `OPTIONS` entries—CPM will forward the values to *cpp-core* automatically.

### 2. Use in code

```cpp
#include <cpp_core/version.h>
#include <cpp_core/helpers.h>

int main() {
    constexpr auto v = cpp_core::VERSION;            // {1,4,0}
}
```

---

## Alternative: add_subdirectory

If you include the source code directly as a sub-repository, simply do:

```cmake
add_subdirectory(externals/cpp-core)
# the same version variables can be set before the call
```

---

## Using the package with `find_package`

After running `cmake --install` (or `make install`) you can locate the package system-wide:

```cmake
find_package(cpp_core REQUIRED)
add_executable(my_app src/main.cpp)
target_link_libraries(my_app PRIVATE cpp_core::cpp_core)
```

---

## License
Apache-2.0 – see [LICENSE](LICENSE).
