# cpp-core – Library Overview

Welcome to **cpp-core**, a header-only, cross-platform C++23 helper library that bundles utilities frequently needed in systems programming and embedded scenarios.  Besides multiple compile-time helpers it exposes a fully-featured **C API for serial communication**, making the library consumable from virtually any language (Rust, Python, Deno, …).

> ⚠️ **Looking for a ready-to-use shared library?** Build one of the platform bindings instead of cpp-core itself:  
> • Windows → [cpp-bindings-windows](https://github.com/Serial-IO/cpp-bindings-windows)  
> • Linux   → [cpp-bindings-linux](https://github.com/Serial-IO/cpp-bindings-linux)

> Looking for the raw function list? See the [C API reference](api_reference.md).
>
> Want to call the library from TypeScript? Jump straight to the [Deno FFI guide](deno_ffi.md).

---

## What ships with cpp-core?

| Area | Header | Highlights |
|------|--------|------------|
| Versioning | `cpp_core/version.h` | Compile-time `constexpr` version struct, filled during the build.
| Helpers    | `cpp_core/helpers.h` | Collection of lightweight, header-only helper templates.
| Serial I/O | `cpp_core/serial.h`  | Cross-platform serial helper built on the POSIX/Win32 APIs (see [C API reference](api_reference.md)).
| Error Codes| `cpp_core/status_codes.h` | Unified negative error codes shared across languages and bindings.

All headers live in the `include/` tree and are directly usable after installation.

---

## Directory structure

```
cpp-core/
├── cmake/              # CMake helper modules
├── docs/               # You are here 📚
├── include/
│   └── cpp_core/       # Public headers (header-only!)
└── test/               # Unit tests (if enabled)
```

---

## Building the library

### 1. Fetch & configure

```bash
# clone
$ git clone https://github.com/Serial-IO/cpp-core.git
$ cd cpp-core

# create an out-of-tree build directory
$ cmake -B build -DCMAKE_BUILD_TYPE=Release
```

### 2. Build & install (header-only)

```bash
$ cmake --build build --target install
```

`install` copies the headers (and *optionally* a shared library, see below) to your default prefix (`/usr/local`, `Program Files/`, …).

### 3. Building a shared library for FFI consumers

Deno, Python and other FFI users expect a shared library (`*.so`, `*.dll`, `*.dylib`).  Enable it like so:

```bash
# add the switch – all code remains header-only for C++ callers
$ cmake -B build-shared -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release
$ cmake --build build-shared --config Release

# resulting artifact (Linux)
$ ls build-shared
libcpp_core.so  # rename or copy as needed, e.g. to libserial.so
```

> **Tip**: keep the header files close to your FFI project so you can re-generate bindings easily.

---

## Minimal C example

```c
#include <cpp_core/serial.h>
#include <stdio.h>

int main() {
    intptr_t h = serialOpen("/dev/ttyUSB0", 115200, 8, 0, 0);
    if (h <= 0) {
        fprintf(stderr, "open failed: %ld\n", (long)h);
        return 1;
    }

    const char msg[] = "hello\n";
    serialWriteLine(h, msg, sizeof msg - 1, 1000);

    char buf[64];
    int n = serialReadLine(h, buf, sizeof buf, 1000);
    fwrite(buf, 1, n, stdout);

    serialClose(h);
}
```

For a deep dive into every parameter head over to the [C API reference](api_reference.md).

---

## Going further

* **Deno, Node, Python & Rust**: see the dedicated [Deno FFI guide](deno_ffi.md) which includes a complete TypeScript example and hints for other languages.
* **Questions / issues** – open a ticket on GitHub; contributions are welcome!

---

## License

cpp-core is licensed under the terms of the Apache-2.0 license.  See [LICENSE](../LICENSE) for details. 
