# C++ Core

`cpp-core` is the header-only **API and ABI contract** shared by the Serial-IO platform bindings. It defines the exported serial interface, the status-code model, build-time version information, and the reflection-backed metadata used by bindgen tooling.

This repository does not provide a ready-to-load shared library by itself. It provides the contract consumed by the platform implementations:

- [cpp-bindings-linux](https://github.com/Serial-IO/cpp-bindings-linux)
- [cpp-bindings-windows](https://github.com/Serial-IO/cpp-bindings-windows)

## Status

`cpp-core` is the canonical definition of the current reflection-based API line.

- Supported compiler family: GNU C++
- Supported Linux toolchain: GCC 16+
- Supported Windows toolchain: MinGW-w64 GCC 16+
- macOS support: not ready yet
- Required language mode: C++26 with `-freflection`
- Required build system: CMake 3.30+

Clang and MSVC are not supported at this time.

## What It Provides

- Header-only C-compatible serial API definitions under `include/cpp_core`
- Reflection-backed ABI metadata for functions, operations, status codes, and shared structs
- A generated version surface used consistently across all platform bindings
- A `cpp_core_bindgen` executable that emits Deno-oriented FFI metadata and wrappers
- An installable CMake package target: `cpp_core::cpp_core`

## Repository Layout

- `include/cpp_core/serial.h`: aggregated C ABI for serial operations
- `include/cpp_core/ffi_metadata.hpp`: compile-time metadata over the exported ABI
- `include/cpp_core/interface/get_version.h`: version struct and `getVersion`
- `tools/generate_deno_symbols.cpp`: bindgen entrypoint for Deno symbols and wrappers

`cpp-core` hard-requires the GNU compiler family with working C++26 reflection support. During configure, CMake rejects unsupported compilers and rejects GNU builds where `-freflection` is unavailable or incomplete.

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

Native GCC build:

```sh
cmake -S . -B build/gcc -G Ninja -DCMAKE_CXX_COMPILER=g++-16
cmake --build build/gcc
ctest --test-dir build/gcc
```

MinGW-w64 GCC build:

```sh
cmake -S . -B build/mingw -G Ninja -DCMAKE_TOOLCHAIN_FILE=cmake/mingw-toolchain.cmake
cmake --build build/mingw
```

The CMake project exports the package target and also builds these relevant targets:

- `cpp_core::cpp_core`: header-only interface target
- `cpp_core_bindgen`: Deno symbol and wrapper generator
- `cpp_core_compile_tests`: compile-time validation target when testing is enabled
- `cpp_core_bindgen_smoke`: generated-output smoke check when testing is enabled

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

This model keeps the ABI easy to consume from Deno, Rust, Python, or other FFI hosts without requiring C++ runtime coupling.

## Reflection Metadata

`cpp-core` exposes compile-time metadata for the ABI and the shared structs:

```cpp
#include <cpp_core/ffi_metadata.hpp>

constexpr auto functions = cpp_core::functionDescriptors();
constexpr auto operations = cpp_core::operationDescriptors();
constexpr auto statuses = cpp_core::statusCodeDescriptors();
constexpr auto serial_config_fields = cpp_core::serialConfigFieldDescriptors();
constexpr auto version_fields = cpp_core::versionFieldDescriptors();
```

Targeted lookup helpers are also available:

```cpp
constexpr auto *open_fn = cpp_core::findFunctionDescriptor("serialOpen");
constexpr auto *open_op = cpp_core::findOperationDescriptor("serialOpen");
constexpr auto *read_error =
    cpp_core::findStatusCodeDescriptor(cpp_core::StatusCode::Io::kReadError);
```

The metadata is the source of truth for:

- exported function names
- parameter names and optionality
- ABI kind classification for strings, buffers, handles, and callbacks
- result model classification
- shared struct field layout metadata

## Deno Bindgen

Build and run the bindgen tool:

```sh
cmake --build build/gcc --target cpp_core_bindgen
./build/gcc/cpp_core_bindgen --output deno_bindings.ts
```

The generated module includes:

- `symbols` for `Deno.dlopen`
- `operations` metadata
- `statusCodes` and status lookup data
- `StatusCodeError`
- `createBindings(dylib)` wrappers
- callback helpers such as `createErrorCallback(...)`

Minimal Deno usage:

```ts
import { createBindings, symbols } from "./deno_bindings.ts";

const dylib = Deno.dlopen("./libcpp_bindings_linux.so", symbols);
const serial = createBindings(dylib);

const handle = serial.serialOpen("/dev/ttyUSB0", 115200, 8);
serial.serialWrite(handle, new Uint8Array([0x41, 0x54, 0x0d]), 3, 500, 0);
serial.serialClose(handle);

dylib.close();
```

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

Keeping the contract, metadata, and version surface here avoids ABI drift between platforms and keeps generated binding layers aligned with the actual exported API.

## License

Licensed under `Apache-2.0`. See [LICENSE](LICENSE).
