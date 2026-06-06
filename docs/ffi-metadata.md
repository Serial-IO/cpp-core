# FFI Metadata Guide

`cpp_core` provides a reflection-backed metadata layer for the public C ABI.

The important distinction is:

* the exported runtime interface is still plain C ABI
* the description of that interface is available as typed compile-time metadata in C++

That gives you a place to inspect the ABI and build tooling around it without maintaining a second hand-written schema.

## Available Metadata

Include:

```cpp
#include <cpp_core/ffi_metadata.hpp>
```

Main entrypoints:

* `cpp_core::functionDescriptors()`
* `cpp_core::operationDescriptors()`
* `cpp_core::statusCodeDescriptors()`
* `cpp_core::serialConfigFieldDescriptors()`
* `cpp_core::versionFieldDescriptors()`
* `cpp_core::findFunctionDescriptor(name)`
* `cpp_core::findOperationDescriptor(name)`
* `cpp_core::findStatusCodeDescriptor(code)`

Generator tooling:

* `cpp_core_bindgen` generates a Deno-oriented TypeScript module from the same metadata used by the headers
* the generated module includes `symbols`, metadata exports, `StatusCodeError`, and `createBindings(dylib)`

Main descriptor types:

* `cpp_core::FunctionDescriptor`
* `cpp_core::OperationDescriptor`
* `cpp_core::ParameterDescriptor`
* `cpp_core::StatusCodeDescriptor`
* `cpp_core::StructFieldDescriptor`
* `cpp_core::AbiValueKind`
* `cpp_core::ParameterDirection`
* `cpp_core::FunctionResultModel`

## What Problem This Solves

Before this layer, consumers had to learn ABI details from handwritten headers only.

That caused three problems:

* tools had to re-parse or duplicate knowledge about functions and parameters
* buffer/callback/string/handle semantics were implicit and spread across comments
* Deno binding generation had no canonical machine-readable source inside `cpp-core`

Now the ABI description is available in a typed form inside the library itself.

## Benefits

### 1. You can inspect the exported ABI programmatically

Example:

```cpp
#include <cpp_core/ffi_metadata.hpp>
#include <iostream>

auto main() -> int
{
    for (const auto &fn : cpp_core::functionDescriptors())
    {
        std::cout << fn.name << " -> " << fn.return_cpp_type << '\n';

        for (const auto &param : fn.parameters)
        {
            std::cout << "  - " << param.name
                      << " : " << param.cpp_type
                      << " optional=" << param.optional
                      << '\n';
        }
    }
}
```

This is useful for debugging the ABI contract, generating reports, or building glue code.

### 2. You get a normalized ABI classification

Each parameter is mapped into a semantic bucket via `AbiValueKind`.

Examples:

* `kUtf8CString`
* `kMutableBuffer`
* `kConstBuffer`
* `kOpaqueHandle`
* `kErrorCallback`
* `kNotificationCallback`

This matters because foreign runtimes care about ABI semantics, not just raw C++ spellings.

For example, `"void*"` by itself is not enough to know whether something is:

* a writable byte buffer
* a UTF-8 C string
* an opaque pointer
* a callback-associated payload

The metadata layer makes that distinction explicit.

### 3. You can reason about result behavior

Each function is classified with `FunctionResultModel`.

Result models:

* `kVoid`
* `kStatusCode`
* `kValueOrStatus`
* `kHandleOrStatus`

That lets tooling distinguish:

* functions that are pure commands
* functions that return status only
* functions that return a useful numeric value or a negative status
* functions like `serialOpen` that return a handle or a negative status

This is especially useful when generating Deno wrappers that should not treat every `int` the same way.

### 4. Status codes become machine-readable

You can inspect names, categories, and numeric values:

```cpp
#include <cpp_core/ffi_metadata.hpp>
#include <iostream>

auto main() -> int
{
    for (const auto &status : cpp_core::statusCodeDescriptors())
    {
        std::cout << status.category
                  << "::" << status.name
                  << " = " << static_cast<long long>(status.value)
                  << '\n';
    }
}
```

This is useful for:

* generating TS enums or lookup tables
* mapping numeric failures back to symbolic names
* producing consistent docs for consumers

