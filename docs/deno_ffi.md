# Using cpp-core from Deno via FFI

This guide shows how to consume the **cpp-core** serial API from [Deno](https://deno.land) using the built-in FFI interface.  The same principles apply to Node-FFI, Python ctypes and Rust `libloading`.

> Make sure you have read the [library overview](overview.md) first.

---

## Prerequisites

1. **Deno ≥ 1.41** (older versions lack callback support)
2. A shared library built from **cpp-bindings-linux** / **cpp-bindings-windows** (these repos compile cpp-core for you)
3. Basic understanding of TypeScript and native pointers

### Building the shared library

```bash
# inside the cpp-bindings-linux (or cpp-bindings-windows) repository
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# copy / rename so Deno can find it next to the script
cp build/libcpp_core.so ./libserial.so  # Linux example
```

On Windows the file will be called `cpp_core.dll`, on macOS `libcpp_core.dylib`.

---

## 1. Describe the symbols

Create a file `serial.ts` next to your Deno script:

```ts
// serial.ts – symbol table for Deno.dlopen
export const symbols = {
  serialOpen: {
    parameters: ["pointer", "i32", "i32", "i32", "i32"] as const,
    result: "i64",
  },
  serialClose: { parameters: ["i64"] as const, result: "void" },
  serialReadLine: {
    parameters: ["i64", "pointer", "i32", "i32"] as const,
    result: "i32",
  },
  serialWriteLine: {
    parameters: ["i64", "pointer", "i32", "i32"] as const,
    result: "i32",
  },
  // Optional callbacks (Deno ≥ 1.41)
  serialOnError: { parameters: ["pointer"] as const, result: "void" },
} as const;
```

---

## 2. Open the library

```ts
import { symbols } from "./serial.ts";

const lib = Deno.dlopen("./libserial.so", symbols);
```

`lib.symbols` now exposes plain JavaScript functions that forward to C.

---

## 3. Helpers for strings & buffers

Most serial functions expect **C-style null-terminated strings** (aka `char*`).  A simple utility makes the conversion easier:

```ts
// cstr.ts
export function cstr(text: string) {
  const buf = new TextEncoder().encode(text + "\0");
  return [buf, Deno.UnsafePointer.of(buf) as Deno.PointerValue] as const;
}
```

---

## 4. Complete echo example

```ts
// echo.ts
import { symbols } from "./serial.ts";
import { cstr } from "./cstr.ts";

const lib = Deno.dlopen("./libserial.so", symbols);

// Open port
const [portBuf, portPtr] = cstr("/dev/ttyUSB0");
const handle = lib.symbols.serialOpen(portPtr, 115200, 8, 0, 0);
if (handle <= 0n) throw new Error(`open failed: ${handle}`);

// Send a line
const [msgBuf, msgPtr] = cstr("Hello from Deno!");
lib.symbols.serialWriteLine(handle, msgPtr, msgBuf.length - 1, 1000);

// Receive until newline (max 256 bytes)
const readBuf = new Uint8Array(256);
const bytes = lib.symbols.serialReadLine(handle, Deno.UnsafePointer.of(readBuf), readBuf.length, 1000);

if (bytes > 0) {
  console.log(new TextDecoder().decode(readBuf.subarray(0, bytes)));
}

lib.symbols.serialClose(handle);
lib.close();
```

Run it:

```bash
deno run --allow-ffi --allow-read echo.ts
```

---

## 5. Registering callbacks

Deno FFI supports **C→JS callbacks** through `Deno.UnsafeCallback`:

```ts
// error_callback.ts
import { symbols } from "./serial.ts";
const lib = Deno.dlopen("./libserial.so", symbols);

const onError = new Deno.UnsafeCallback(
  { parameters: ["i32", "pointer"], result: "void" } as const,
  (code: number, msgPtr: Deno.PointerValue) => {
    const msg = new Deno.UnsafePointerView(msgPtr).getCString();
    console.error(`serial error ${code}: ${msg}`);
  },
);

lib.symbols.serialOnError(onError.pointer);
```

Remember to `onError.close()` and `lib.close()` when shutting down to free native resources.

---

## 6. Troubleshooting

| Symptom | Solution |
|---------|----------|
| `TypeError: ffi symbol not found` | Verify the exact exported name (no C++ mangling). Use `nm -D libserial.so` on Linux. |
| `BadResource: attempted to call a non-function` | The parameters array/order does **not** match the C prototype. |
| Timeouts (`READ_ERROR` −3) | Increase `timeoutMs` or ensure the device actually sends data. |

---

## Links & further reading

* Back to [Overview](overview.md)
* Full [C API reference](api_reference.md)
* Deno FFI docs: https://docs.deno.com/runtime/manual/runtime/ffi 
