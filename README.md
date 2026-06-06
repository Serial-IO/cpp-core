# C++ Core

Header-only API definition library for cross-platform serial communication. Defines the **API contract** and **version information** shared by all platform-specific binding implementations.

> [!IMPORTANT]
>
> This branch is an **experimental GCC 16 / C++26 / reflection** branch.
>
> **API definitions only** (headers). For implementations and ready-to-use shared libraries:
> - [cpp-bindings-windows](https://github.com/Serial-IO/cpp-bindings-windows) - Windows (DLL)
> - [cpp-bindings-linux](https://github.com/Serial-IO/cpp-bindings-linux) - Linux (SO)
> - [cpp-bindings-macos](https://github.com/Serial-IO/cpp-bindings-macos) - macOS (dylib)

## Features

* **C++26 + reflection** header-only API definitions
* **Cross-platform serial I/O API** (POSIX/Windows compatible)
* **Centralized version information** from Git tags (`cpp_core::kVersion`)
* **Reflection-backed FFI metadata** for functions, status codes, and shared structs
* **C-compatible ABI** for FFI bindings (Rust, Python, Deno, etc.)

## Requirements

* CMake >= 3.30
* GCC 16 or newer
* `-std=c++26` and `-freflection`
* Git (for automatic version detection)

Clang and MSVC are intentionally not supported on this branch.

## Experimental Direction

This branch keeps the **C ABI** as the external contract while moving internal metadata to a reflection-backed C++26 model.

That split is intentional:

* the **runtime ABI** stays easy to consume from Deno FFI and other foreign runtimes
* the **C++ authoring model** gains a single metadata source for status codes, shared structs, and exported function signatures
* bind generation for Deno bindings can read `cpp_core::kFunctionDescriptors`, `cpp_core::kOperationDescriptors`,
  and the reflected struct field descriptors instead of re-parsing handwritten headers

The new metadata entrypoint is:

```cpp
#include <cpp_core/ffi_metadata.hpp>

constexpr auto functions = cpp_core::functionDescriptors();
constexpr auto operations = cpp_core::operationDescriptors();
constexpr auto statuses = cpp_core::statusCodeDescriptors();
constexpr auto config_fields = cpp_core::serialConfigFieldDescriptors();
```

More practical usage examples and the intended Deno/FFI workflow are documented in [docs/ffi-metadata.md](docs/ffi-metadata.md).

There is also a bindgen target now:

```sh
cmake --build build/gcc --target cpp_core_bindgen
./build/gcc/cpp_core_bindgen --output deno_bindings.ts
```

The generated module now contains:

* `symbols` for `Deno.dlopen`
* `operations` metadata
* `statusCodes` plus `StatusCodeError`
* `createBindings(dylib)` which returns named TypeScript wrapper functions

## Why This Helps Already

The new metadata layer is useful even before any full code generator exists:

* **One source of truth** for exported function names, parameter names, return semantics, and status categories
* **No manual re-parsing of headers** when you want to derive Deno FFI symbol definitions or internal binding manifests
* **Generated wrapper functions** with named parameters, result decoding, and status-to-exception conversion
* **Safer refactors** because compile-time metadata checks fail when the exposed signatures drift
* **Shared understanding of the ABI** because buffers, callbacks, UTF-8 strings, opaque handles, and status-returning functions are classified centrally
* **Less version drift in consumers** because the same `cpp-core` checkout now provides both the headers and the generator used by Linux/Windows builds

## Version Information

The version is **automatically extracted from Git tags** during CMake configuration and cannot be overridden:

* **With Git tag** (e.g., `v1.2.3`): Version is `1.2.3` (or `1.2.3-dirty` if working tree has uncommitted changes)
* **Without Git tag**: Version is `0.0.0` (or `0.0.0-dirty` if working tree has uncommitted changes)

All binding repositories that include this repository will use the same version information, ensuring consistency across platforms.

```cpp
#include <cpp_core/version.hpp>

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
#include <cpp_core/version.hpp>

// Implement platform-specific functions matching the API
intptr_t serialOpen(
    void *port,
    int baudrate,
    int data_bits,
    int parity,
    int stop_bits,
    ErrorCallbackT error_callback
) {
    // Platform-specific implementation (Windows/Linux/macOS)
}
```

## Error Handling Today and Later

The current ABI remains `status + out-params` or `value-or-negative-status`, because that maps cleanly to plain C and Deno FFI.

```c
intptr_t serialOpen(
    const char *port,
    int baudrate,
    int data_bits,
    int parity,
    int stop_bits,
    ErrorCallbackT error_callback
);
```

Internally the implementation can still use `std::expected`-style flows and translate them at the ABI edge into the plain C model.

The corresponding Deno FFI shape would stay straightforward:

```ts
const symbols = {
  serialOpen: {
    parameters: ["pointer", "i32", "i32", "i32", "i32", "pointer"],
    result: "isize",
  },
} as const;

const handle = dylib.symbols.serialOpen(
  cstr("/dev/ttyUSB0"),
  115200,
  8,
  0,
  0,
  null,
);
```

With the generated wrapper layer on top, the same call can be used more directly:

```ts
const dylib = Deno.dlopen(path, symbols);
const serial = createBindings(dylib);

const handle = serial.serialOpen({
  port: "/dev/ttyUSB0",
  baudrate: 115200,
  data_bits: 8,
});
```

## Using The Metadata

For concrete usage examples, see [docs/ffi-metadata.md](docs/ffi-metadata.md). The short version is:

* use `cpp_core::functionDescriptors()` when you want to inspect the exported C ABI
* use `cpp_core::operationDescriptors()` when you want a stable per-operation view next to the raw function list
* use `cpp_core::statusCodeDescriptors()` when you want symbolic error information
* use `cpp_core::serialConfigFieldDescriptors()` and `cpp_core::versionFieldDescriptors()` when you want shared struct metadata
* use `cpp_core::findFunctionDescriptor()`, `cpp_core::findOperationDescriptor()`, and `cpp_core::findStatusCodeDescriptor()` for targeted lookups

## License

`Apache-2.0` - Check [LICENSE](LICENSE) for more details.