### 5. Shared structs can be described centrally

`SerialConfig` and `Version` expose field descriptors.

This is useful for:

* generated docs for shared data
* generated FFI struct definitions and byte-layout helpers
* compatibility checks between metadata and runtime-facing declarations

## Example: Look Up One Function

```cpp
#include <cpp_core/ffi_metadata.hpp>
#include <iostream>

auto main() -> int
{
    const auto *fn = cpp_core::findFunctionDescriptor("serialRead");
    if (fn == nullptr)
    {
        return 1;
    }

    std::cout << fn->name << '\n';
    std::cout << fn->signature << '\n';

    for (const auto &param : fn->parameters)
    {
        std::cout << param.name << " -> " << param.cpp_type << '\n';
    }
}
```

Typical use case:

* lookup by exported symbol name
* build FFI binding metadata for exactly that symbol
* emit wrapper code based on parameter and result classifications

## Example: Derive Deno-Oriented Symbol Metadata

The metadata is sufficient to drive a generator directly.

Bindgen CLI:

```sh
cmake --build build/gcc --target cpp_core_bindgen
./build/gcc/cpp_core_bindgen
```

To capture the output into a TypeScript file:

```sh
./build/gcc/cpp_core_bindgen --output deno_bindings.ts
```

Example sketch:

```cpp
#include <cpp_core/ffi_metadata.hpp>

constexpr auto toDenoType(cpp_core::AbiValueKind kind) -> const char *
{
    switch (kind)
    {
    case cpp_core::AbiValueKind::kInt32:
        return "i32";
    case cpp_core::AbiValueKind::kInt64:
    case cpp_core::AbiValueKind::kOpaqueHandle:
        return "i64";
    case cpp_core::AbiValueKind::kUtf8CString:
    case cpp_core::AbiValueKind::kMutableBuffer:
    case cpp_core::AbiValueKind::kConstBuffer:
    case cpp_core::AbiValueKind::kOpaquePointer:
    case cpp_core::AbiValueKind::kVersionStructPointer:
    case cpp_core::AbiValueKind::kErrorCallback:
    case cpp_core::AbiValueKind::kNotificationCallback:
        return "pointer";
    case cpp_core::AbiValueKind::kVoid:
        return "void";
    }

    return "pointer";
}
```

The generator emits an importable module and wraps the legacy ABI:

```ts
// Generated from cpp-core reflection metadata.
// Do not edit manually.
// Intended for use with Deno.dlopen.
// ABI mode: legacy.

export const symbols = {
  serialOpen: {
    parameters: ["pointer", "i32", "i32", "i32", "i32", "pointer"],
    result: "isize",
  },
  // ...
} as const;

export const operations = {
  serialOpen: {
    symbol: "serialOpen",
  },
} as const;

export function createBindings(dylib: Deno.DynamicLibrary<typeof symbols>) {
  return {
    serialOpen(args: SerialOpenParams): bigint {
      // marshals strings
      // throws StatusCodeError on failure
    },
  };
}
```

## Scope

Limitations:

* it does not encode every possible semantic rule for every parameter
* it does not expose `std::expected` itself across the external ABI

That is deliberate. The ABI description is explicit and compile-checked.

## Relation To `std::expected`

The metadata layer does not expose `std::expected` across the ABI boundary.

That would be the wrong abstraction for Deno FFI and plain C consumers.

Instead, the model is:

* internally: implementations can use richer C++ result handling
* externally: the ABI stays plain C
* metadata: describes exported functions, parameters, shared structs, and error semantics

That gives you most of the practical benefits of structured error handling without making the FFI surface harder to consume.

## Usage

The metadata layer is useful for:

* inspect exported functions from tooling or tests
* generate a Deno metadata module with `cpp_core_bindgen`
* generate or validate Deno FFI symbol tables
* consume `createBindings(dylib)` as a higher-level TS wrapper layer with named params and automatic error conversion
* generate status-code lookup tables
* let Linux/Windows builds consume the same `cpp-core` revision for both headers and bindgen output
* keep ABI comments and generated docs consistent with the actual declarations
