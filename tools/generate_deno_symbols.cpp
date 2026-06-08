#include <cpp_core/ffi_metadata.hpp>

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

namespace
{

struct CliOptions
{
    std::optional<std::string> output_path;
    bool show_help = false;
};

[[nodiscard]] constexpr auto toDenoParameterType(cpp_core::AbiValueKind kind) -> std::string_view
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

[[nodiscard]] constexpr auto toDenoResultType(const cpp_core::FunctionDescriptor &function) -> std::string_view
{
    switch (function.return_abi_kind)
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

[[nodiscard]] auto toPascalCase(std::string_view text) -> std::string
{
    std::string out;
    out.reserve(text.size());

    bool upper_next = true;
    for (const char ch : text)
    {
        if (ch == '_' || ch == '-' || ch == ' ')
        {
            upper_next = true;
            continue;
        }

        if (upper_next)
        {
            out.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(ch))));
            upper_next = false;
            continue;
        }

        out.push_back(ch);
    }

    return out;
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
        return "string | Deno.PointerValue";
    case cpp_core::AbiValueKind::kMutableBuffer:
        return "ArrayBufferView | Deno.PointerValue";
    case cpp_core::AbiValueKind::kConstBuffer:
        return "string | ArrayBufferView | Deno.PointerValue";
    case cpp_core::AbiValueKind::kOpaquePointer:
    case cpp_core::AbiValueKind::kVersionStructPointer:
        return "ArrayBufferView | Deno.PointerValue";
    case cpp_core::AbiValueKind::kErrorCallback:
    case cpp_core::AbiValueKind::kNotificationCallback:
        return "Deno.PointerValue | null";
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

[[nodiscard]] auto sourceExpr(const cpp_core::ParameterDescriptor &parameter) -> std::string
{
    if (parameter.optional)
    {
        return std::string(parameter.name);
    }

    return "args." + std::string(parameter.name);
}

auto printUsage(std::ostream &stream) -> void
{
    stream << "Usage: cpp_core_bindgen [--output PATH] [--help]\n";
    stream << "Generates a Deno.dlopen-compatible TypeScript module from cpp-core FFI metadata.\n";
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

        return false;
    }

    return true;
}

auto writeHeader(std::ostream &stream) -> void
{
    stream << "// Generated from cpp-core reflection metadata.\n";
    stream << "// Do not edit manually.\n";
    stream << "// Intended for use with Deno.dlopen.\n";
    stream << "// ABI mode: modern.\n\n";
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

            stream << '"' << toDenoParameterType(function.parameters[index].abi_kind) << '"';
        }

        stream << "],\n";
        stream << "    result: \"" << toDenoResultType(function) << "\",\n";
        stream << "  },\n";
    }

    stream << "} as const;\n\n";
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

auto writeParamInterfaces(std::ostream &stream) -> void
{
    for (const auto &function : cpp_core::functionDescriptors())
    {
        stream << "export interface " << toPascalCase(function.name) << "Params {\n";
        for (const auto &parameter : function.parameters)
        {
            stream << "  " << parameter.name << (parameter.optional ? "?: " : ": ") << toTsParameterType(parameter)
                   << ";\n";
        }
        stream << "}\n\n";
    }
}

