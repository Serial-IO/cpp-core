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

### 1. Add *cpp-core* with CPM.cmake (empfohlener Weg)

```cmake
# Projekt-Version einmal als **Cache-Variablen** setzen –
# so landen sie garantiert in cpp-core, egal wann CPM konfiguriert.
set(CPP_CORE_VERSION_MAJOR 1 CACHE STRING "")
set(CPP_CORE_VERSION_MINOR 0 CACHE STRING "")
set(CPP_CORE_VERSION_PATCH 3 CACHE STRING "")

CPMAddPackage(
  NAME cpp_core
  GITHUB_REPOSITORY Serial-IO/cpp-core    # Fork / Upstream
  GIT_TAG main                            # oder z. B. v0.1.0
)

add_executable(my_app src/main.cpp)
target_link_libraries(my_app PRIVATE cpp_core::cpp_core)
```

> Warum Cache-Variablen?  
> Nur so ist sicher­gestellt, dass die Werte bereits **vor** dem
> `CPMAddPackage()`-Aufruf existieren.  Falls die Variablen erst später oder
> über `OPTIONS ... "CPP_CORE_VERSION_MAJOR=${FOO}"` gesetzt werden und `FOO`
> zu diesem Zeitpunkt leer ist, fällt *cpp-core* auf seine Default-Version
> (0.1.0) zurück.

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
