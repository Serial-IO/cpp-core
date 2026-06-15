# C++ Core

`cpp-core` is the header-only **API and ABI contract** shared by the Serial-IO platform bindings. It defines the exported serial interface, the status-code model, and build-time version information consumed by the platform implementations.

This repository does not provide a ready-to-load shared library by itself. It provides the contract consumed by the platform implementations:

- [cpp-bindings-linux](https://github.com/Serial-IO/cpp-bindings-linux)
- [cpp-bindings-windows](https://github.com/Serial-IO/cpp-bindings-windows)

## Status

`cpp-core` is the canonical definition of the current API line.

- Supported Linux toolchain: modern C++26-capable toolchain
- Supported Windows toolchain: modern C++26-capable toolchain
- macOS support: not ready yet
- Required language mode: C++26
- Required build system: CMake 3.30+

## What It Provides

- Header-only C-compatible serial API definitions under `include/cpp_core`
- A generated version surface used consistently across all platform bindings
- An installable CMake package target: `cpp_core::cpp_core`

## Repository Layout

- `include/cpp_core/serial.h`: aggregated C ABI for serial operations
- `include/cpp_core/status_code.h`: shared status-code model
- `include/cpp_core/interface/get_version.h`: version struct and `getVersion`

## Quick Start

Consume `cpp-core` as a CMake dependency from another project:

```cmake
CPMAddPackage(
  NAME cpp_core
  GITHUB_REPOSITORY Serial-IO/cpp-core
  GIT_TAG v1.1.0
)

target_link_libraries(my_binding PRIVATE cpp_core::cpp_core)
```

Use the exported headers in your implementation:

```cpp
#include <cpp_core/serial.h>
#include <cpp_core/interface/get_version.h>

auto serialOpen(
    void *port,
    int baudrate,
    int data_bits,
    int parity,
    int stop_bits,
    ErrorCallbackT error_callback
) -> intptr_t;
```

Read the version data baked into the checkout:

```cpp
#include <cpp_core/interface/get_version.h>

cpp_core::Version version{};
getVersion(&version);
```

## Building This Repository

Native build:

```sh
cmake -S . -B build -G Ninja
cmake --build build
ctest --test-dir build
```

The CMake project exports the package target and also builds these relevant targets:

- `cpp_core::cpp_core`: header-only interface target
- `cpp_core_compile_tests`: compile-time validation target when testing is enabled

## ABI Surface

The main aggregated interface lives in:

```cpp
#include <cpp_core/serial.h>
```

The ABI is intentionally plain-C friendly: functions either return a status code, return a value-or-negative-status, or return an opaque handle-or-negative-status.

Example:

```cpp
MODULE_API auto serialOpen(
    void *port,
    int baudrate,
    int data_bits,
    int parity = 0,
    int stop_bits = 0,
    ErrorCallbackT error_callback = nullptr
) -> intptr_t;
```

This model keeps the ABI easy to consume from TypeScript hosts, Rust, Python, or other FFI hosts without requiring C++ runtime coupling.

## Versioning

Version information is generated from Git during CMake configure and written into `include/cpp_core/version.hpp`.

- Tagged checkout: uses the tag as the base version
- Additional commits after a tag: appends the commit distance and short hash
- Dirty working tree: appends `-dirty`
- No usable tag: falls back to `0.0.0`

The version data is exposed through:

- the `version` namespace in `include/cpp_core/version.hpp`
- the `cpp_core::Version` struct
- the `getVersion(cpp_core::Version *out)` ABI function

## Relationship to Platform Repositories

`cpp-core` defines the contract. The platform repositories implement it.

- `cpp-bindings-linux` provides the Linux shared library implementation
- `cpp-bindings-windows` provides the Windows DLL implementation
- macOS bindings are not part of the supported line yet

Keeping the contract and version surface here avoids ABI drift between platforms and keeps the shared API aligned with the actual exported implementation.

## License

Licensed under `Apache-2.0`. See [LICENSE](LICENSE).