auto writeHelpers(std::ostream &stream) -> void
{
    stream << "const textEncoder = new TextEncoder();\n\n";
    stream << "type KeepAlive = ArrayBufferView[];\n\n";
    stream << "function isArrayBufferView(value: unknown): value is ArrayBufferView {\n";
    stream << "  return ArrayBuffer.isView(value);\n";
    stream << "}\n\n";
    stream << "function isPointerValue(value: unknown): value is Exclude<Deno.PointerValue, null> {\n";
    stream << "  return typeof value === \"number\" || typeof value === \"bigint\";\n";
    stream << "}\n\n";
    stream << "function requirePointer(\n";
    stream << "  value: Deno.PointerValue | ArrayBufferView | null | undefined,\n";
    stream << "  name: string,\n";
    stream << "  keepAlive?: KeepAlive,\n";
    stream << "): Deno.PointerValue {\n";
    stream << "  if (isPointerValue(value)) {\n";
    stream << "    return value;\n";
    stream << "  }\n";
    stream << "  if (value == null || !isArrayBufferView(value)) {\n";
    stream << "    throw new TypeError(`${name} must be a Deno.PointerValue or ArrayBufferView`);\n";
    stream << "  }\n";
    stream << "  keepAlive?.push(value);\n";
    stream << "  const pointer = Deno.UnsafePointer.of(value as BufferSource);\n";
    stream << "  if (pointer === null) {\n";
    stream << "    throw new TypeError(`Failed to get pointer for ${name}`);\n";
    stream << "  }\n";
    stream << "  return pointer;\n";
    stream << "}\n\n";
    stream << "function marshalCString(value: string | Deno.PointerValue, name: string, keepAlive: KeepAlive): "
              "Deno.PointerValue {\n";
    stream << "  if (typeof value === \"string\") {\n";
    stream << "    const bytes = textEncoder.encode(`${value}\\0`);\n";
    stream << "    keepAlive.push(bytes);\n";
    stream << "    return requirePointer(bytes, name, keepAlive);\n";
    stream << "  }\n";
    stream << "  return requirePointer(value, name);\n";
    stream << "}\n\n";
    stream << "function marshalConstBuffer(\n";
    stream << "  value: string | ArrayBufferView | Deno.PointerValue,\n";
    stream << "  name: string,\n";
    stream << "  keepAlive: KeepAlive,\n";
    stream << "): Deno.PointerValue {\n";
    stream << "  if (typeof value === \"string\") {\n";
    stream << "    const bytes = textEncoder.encode(value);\n";
    stream << "    keepAlive.push(bytes);\n";
    stream << "    return requirePointer(bytes, name, keepAlive);\n";
    stream << "  }\n";
    stream << "  return requirePointer(value, name, keepAlive);\n";
    stream << "}\n\n";
    stream << "function marshalMutableBuffer(\n";
    stream << "  value: ArrayBufferView | Deno.PointerValue,\n";
    stream << "  name: string,\n";
    stream << "  keepAlive: KeepAlive,\n";
    stream << "): Deno.PointerValue {\n";
    stream << "  return requirePointer(value, name, keepAlive);\n";
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

auto writeWrapperBody(std::ostream &stream, const cpp_core::FunctionDescriptor &function) -> void
{
    const bool use_keep_alive = usesBufferMarshalling(function);
    if (use_keep_alive)
    {
        stream << "      const keepAlive: KeepAlive = [];\n";
    }

    for (const auto &parameter : function.parameters)
    {
        if (!parameter.optional)
        {
            continue;
        }

        stream << "      const " << parameter.name << " = args." << parameter.name;
        if (parameter.abi_kind == cpp_core::AbiValueKind::kInt32)
        {
            stream << " ?? 0";
        }
        else if (parameter.abi_kind == cpp_core::AbiValueKind::kErrorCallback ||
                 parameter.abi_kind == cpp_core::AbiValueKind::kNotificationCallback)
        {
            stream << " ?? null";
        }
        stream << ";\n";
    }

    for (const auto &parameter : function.parameters)
    {
        const std::string source = sourceExpr(parameter);
        switch (parameter.abi_kind)
        {
        case cpp_core::AbiValueKind::kUtf8CString:
            stream << "      const " << parameter.name << "Pointer = marshalCString(" << source << ", \""
                   << parameter.name << "\", keepAlive);\n";
            break;
        case cpp_core::AbiValueKind::kConstBuffer:
            stream << "      const " << parameter.name << "Pointer = marshalConstBuffer(" << source << ", \""
                   << parameter.name << "\", keepAlive);\n";
            break;
        case cpp_core::AbiValueKind::kMutableBuffer:
            stream << "      const " << parameter.name << "Pointer = marshalMutableBuffer(" << source << ", \""
                   << parameter.name << "\", keepAlive);\n";
            break;
        case cpp_core::AbiValueKind::kOpaquePointer:
        case cpp_core::AbiValueKind::kVersionStructPointer:
            stream << "      const " << parameter.name << "Pointer = requirePointer(" << source << ", \""
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

auto writeWrapperFunctions(std::ostream &stream) -> void
{
    stream << "export type BindgenLibrary = Deno.DynamicLibrary<typeof symbols>;\n";
    stream << "export type BindgenSymbols = BindgenLibrary[\"symbols\"];\n\n";
    stream << "export function createBindings(dylib: BindgenLibrary) {\n";
    stream << "  return {\n";

    for (const auto &function : cpp_core::functionDescriptors())
    {
        stream << "    " << function.name << "(args: " << toPascalCase(function.name)
               << "Params): " << toWrapperReturnType(function) << " {\n";
        writeWrapperBody(stream, function);
        stream << "    },\n";
    }

    stream << "  };\n";
    stream << "}\n\n";
    stream << "export type GeneratedBindings = ReturnType<typeof createBindings>;\n";
}

auto writeModule(std::ostream &stream) -> void
{
    writeHeader(stream);
    writeSymbols(stream);
    writeOperations(stream);
    writeStatusCodes(stream);
    writeParamInterfaces(stream);
    writeHelpers(stream);
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

        writeModule(output);
        return 0;
    }

    writeModule(std::cout);
    return 0;
}
