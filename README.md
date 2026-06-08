# C++ Core

Header-only API definition library for cross-platform serial communication. Defines the **API contract** and **version information** shared by all platform-specific binding implementations.

> [!IMPORTANT]
>
> This version of `cpp-core` requires **GCC 16**, **C++26**, and **reflection support**.
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

Clang and MSVC are not supported.

## C Interface

`cpp-core` defines a C-compatible interface for the platform-specific binding libraries and exposes reflection-backed
metadata for that same interface.

Metadata entrypoints:

```cpp
#include <cpp_core/ffi_metadata.hpp>

constexpr auto functions = cpp_core::functionDescriptors();
constexpr auto operations = cpp_core::operationDescriptors();
constexpr auto statuses = cpp_core::statusCodeDescriptors();
constexpr auto config_fields = cpp_core::serialConfigFieldDescriptors();
```

Bindgen target:

```sh
cmake --build build/gcc --target cpp_core_bindgen
./build/gcc/cpp_core_bindgen --output deno_bindings.ts
```

The generated module contains:

* `symbols` for `Deno.dlopen`
* `operations` metadata
* `statusCodes` plus `StatusCodeError`
* `createBindings(dylib)` which returns positional TypeScript wrapper functions

Example:

```ts
import {
  createBindings,
  operations,
  StatusCodeError,
  statusCodeInfo,
  statusCodes,
  symbols,
} from "./deno_bindings.ts";

const dylib = Deno.dlopen("./libcpp_bindings_linux.so", symbols);
const serial = createBindings(dylib);
const decoder = new TextDecoder();

let handle: bigint | undefined;

function describeStatus(code: number) {
  const info = statusCodeInfo[String(code) as keyof typeof statusCodeInfo];
  return info ? `${info.category}::${info.name}` : `UnknownStatus (${code})`;
}

try {
  console.log("opening via symbol", operations.serialOpen.symbol);

  handle = serial.serialOpen("/dev/ttyUSB0", 115200, 8, 0, 0);

  const writeBuffer = new Uint8Array([0x41, 0x54, 0x49, 0x0d, 0x0a]);
  const bytesWritten = serial.serialWrite(handle, writeBuffer, writeBuffer.byteLength, 500, 0);

  const queued = serial.serialInBytesWaiting(handle);
  const readBuffer = new Uint8Array(256);
  const bytesRead = serial.serialRead(handle, readBuffer, readBuffer.byteLength, 500, 0);

  const payload = decoder.decode(readBuffer.subarray(0, bytesRead));

  console.log({
    handle,
    bytesWritten,
    queued,
    bytesRead,
    payload,
  });
} catch (error) {
  if (error instanceof StatusCodeError) {
    if (error.code === statusCodes.NotFoundError) {
      console.error("port not found");
    } else if (error.code === statusCodes.ReadError) {
      console.error("read failed");
    } else {
      console.error(describeStatus(error.code));
    }
    throw error;
  }
  throw error;
} finally {
  if (handle !== undefined) {
    try {
      serial.serialClose(handle);
    } catch (error) {
      if (error instanceof StatusCodeError) {
        console.error("close failed:", describeStatus(error.code));
      } else {
        console.error("close failed:", error);
      }
    }
  }
  dylib.close();
}
```

## Benefits

The metadata layer provides:

* **One source of truth** for exported function names, parameter names, return semantics, and status categories
* **No manual re-parsing of headers** when you want to derive Deno FFI symbol definitions or internal binding manifests
* **Generated wrapper functions** with positional parameters, result decoding, and status-to-exception conversion
* **Safer refactors** because compile-time metadata checks fail when the exposed signatures drift
* **Shared understanding of the ABI** because buffers, callbacks, UTF-8 strings, opaque handles, and status-returning functions are classified centrally
* **Less version drift in consumers** because the same `cpp-core` checkout provides both the headers and the generator used by Linux/Windows builds

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

## Error Handling

The API uses `status + out-params` or `value-or-negative-status`, because that maps cleanly to plain C and Deno FFI.

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

The corresponding Deno FFI shape is straightforward:

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

const handle = serial.serialOpen("/dev/ttyUSB0", 115200, 8);
```

## TypeScript Wrapper Examples

The generated wrapper layer is intended to feel like ordinary TypeScript, not a metadata-driven object API.

Simple open / write / close flow:

```ts
import { createBindings, symbols } from "./deno_bindings.ts";

const dylib = Deno.dlopen("./libcpp_bindings_linux.so", symbols);
const serial = createBindings(dylib);

const handle = serial.serialOpen("/dev/ttyUSB0", 115200, 8);
serial.serialWrite(handle, new Uint8Array([0x41, 0x54, 0x0d]), 3, 500, 0);
serial.serialClose(handle);

dylib.close();
```

Using optional ABI parameters through normal TypeScript defaults:

```ts
const handle = serial.serialOpen("/dev/ttyUSB0", 115200, 8);
const available = serial.serialInBytesWaiting(handle);
const line = new Uint8Array(256);
const bytesRead = serial.serialReadLine(handle, line, line.byteLength, 250, 2);
```

Handling wrapper-thrown `StatusCodeError`:

```ts
import {
  createBindings,
  StatusCodeError,
  statusCodes,
  symbols,
} from "./deno_bindings.ts";

const dylib = Deno.dlopen("./libcpp_bindings_linux.so", symbols);
const serial = createBindings(dylib);

try {
  serial.serialOpen("/dev/does-not-exist", 115200, 8);
} catch (error) {
  if (error instanceof StatusCodeError) {
    if (error.code === statusCodes.NotFoundError) {
      console.error("port not found");
    }
    console.error(error.category, error.statusName, error.code);
  }
}
```

Passing an explicit error callback pointer to the generated wrapper:

```ts
import {
  createBindings,
  createErrorCallback,
  symbols,
} from "./deno_bindings.ts";

const dylib = Deno.dlopen("./libcpp_bindings_linux.so", symbols);
const serial = createBindings(dylib);

const errorCallback = createErrorCallback((error_code, message) => {
  console.error("ffi error callback:", error_code, message ?? "<no message>");
});

try {
  serial.serialOpen("/dev/does-not-exist", 115200, 8, 0, 0, errorCallback.pointer);
} finally {
  errorCallback.close();
  dylib.close();
}
```

## Using The Metadata

The short version is:

* use `cpp_core::functionDescriptors()` when you want to inspect the exported C ABI
* use `cpp_core::operationDescriptors()` when you want a stable per-operation view next to the raw function list
* use `cpp_core::statusCodeDescriptors()` when you want symbolic error information
* use `cpp_core::serialConfigFieldDescriptors()` and `cpp_core::versionFieldDescriptors()` when you want shared struct metadata
* use `cpp_core::findFunctionDescriptor()`, `cpp_core::findOperationDescriptor()`, and `cpp_core::findStatusCodeDescriptor()` for targeted lookups

## License

`Apache-2.0` - Check [LICENSE](LICENSE) for more details.
