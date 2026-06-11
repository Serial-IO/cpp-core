#include <cpp_core/ffi_metadata.hpp>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>

namespace
{

template <typename> struct FunctionPointerTraits;

template <typename Return, typename... Args> struct FunctionPointerTraits<Return (*)(Args...)>
{
    using ReturnType = Return;
    using ArgumentTypes = std::tuple<Args...>;
    static constexpr std::size_t kArity = sizeof...(Args);

    template <std::size_t Index> using Argument = std::tuple_element_t<Index, ArgumentTypes>;
};

using ErrorCallbackTraits = FunctionPointerTraits<ErrorCallbackT>;

static_assert(ErrorCallbackTraits::kArity == 2);
static_assert(std::is_same_v<ErrorCallbackTraits::ReturnType, void>);
static_assert(std::is_same_v<ErrorCallbackTraits::Argument<0>, int>);
static_assert(std::is_same_v<ErrorCallbackTraits::Argument<1>, const char *>);

struct CliOptions
{
    std::optional<std::string> output_path;
    enum class RuntimeProfile
    {
        kGeneric,
        kNode,
        kDeno,
        kBun,
    };

    RuntimeProfile runtime = RuntimeProfile::kGeneric;
    bool show_help = false;
};

[[nodiscard]] constexpr auto runtimeProfileName(CliOptions::RuntimeProfile runtime) -> std::string_view
{
    switch (runtime)
    {
    case CliOptions::RuntimeProfile::kGeneric:
        return "generic";
    case CliOptions::RuntimeProfile::kNode:
        return "node";
    case CliOptions::RuntimeProfile::kDeno:
        return "deno";
    case CliOptions::RuntimeProfile::kBun:
        return "bun";
    }

    return "generic";
}

[[nodiscard]] auto parseRuntimeProfile(std::string_view value, CliOptions::RuntimeProfile &runtime) -> bool
{
    if (value == "generic" || value == "universal")
    {
        runtime = CliOptions::RuntimeProfile::kGeneric;
        return true;
    }

    if (value == "node")
    {
        runtime = CliOptions::RuntimeProfile::kNode;
        return true;
    }

    if (value == "deno")
    {
        runtime = CliOptions::RuntimeProfile::kDeno;
        return true;
    }

    if (value == "bun")
    {
        runtime = CliOptions::RuntimeProfile::kBun;
        return true;
    }

    return false;
}

[[nodiscard]] constexpr auto toNativeParameterType(cpp_core::AbiValueKind kind) -> std::string_view
{
    switch (kind)
    {
    case cpp_core::AbiValueKind::kVoid:
        return "void";
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
    }

    return "pointer";
}

[[nodiscard]] constexpr auto toNativeResultType(cpp_core::AbiValueKind kind) -> std::string_view
{
    switch (kind)
    {
    case cpp_core::AbiValueKind::kVoid:
        return "void";
    case cpp_core::AbiValueKind::kInt32:
        return "i32";
    case cpp_core::AbiValueKind::kInt64:
        return "i64";
    case cpp_core::AbiValueKind::kOpaqueHandle:
        return "isize";
    case cpp_core::AbiValueKind::kUtf8CString:
    case cpp_core::AbiValueKind::kMutableBuffer:
    case cpp_core::AbiValueKind::kConstBuffer:
    case cpp_core::AbiValueKind::kOpaquePointer:
    case cpp_core::AbiValueKind::kVersionStructPointer:
    case cpp_core::AbiValueKind::kErrorCallback:
    case cpp_core::AbiValueKind::kNotificationCallback:
        return "pointer";
    }

    return "void";
}

[[nodiscard]] constexpr auto toNativeResultType(const cpp_core::FunctionDescriptor &function) -> std::string_view
{
    return toNativeResultType(function.return_abi_kind);
}

[[nodiscard]] auto toTsParameterType(const cpp_core::ParameterDescriptor &parameter) -> std::string_view
{
    switch (parameter.abi_kind)
    {
    case cpp_core::AbiValueKind::kInt32:
        return "number";
    case cpp_core::AbiValueKind::kInt64:
    case cpp_core::AbiValueKind::kOpaqueHandle:
        return "number | bigint";
    case cpp_core::AbiValueKind::kUtf8CString:
        return "string | PointerValue";
    case cpp_core::AbiValueKind::kMutableBuffer:
        return "ArrayBufferView | PointerValue";
    case cpp_core::AbiValueKind::kConstBuffer:
        return "string | ArrayBufferView | PointerValue";
    case cpp_core::AbiValueKind::kOpaquePointer:
    case cpp_core::AbiValueKind::kVersionStructPointer:
        return "ArrayBufferView | PointerValue";
    case cpp_core::AbiValueKind::kErrorCallback:
    case cpp_core::AbiValueKind::kNotificationCallback:
        return "PointerValue | null";
    case cpp_core::AbiValueKind::kVoid:
        return "void";
    }

    return "unknown";
}

[[nodiscard]] auto toWrapperReturnType(const cpp_core::FunctionDescriptor &function) -> std::string_view
{
    switch (function.result_model)
    {
    case cpp_core::FunctionResultModel::kVoid:
    case cpp_core::FunctionResultModel::kStatusCode:
        return "void";
    case cpp_core::FunctionResultModel::kValueOrStatus:
        return function.return_abi_kind == cpp_core::AbiValueKind::kInt64 ? "bigint" : "number";
    case cpp_core::FunctionResultModel::kHandleOrStatus:
        return "bigint";
    }

    return "unknown";
}

[[nodiscard]] auto usesBufferMarshalling(const cpp_core::FunctionDescriptor &function) -> bool
{
    return std::ranges::any_of(function.parameters, [](const auto &parameter) {
        return parameter.abi_kind == cpp_core::AbiValueKind::kUtf8CString ||
               parameter.abi_kind == cpp_core::AbiValueKind::kConstBuffer ||
               parameter.abi_kind == cpp_core::AbiValueKind::kMutableBuffer ||
               parameter.abi_kind == cpp_core::AbiValueKind::kOpaquePointer ||
               parameter.abi_kind == cpp_core::AbiValueKind::kVersionStructPointer;
    });
}

[[nodiscard]] auto optionalParameterDefault(const cpp_core::ParameterDescriptor &parameter)
    -> std::optional<std::string_view>
{
    if (!parameter.optional)
    {
        return std::nullopt;
    }

    switch (parameter.abi_kind)
    {
    case cpp_core::AbiValueKind::kInt32:
        return "0";
    case cpp_core::AbiValueKind::kErrorCallback:
    case cpp_core::AbiValueKind::kNotificationCallback:
        return "null";
    default:
        return std::nullopt;
    }
}

[[nodiscard]] auto sourceExpr(const cpp_core::ParameterDescriptor &parameter) -> std::string
{
    return std::string(parameter.name);
}

auto printUsage(std::ostream &stream) -> void
{
    stream << "Usage: cpp_core_bindgen [--output PATH] [--runtime generic|node|deno|bun] [--help]\n";
    stream << "Generates a TypeScript FFI module from cpp-core FFI metadata.\n";
}

[[nodiscard]] auto parseArgs(int argc, char **argv, CliOptions &options) -> bool
{
    for (int index = 1; index < argc; ++index)
    {
        const std::string_view arg = argv[index];
        if (arg == "--help" || arg == "-h")
        {
            options.show_help = true;
            return true;
        }

        if (arg == "--output")
        {
            if (index + 1 >= argc)
            {
                return false;
            }

            options.output_path = std::string(argv[++index]);
            continue;
        }

        if (arg == "--runtime")
        {
            if (index + 1 >= argc)
            {
                return false;
            }

            if (!parseRuntimeProfile(argv[++index], options.runtime))
            {
                return false;
            }

            continue;
        }

        return false;
    }

    return true;
}

auto writeHeader(std::ostream &stream, CliOptions::RuntimeProfile runtime) -> void
{
    stream << "// Generated from cpp-core reflection metadata.\n";
    stream << "// Do not edit manually.\n";
    stream << "// Intended for use with a host-specific FFI adapter.\n";
    stream << "// ABI mode: modern.\n";
    stream << "// Runtime profile: " << runtimeProfileName(runtime) << ".\n\n";
}

auto writeHostTypes(std::ostream &stream) -> void
{
    stream << "export type OpaquePointer = object;\n";
    stream << "export type PointerValue = number | bigint | OpaquePointer;\n";
    stream << "export type NativeType = \"void\" | \"i32\" | \"i64\" | \"isize\" | \"pointer\";\n\n";
    stream << "export type NativeFunctionDefinition = {\n";
    stream << "  readonly parameters: readonly NativeType[];\n";
    stream << "  readonly result: NativeType;\n";
    stream << "};\n\n";
    stream << "type NativeValue<T extends NativeType> = T extends \"pointer\"\n";
    stream << "  ? PointerValue | null\n";
    stream << "  : T extends \"i64\" | \"isize\"\n";
    stream << "    ? bigint\n";
    stream << "    : T extends \"i32\"\n";
    stream << "      ? number\n";
    stream << "      : void;\n\n";
    stream << "type NativeFunctionArguments<Parameters extends readonly NativeType[]> = {\n";
    stream << "  [Index in keyof Parameters]: NativeValue<Parameters[Index]>;\n";
    stream << "};\n\n";
    stream << "type NativeFunctionResult<Result extends NativeType> = NativeValue<Result>;\n\n";
    stream << "type NativeFunction<Definition extends NativeFunctionDefinition> = (\n";
    stream << "  ...args: NativeFunctionArguments<Definition[\"parameters\"]>\n";
    stream << ") => NativeFunctionResult<Definition[\"result\"]>;\n\n";
    stream << "export type CallbackHandle = {\n";
    stream << "  pointer: PointerValue;\n";
    stream << "  close(): void;\n";
    stream << "};\n\n";
    stream << "export interface BindgenRuntimeAdapter {\n";
    stream << "  loadLibrary(path: string | URL, symbols: Record<string, NativeFunctionDefinition>): BindgenLibrary;\n";
    stream << "  pointerOf(value: ArrayBuffer | ArrayBufferView): PointerValue | null;\n";
    stream << "  readCString(pointer: PointerValue): string | null;\n";
    stream << "  createCallback<Definition extends NativeFunctionDefinition>(\n";
    stream << "    definition: Definition,\n";
    stream << "    callback: (...args: NativeFunctionArguments<Definition[\"parameters\"]>) => "
              "NativeFunctionResult<Definition[\"result\"]>,\n";
    stream << "  ): CallbackHandle;\n";
    stream << "}\n\n";
    stream << "export interface BindgenHost {\n";
    stream << "  pointerOf(value: ArrayBuffer | ArrayBufferView): PointerValue | null;\n";
    stream << "  readCString(pointer: PointerValue): string | null;\n";
    stream << "  createCallback<Definition extends NativeFunctionDefinition>(\n";
    stream << "    definition: Definition,\n";
    stream << "    callback: (...args: NativeFunctionArguments<Definition[\"parameters\"]>) => "
              "NativeFunctionResult<Definition[\"result\"]>,\n";
    stream << "  ): CallbackHandle;\n";
    stream << "}\n\n";
}

auto writeSymbols(std::ostream &stream) -> void
{
    stream << "export const symbols = {\n";

    for (const auto &function : cpp_core::functionDescriptors())
    {
        stream << "  " << function.name << ": {\n";
        stream << "    parameters: [";

        for (std::size_t index = 0; index < function.parameters.size(); ++index)
        {
            if (index != 0)
            {
                stream << ", ";
            }

            stream << '"' << toNativeParameterType(function.parameters[index].abi_kind) << '"';
        }

        stream << "],\n";
        stream << "    result: \"" << toNativeResultType(function) << "\",\n";
        stream << "  },\n";
    }

    stream << "} as const;\n\n";
    stream << "type SymbolDefinitions = typeof symbols;\n\n";
    stream << "export type BindgenSymbols = {\n";
    stream << "  [Name in keyof SymbolDefinitions]: NativeFunction<SymbolDefinitions[Name]>;\n";
    stream << "};\n\n";
    stream << "export interface BindgenLibrary {\n";
    stream << "  symbols: BindgenSymbols;\n";
    stream << "}\n\n";
}

auto writeOperations(std::ostream &stream) -> void
{
    stream << "export const operations = {\n";
    for (const auto &operation : cpp_core::operationDescriptors())
    {
        stream << "  " << operation.name << ": { symbol: \"" << operation.function_name << "\" },\n";
    }
    stream << "} as const;\n\n";
}

auto writeStatusCodes(std::ostream &stream) -> void
{
    stream << "export const statusCodes = {\n";
    for (const auto &descriptor : cpp_core::statusCodeDescriptors())
    {
        stream << "  " << descriptor.name << ": " << descriptor.value << ",\n";
    }
    stream << "} as const;\n\n";

    stream << "export const statusCodeInfo = {\n";
    for (const auto &descriptor : cpp_core::statusCodeDescriptors())
    {
        stream << "  \"" << descriptor.value << "\": { category: \"" << descriptor.category << "\", name: \""
               << descriptor.name << "\" },\n";
    }
    stream << "} as const;\n\n";
}

auto writeHelpers(std::ostream &stream) -> void
{
    stream << "const textEncoder = new TextEncoder();\n\n";
    stream << "type KeepAlive = ArrayBufferView[];\n\n";
    stream << "function isArrayBufferView(value: unknown): value is ArrayBufferView {\n";
    stream << "  return ArrayBuffer.isView(value);\n";
    stream << "}\n\n";
    stream << "function isArrayBuffer(value: unknown): value is ArrayBuffer {\n";
    stream << "  return value instanceof ArrayBuffer;\n";
    stream << "}\n\n";
    stream << "function isPointerValue(value: unknown): value is PointerValue {\n";
    stream << "  return value != null && !isArrayBufferView(value) && !isArrayBuffer(value) &&\n";
    stream << "    (typeof value === \"number\" || typeof value === \"bigint\" || typeof value === \"object\");\n";
    stream << "}\n\n";
    stream << "function requirePointer(\n";
    stream << "  host: BindgenHost,\n";
    stream << "  value: PointerValue | ArrayBufferView | null | undefined,\n";
    stream << "  name: string,\n";
    stream << "  keepAlive?: KeepAlive,\n";
    stream << "): PointerValue {\n";
    stream << "  if (isPointerValue(value)) {\n";
    stream << "    return value;\n";
    stream << "  }\n";
    stream << "  if (value == null || !isArrayBufferView(value)) {\n";
    stream << "    throw new TypeError(`${name} must be a PointerValue or ArrayBufferView`);\n";
    stream << "  }\n";
    stream << "  keepAlive?.push(value);\n";
    stream << "  const pointer = host.pointerOf(value);\n";
    stream << "  if (pointer === null) {\n";
    stream << "    throw new TypeError(`Failed to get pointer for ${name}`);\n";
    stream << "  }\n";
    stream << "  return pointer;\n";
    stream << "}\n\n";
    stream << "function decodeCStringPointer(host: BindgenHost, value: PointerValue | null): string | null {\n";
    stream << "  if (value === null) {\n";
    stream << "    return null;\n";
    stream << "  }\n";
    stream << "  return host.readCString(value);\n";
    stream << "}\n\n";
    stream << "function marshalCString(\n";
    stream << "  host: BindgenHost,\n";
    stream << "  value: string | PointerValue,\n";
    stream << "  name: string,\n";
    stream << "  keepAlive: KeepAlive,\n";
    stream << "): PointerValue {\n";
    stream << "  if (typeof value === \"string\") {\n";
    stream << "    const bytes = textEncoder.encode(`${value}\\0`);\n";
    stream << "    keepAlive.push(bytes);\n";
    stream << "    return requirePointer(host, bytes, name, keepAlive);\n";
    stream << "  }\n";
    stream << "  return requirePointer(host, value, name);\n";
    stream << "}\n\n";
    stream << "function marshalConstBuffer(\n";
    stream << "  host: BindgenHost,\n";
    stream << "  value: string | ArrayBufferView | PointerValue,\n";
    stream << "  name: string,\n";
    stream << "  keepAlive: KeepAlive,\n";
    stream << "): PointerValue {\n";
    stream << "  if (typeof value === \"string\") {\n";
    stream << "    const bytes = textEncoder.encode(value);\n";
    stream << "    keepAlive.push(bytes);\n";
    stream << "    return requirePointer(host, bytes, name, keepAlive);\n";
    stream << "  }\n";
    stream << "  return requirePointer(host, value, name, keepAlive);\n";
    stream << "}\n\n";
    stream << "function marshalMutableBuffer(\n";
    stream << "  host: BindgenHost,\n";
    stream << "  value: ArrayBufferView | PointerValue,\n";
    stream << "  name: string,\n";
    stream << "  keepAlive: KeepAlive,\n";
    stream << "): PointerValue {\n";
    stream << "  return requirePointer(host, value, name, keepAlive);\n";
    stream << "}\n\n";
    stream << "function toInt64(value: number | bigint): bigint {\n";
    stream << "  return typeof value === \"bigint\" ? value : BigInt(value);\n";
    stream << "}\n\n";
    stream << "export class StatusCodeError extends Error {\n";
    stream << "  readonly code: number;\n";
    stream << "  readonly category: string;\n";
    stream << "  readonly statusName: string;\n\n";
    stream << "  constructor(code: number | bigint) {\n";
    stream << "    const normalized = typeof code === \"bigint\" ? Number(code) : code;\n";
    stream << "    const info = statusCodeInfo[String(normalized) as keyof typeof statusCodeInfo];\n";
    stream << "    const category = info?.category ?? \"Unknown\";\n";
    stream << "    const statusName = info?.name ?? \"UnknownStatus\";\n";
    stream << "    super(`${category}::${statusName} (${normalized})`);\n";
    stream << "    this.name = \"StatusCodeError\";\n";
    stream << "    this.code = normalized;\n";
    stream << "    this.category = category;\n";
    stream << "    this.statusName = statusName;\n";
    stream << "  }\n";
    stream << "}\n\n";
    stream << "function assertStatus(status: number | bigint): void {\n";
    stream << "  const normalized = typeof status === \"bigint\" ? Number(status) : status;\n";
    stream << "  if (normalized < 0) {\n";
    stream << "    throw new StatusCodeError(normalized);\n";
    stream << "  }\n";
    stream << "}\n\n";
}

auto writeCustomRuntimeAdapters(std::ostream &stream) -> void
{
    stream << "export function createHostFromRuntimeAdapter(adapter: BindgenRuntimeAdapter): BindgenHost {\n";
    stream << "  return {\n";
    stream << "    pointerOf(value) {\n";
    stream << "      return adapter.pointerOf(value);\n";
    stream << "    },\n";
    stream << "    readCString(pointer) {\n";
    stream << "      return adapter.readCString(pointer);\n";
    stream << "    },\n";
    stream << "    createCallback(definition, callback) {\n";
    stream << "      return adapter.createCallback(definition, callback);\n";
    stream << "    },\n";
    stream << "  };\n";
    stream << "}\n\n";

    stream << "export function loadLibraryFromRuntimeAdapter(\n";
    stream << "  adapter: BindgenRuntimeAdapter,\n";
    stream << "  path: string | URL,\n";
    stream << "  definitions: Record<string, NativeFunctionDefinition> = symbols,\n";
    stream << "): BindgenLibrary {\n";
    stream << "  return adapter.loadLibrary(path, definitions);\n";
    stream << "}\n\n";
}

auto writeDenoRuntimeAdapters(std::ostream &stream) -> void
{
    stream << "function getDenoRuntime(): {\n";
    stream << "  dlopen(path: string | URL, definitions: Record<string, NativeFunctionDefinition>): BindgenLibrary & "
              "{ close(): void };\n";
    stream << "  UnsafePointer: { of(value: ArrayBuffer | ArrayBufferView): PointerValue | null };\n";
    stream << "  UnsafePointerView: new (pointer: PointerValue) => { getCString(): string | null };\n";
    stream << "  UnsafeCallback: new (\n";
    stream << "    definition: NativeFunctionDefinition,\n";
    stream << "    callback: (...args: unknown[]) => unknown,\n";
    stream << "  ) => { pointer: PointerValue; close(): void };\n";
    stream << "} {\n";
    stream << "  const runtime = (globalThis as typeof globalThis & { Deno?: unknown }).Deno;\n";
    stream << "  if (!runtime) {\n";
    stream << "    throw new Error(\"Deno runtime is not available.\");\n";
    stream << "  }\n";
    stream << "  return runtime as {\n";
    stream << "    dlopen(path: string | URL, definitions: Record<string, NativeFunctionDefinition>): BindgenLibrary & "
              "{ close(): void };\n";
    stream << "    UnsafePointer: { of(value: ArrayBuffer | ArrayBufferView): PointerValue | null };\n";
    stream << "    UnsafePointerView: new (pointer: PointerValue) => { getCString(): string | null };\n";
    stream << "    UnsafeCallback: new (\n";
    stream << "      definition: NativeFunctionDefinition,\n";
    stream << "      callback: (...args: unknown[]) => unknown,\n";
    stream << "    ) => { pointer: PointerValue; close(): void };\n";
    stream << "  };\n";
    stream << "}\n\n";

    stream << "export function createDenoHost(): BindgenHost {\n";
    stream << "  const deno = getDenoRuntime();\n";
    stream << "  return {\n";
    stream << "    pointerOf(value) {\n";
    stream << "      return deno.UnsafePointer.of(value);\n";
    stream << "    },\n";
    stream << "    readCString(pointer) {\n";
    stream << "      return new deno.UnsafePointerView(pointer).getCString();\n";
    stream << "    },\n";
    stream << "    createCallback(definition, callback) {\n";
    stream << "      const unsafe = new deno.UnsafeCallback(definition, callback as (...args: unknown[]) => unknown);\n";
    stream << "      return {\n";
    stream << "        pointer: unsafe.pointer,\n";
    stream << "        close() {\n";
    stream << "          unsafe.close();\n";
    stream << "        },\n";
    stream << "      };\n";
    stream << "    },\n";
    stream << "  };\n";
    stream << "}\n\n";

    stream << "export function loadDenoLibrary(\n";
    stream << "  path: string | URL,\n";
    stream << "  definitions: Record<string, NativeFunctionDefinition> = symbols,\n";
    stream << "): BindgenLibrary & { close(): void } {\n";
    stream << "  return getDenoRuntime().dlopen(path, definitions);\n";
    stream << "}\n\n";
}

auto writeBunRuntimeAdapters(std::ostream &stream) -> void
{
    stream << "type BunFfiModule = {\n";
    stream << "  FFIType: {\n";
    stream << "    void: unknown;\n";
    stream << "    i32: unknown;\n";
    stream << "    i64_fast: unknown;\n";
    stream << "    ptr: unknown;\n";
    stream << "  };\n";
    stream << "  CString: new (pointer: number, byteOffset?: number, byteLength?: number) => { toString(): string };\n";
    stream << "  JSCallback: new (\n";
    stream << "    callback: (...args: unknown[]) => unknown,\n";
    stream << "    options: { args: unknown[]; returns: unknown },\n";
    stream << "  ) => { ptr: number | bigint; close(): void };\n";
    stream << "  dlopen(\n";
    stream << "    path: string,\n";
    stream << "    symbols: Record<string, { args: unknown[]; returns: unknown }>,\n";
    stream << "  ): BindgenLibrary & { close(): void };\n";
    stream << "};\n\n";

    stream << "async function getBunFfiModule(): Promise<BunFfiModule> {\n";
    stream << "  const bun = (globalThis as typeof globalThis & { Bun?: unknown }).Bun;\n";
    stream << "  if (!bun) {\n";
    stream << "    throw new Error(\"Bun runtime is not available.\");\n";
    stream << "  }\n";
    stream << "  const specifier = \"bun:ffi\";\n";
    stream << "  return await import(specifier) as BunFfiModule;\n";
    stream << "}\n\n";

    stream << "function toBunFfiType(module: BunFfiModule, type: NativeType): unknown {\n";
    stream << "  switch (type) {\n";
    stream << "    case \"void\":\n";
    stream << "      return module.FFIType.void;\n";
    stream << "    case \"i32\":\n";
    stream << "      return module.FFIType.i32;\n";
    stream << "    case \"i64\":\n";
    stream << "    case \"isize\":\n";
    stream << "      return module.FFIType.i64_fast;\n";
    stream << "    case \"pointer\":\n";
    stream << "      return module.FFIType.ptr;\n";
    stream << "  }\n";
    stream << "}\n\n";

    stream << "export async function createBunHost(): Promise<BindgenHost> {\n";
    stream << "  const ffi = await getBunFfiModule();\n";
    stream << "  const bun = (globalThis as typeof globalThis & {\n";
    stream << "    Bun?: { ptr(value: ArrayBuffer | ArrayBufferView): number | bigint };\n";
    stream << "  }).Bun;\n";
    stream << "  if (!bun) {\n";
    stream << "    throw new Error(\"Bun runtime is not available.\");\n";
    stream << "  }\n";
    stream << "  return {\n";
    stream << "    pointerOf(value) {\n";
    stream << "      return bun.ptr(value);\n";
    stream << "    },\n";
    stream << "    readCString(pointer) {\n";
    stream << "      return new ffi.CString(Number(pointer)).toString();\n";
    stream << "    },\n";
    stream << "    createCallback(definition, callback) {\n";
    stream << "      const jsCallback = new ffi.JSCallback(callback as (...args: unknown[]) => unknown, {\n";
    stream << "        args: definition.parameters.map((type) => toBunFfiType(ffi, type)),\n";
    stream << "        returns: toBunFfiType(ffi, definition.result),\n";
    stream << "      });\n";
    stream << "      return {\n";
    stream << "        pointer: jsCallback.ptr,\n";
    stream << "        close() {\n";
    stream << "          jsCallback.close();\n";
    stream << "        },\n";
    stream << "      };\n";
    stream << "    },\n";
    stream << "  };\n";
    stream << "}\n\n";

    stream << "export async function loadBunLibrary(\n";
    stream << "  path: string,\n";
    stream << "  definitions: Record<string, NativeFunctionDefinition> = symbols,\n";
    stream << "): Promise<BindgenLibrary & { close(): void }> {\n";
    stream << "  const ffi = await getBunFfiModule();\n";
    stream << "  const ffiSymbols = Object.fromEntries(\n";
    stream << "    Object.entries(definitions).map(([name, definition]) => [\n";
    stream << "      name,\n";
    stream << "      {\n";
    stream << "        args: definition.parameters.map((type) => toBunFfiType(ffi, type)),\n";
    stream << "        returns: toBunFfiType(ffi, definition.result),\n";
    stream << "      },\n";
    stream << "    ]),\n";
    stream << "  );\n";
    stream << "  return ffi.dlopen(path, ffiSymbols);\n";
    stream << "}\n\n";
}

auto writeRuntimeAdapters(std::ostream &stream, CliOptions::RuntimeProfile runtime) -> void
{
    writeCustomRuntimeAdapters(stream);

    if (runtime == CliOptions::RuntimeProfile::kDeno)
    {
        writeDenoRuntimeAdapters(stream);
    }
    else if (runtime == CliOptions::RuntimeProfile::kBun)
    {
        writeBunRuntimeAdapters(stream);
    }
}

auto writeErrorCallbackSupport(std::ostream &stream) -> void
{
    stream << "export const errorCallbackDefinition = {\n";
    stream << "  parameters: [\"i32\", \"pointer\"],\n";
    stream << "  result: \"void\",\n";
    stream << "} as const;\n\n";

    stream << "export type RawErrorCallback = (error_code: number, message: PointerValue | null) => void;\n";
    stream << "export type ErrorCallback = (error_code: number, message: string | null) => void;\n\n";

    stream << "export function createRawErrorCallback(host: BindgenHost, callback: RawErrorCallback): CallbackHandle "
              "{\n";
    stream << "  return host.createCallback(errorCallbackDefinition, callback);\n";
    stream << "}\n\n";

    stream << "export function createErrorCallback(host: BindgenHost, callback: ErrorCallback): CallbackHandle {\n";
    stream << "  return host.createCallback(errorCallbackDefinition, (error_code, message) => {\n";
    stream << "    callback(error_code, decodeCStringPointer(host, message));\n";
    stream << "  });\n";
    stream << "}\n\n";
}

auto writeWrapperBody(std::ostream &stream, const cpp_core::FunctionDescriptor &function) -> void
{
    const bool use_keep_alive = usesBufferMarshalling(function);
    if (use_keep_alive)
    {
        stream << "      const keepAlive: KeepAlive = [];\n";
    }

    for (const auto &parameter : function.parameters)
    {
        const std::string source = sourceExpr(parameter);
        switch (parameter.abi_kind)
        {
        case cpp_core::AbiValueKind::kUtf8CString:
            stream << "      const " << parameter.name << "Pointer = marshalCString(host, " << source << ", \""
                   << parameter.name << "\", keepAlive);\n";
            break;
        case cpp_core::AbiValueKind::kConstBuffer:
            stream << "      const " << parameter.name << "Pointer = marshalConstBuffer(host, " << source << ", \""
                   << parameter.name << "\", keepAlive);\n";
            break;
        case cpp_core::AbiValueKind::kMutableBuffer:
            stream << "      const " << parameter.name
                   << "Pointer = marshalMutableBuffer(host, " << source << ", \"" << parameter.name
                   << "\", keepAlive);\n";
            break;
        case cpp_core::AbiValueKind::kOpaquePointer:
        case cpp_core::AbiValueKind::kVersionStructPointer:
            stream << "      const " << parameter.name << "Pointer = requirePointer(host, " << source << ", \""
                   << parameter.name << "\", keepAlive);\n";
            break;
        case cpp_core::AbiValueKind::kInt64:
        case cpp_core::AbiValueKind::kOpaqueHandle:
            stream << "      const " << parameter.name << "Value = toInt64(" << source << ");\n";
            break;
        default:
            break;
        }
    }

    stream << "      const rawResult = dylib.symbols." << function.name << "(";
    for (std::size_t index = 0; index < function.parameters.size(); ++index)
    {
        if (index != 0)
        {
            stream << ", ";
        }

        const auto &parameter = function.parameters[index];
        switch (parameter.abi_kind)
        {
        case cpp_core::AbiValueKind::kUtf8CString:
        case cpp_core::AbiValueKind::kConstBuffer:
        case cpp_core::AbiValueKind::kMutableBuffer:
        case cpp_core::AbiValueKind::kOpaquePointer:
        case cpp_core::AbiValueKind::kVersionStructPointer:
            stream << parameter.name << "Pointer";
            break;
        case cpp_core::AbiValueKind::kInt64:
        case cpp_core::AbiValueKind::kOpaqueHandle:
            stream << parameter.name << "Value";
            break;
        default:
            stream << sourceExpr(parameter);
            break;
        }
    }
    stream << ");\n";

    if (use_keep_alive)
    {
        stream << "      void keepAlive;\n";
    }

    switch (function.result_model)
    {
    case cpp_core::FunctionResultModel::kVoid:
        stream << "      return rawResult;\n";
        break;
    case cpp_core::FunctionResultModel::kStatusCode:
        stream << "      assertStatus(rawResult);\n";
        stream << "      return;\n";
        break;
    case cpp_core::FunctionResultModel::kValueOrStatus:
    case cpp_core::FunctionResultModel::kHandleOrStatus:
        stream << "      assertStatus(rawResult);\n";
        stream << "      return rawResult;\n";
        break;
    }
}

auto writeWrapperSignature(std::ostream &stream, const cpp_core::FunctionDescriptor &function) -> void
{
    stream << "    " << function.name << "(";

    for (std::size_t index = 0; index < function.parameters.size(); ++index)
    {
        if (index != 0)
        {
            stream << ", ";
        }

        const auto &parameter = function.parameters[index];
        stream << parameter.name << ": " << toTsParameterType(parameter);
        if (const auto default_value = optionalParameterDefault(parameter))
        {
            stream << " = " << *default_value;
        }
    }

    stream << "): " << toWrapperReturnType(function);
}

auto writeWrapperFunctions(std::ostream &stream) -> void
{
    stream << "export function createBindings(host: BindgenHost, dylib: BindgenLibrary) {\n";
    stream << "  return {\n";

    for (const auto &function : cpp_core::functionDescriptors())
    {
        writeWrapperSignature(stream, function);
        stream << " {\n";
        writeWrapperBody(stream, function);
        stream << "    },\n";
    }

    stream << "  };\n";
    stream << "}\n\n";
    stream << "export type GeneratedBindings = ReturnType<typeof createBindings>;\n";
}

auto writeModule(std::ostream &stream, CliOptions::RuntimeProfile runtime) -> void
{
    writeHeader(stream, runtime);
    writeHostTypes(stream);
    writeSymbols(stream);
    writeOperations(stream);
    writeStatusCodes(stream);
    writeHelpers(stream);
    writeRuntimeAdapters(stream, runtime);
    writeErrorCallbackSupport(stream);
    writeWrapperFunctions(stream);
}

} // namespace

auto main(int argc, char **argv) -> int
{
    CliOptions options;
    if (!parseArgs(argc, argv, options))
    {
        printUsage(std::cerr);
        return 1;
    }

    if (options.show_help)
    {
        printUsage(std::cout);
        return 0;
    }

    if (options.output_path.has_value())
    {
        std::ofstream output(*options.output_path);
        if (!output)
        {
            std::cerr << "Failed to open output path: " << *options.output_path << '\n';
            return 1;
        }

        writeModule(output, options.runtime);
        return 0;
    }

    writeModule(std::cout, options.runtime);
    return 0;
}
