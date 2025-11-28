# C++ Core

Header-only API definition library for cross-platform serial communication. Defines the **API contract** and **version information** shared by all platform-specific binding implementations.

> [!IMPORTANT]
>
> **API definitions only** (headers). For implementations and ready-to-use shared libraries:
> - [cpp-bindings-windows](https://github.com/Serial-IO/cpp-bindings-windows) - Windows (DLL)
> - [cpp-bindings-linux](https://github.com/Serial-IO/cpp-bindings-linux) - Linux (SO)
> - [cpp-bindings-macos](https://github.com/Serial-IO/cpp-bindings-macos) - macOS (dylib)

## Features

* **C++23** header-only API definitions
* **Cross-platform serial I/O API** (POSIX/Windows compatible)
* **Centralized version information** from Git tags (`cpp_core::kVersion`)
* **Status codes** enum for error handling
* **C-compatible API** for FFI bindings (Rust, Python, Deno, etc.)

## Requirements

* CMake ≥ 3.30
* A C++23 compatible compiler (GCC 13+, Clang 16+, MSVC 2022+)
* Git (for automatic version detection)

## Version Information

The version is **automatically extracted from Git tags** during CMake configuration and cannot be overridden:

* **With Git tag** (e.g., `v1.2.3`): Version is `1.2.3` (or `1.2.3-dirty` if working tree has uncommitted changes)
* **Without Git tag**: Version is `0.0.0` (or `0.0.0-dirty` if working tree has uncommitted changes)

All binding repositories that include this repository will use the same version information, ensuring consistency across platforms.

```cpp
#include <cpp_core/version.h>

constexpr auto v = cpp_core::kVersion;  // v.major, v.minor, v.patch
```

## Usage in Binding Repositories

Binding repositories typically include this repository as a dependency:

```cmake
CPMAddPackage(
  NAME cpp_core
  GITHUB_REPOSITORY Serial-IO/cpp-core
  GIT_TAG v1.2.3  # Version tag determines the API version
)

target_link_libraries(my_binding_library PRIVATE cpp_core::cpp_core)
```

The binding implementation then uses the API definitions:

```cpp
#include <cpp_core/serial.h>
#include <cpp_core/version.h>

// Implement platform-specific functions matching the API
intptr_t serialOpen(void *port, int baudrate, int data_bits, int parity, int stop_bits, ErrorCallbackT error_callback) {
    // Platform-specific implementation (Windows/Linux/macOS)
}
```

## License

`Apache-2.0` - Check [LICENSE](LICENSE) for more details.
