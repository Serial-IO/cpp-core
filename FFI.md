# FFI Runtime Notes

`cpp_core_bindgen` generates a runtime-neutral TypeScript layer:

- `symbols` ABI metadata
- `createBindings(host, dylib)` wrappers
- `createErrorCallback(host, ...)`
- built-in Deno helpers
- built-in Bun helpers
- custom adapter helpers
- `BindgenHost` / `BindgenLibrary` / `BindgenRuntimeAdapter` interfaces

That means the generated file is not tied to one runtime anymore. Some adapters are already generated for you, and you can still plug in your own.

## Runtime Profiles

By default, the generator emits the generic adapter path only:

```sh
./build/gcc/cpp_core_bindgen --output cpp_core_bindings.ts
```

You can opt into a runtime-specific profile:

```sh
./build/gcc/cpp_core_bindgen --runtime deno --output cpp_core_bindings.ts
./build/gcc/cpp_core_bindgen --runtime bun --output cpp_core_bindings.ts
```

Supported values:

- `generic`
- `node`
- `deno`
- `bun`

## Quick Status

| Runtime | Status | Notes |
| --- | --- | --- |
| Generic / Node.js | Default | Use `createHostFromRuntimeAdapter()` and `loadLibraryFromRuntimeAdapter()`. |
| Deno | Opt-in built in | Generate with `--runtime deno`, then use `createDenoHost()` and `loadDenoLibrary()`. |
| Bun | Opt-in built in | Generate with `--runtime bun`, then use `await createBunHost()` and `await loadBunLibrary()`. |

## Common Shape

All runtime examples follow the same pattern:

```ts
import {
  createBindings,
  createErrorCallback,
  createHostFromRuntimeAdapter,
  loadLibraryFromRuntimeAdapter,
} from "./cpp_core_bindings.ts";

const host = createHostFromRuntimeAdapter(myRuntimeAdapter);
const dylib = loadLibraryFromRuntimeAdapter(
  myRuntimeAdapter,
  "./libcpp_bindings_linux.so",
);

const serial = createBindings(host, dylib);
const errorCallback = createErrorCallback(host, (code, message) => {
  console.error(code, message);
});

const handle = serial.serialOpen("/dev/ttyUSB0", 115200, 8, 0, 0, errorCallback.pointer);
serial.serialWrite(handle, new Uint8Array([0x41, 0x54, 0x0d]), 3, 500, 0);
serial.serialClose(handle);
errorCallback.close();
```

The only runtime-specific parts are:

- the runtime adapter itself
- or the built-in Deno/Bun helpers

## Deno

Deno helpers are generated when you build with `--runtime deno`.

```ts
import {
  createBindings,
  createErrorCallback,
  createDenoHost,
  loadDenoLibrary,
  symbols,
} from "./cpp_core_bindings.ts";

const host = createDenoHost();
const dylib = loadDenoLibrary("./libcpp_bindings_linux.so", symbols);
const serial = createBindings(host, dylib);
const errorCallback = createErrorCallback(host, (code, message) => {
  console.error(code, message);
});

const handle = serial.serialOpen("/dev/ttyUSB0", 115200, 8, 0, 0, errorCallback.pointer);
serial.serialClose(handle);
errorCallback.close();
dylib.close();
```

What the generated file already gives you:

```ts
createDenoHost()
loadDenoLibrary(path, symbols?)
```

## Bun

Bun helpers are generated when you build with `--runtime bun`.

```ts
import {
  createBindings,
  createBunHost,
  loadBunLibrary,
  symbols,
} from "./cpp_core_bindings.ts";

const host = await createBunHost();
const dylib = await loadBunLibrary("./libcpp_bindings_linux.so", symbols);
const serial = createBindings(host, dylib);

const handle = serial.serialOpen("/dev/ttyUSB0", 115200, 8);
serial.serialClose(handle);
dylib.close();
```

What the generated file already gives you:

```ts
await createBunHost()
await loadBunLibrary(path, symbols?)
```

## Node.js

Node uses the default generic output. There is no built-in Node loader implementation, because there is no single official `dlopen`-style JS API that matches Deno/Bun. The intended path is: provide a Node-side bridge and plug it into the generated custom-adapter helpers.

```ts
import native from "./build/Release/cpp_core.node";

import {
  createBindings,
  createHostFromRuntimeAdapter,
  loadLibraryFromRuntimeAdapter,
  symbols,
  type BindgenRuntimeAdapter,
  BindgenLibrary,
  NativeFunctionDefinition,
  PointerValue,
} from "./cpp_core_bindings.ts";

type NativeAddon = {
  loadLibrary(path: string, symbols: Record<string, NativeFunctionDefinition>): BindgenLibrary;
  pointerOf(value: ArrayBufferView): PointerValue | null;
  readCString(pointer: PointerValue): string | null;
  createCallback(
    definition: NativeFunctionDefinition,
    callback: (...args: unknown[]) => unknown,
  ): { pointer: PointerValue; close(): void };
};

const addon = native as NativeAddon;

const adapter: BindgenRuntimeAdapter = {
  loadLibrary(path, definitions) {
    return addon.loadLibrary(String(path), definitions);
  },
  pointerOf(value) {
    return addon.pointerOf(value);
  },
  readCString(pointer) {
    return addon.readCString(pointer);
  },
  createCallback(definition, callback) {
    return addon.createCallback(definition, callback);
  },
};

const host = createHostFromRuntimeAdapter(adapter);
const dylib = loadLibraryFromRuntimeAdapter(
  adapter,
  "./libcpp_bindings_linux.so",
  symbols,
);
const serial = createBindings(host, dylib);
```

For Node, the missing piece is still the addon implementation behind `cpp_core.node`. The generated TypeScript layer already provides the integration points:

- `BindgenRuntimeAdapter`
- `createHostFromRuntimeAdapter(...)`
- `loadLibraryFromRuntimeAdapter(...)`

## Practical Takeaway

- Generic / Node: built in as a custom-adapter path
- Deno: built in when generated with `--runtime deno`
- Bun: built in when generated with `--runtime bun`
- Node loader implementation itself is still yours

## Official References

- Deno FFI: https://docs.deno.com/runtime/fundamentals/ffi/
- Deno `Deno.dlopen`: https://docs.deno.com/api/deno/~/Deno.dlopen
- Bun FFI: https://bun.sh/docs/runtime/ffi
- Node addons: https://nodejs.org/api/addons.html
- Node-API: https://nodejs.org/api/n-api.html
