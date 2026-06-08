#pragma once

#include "ffi/abi_function_registry.hpp"
#include "serial.h"
#include "serial_config.hpp"
#include "status_code.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <meta>
#include <optional>
#include <span>
#include <string_view>
#include <utility>
#include <vector>

#ifndef __cpp_impl_reflection
#error "cpp_core/ffi_metadata.hpp requires C++26 reflection support."
#endif

namespace cpp_core
{

enum class AbiValueKind
{
    kVoid,
    kInt32,
    kInt64,
    kOpaqueHandle,
    kUtf8CString,
    kMutableBuffer,
    kConstBuffer,
    kOpaquePointer,
    kVersionStructPointer,
    kErrorCallback,
    kNotificationCallback,
};

enum class ParameterDirection
{
    kIn,
    kOut,
    kInOut,
};

enum class FunctionResultModel
{
    kVoid,
    kStatusCode,
    kValueOrStatus,
    kHandleOrStatus,
};

struct ParameterDescriptor
{
    std::string_view name;
    std::string_view cpp_type;
    AbiValueKind abi_kind;
    ParameterDirection direction;
    bool optional;
};

struct FunctionDescriptor
{
    std::string_view name;
    std::string_view return_cpp_type;
    std::string_view signature;
    AbiValueKind return_abi_kind;
    FunctionResultModel result_model;
    std::span<const ParameterDescriptor> parameters;
};

struct OperationDescriptor
{
    std::string_view name;
    std::string_view function_name;
};

struct StatusCodeDescriptor
{
    std::string_view category;
    std::string_view name;
    StatusCodeValue value;
};

struct StructFieldDescriptor
{
    std::string_view name;
    std::string_view cpp_type;
    AbiValueKind abi_kind;
    std::size_t offset;
    std::size_t size;
};

namespace detail
{

[[nodiscard]] consteval auto overrideParameterAbiValueKind(std::string_view function_name, std::string_view parameter_name,
                                                           std::meta::info type_info) -> std::optional<AbiValueKind>
{
    if (!std::meta::is_pointer_type(type_info))
    {
        return std::nullopt;
    }

    if (function_name == "getVersion" && parameter_name == "out")
    {
        return AbiValueKind::kVersionStructPointer;
    }

    return std::nullopt;
}

[[nodiscard]] consteval auto isConstPointer(std::meta::info type_info) -> bool
{
    if (!std::meta::is_pointer_type(type_info))
    {
        return false;
    }

    return std::meta::is_const(std::meta::remove_pointer(type_info));
}

[[nodiscard]] consteval auto inferAbiValueKind(std::meta::info type_info, std::string_view parameter_name) -> AbiValueKind
{
    if (std::meta::is_pointer_type(type_info))
    {
        if (std::meta::display_string_of(type_info) == "cpp_core::Version*")
        {
            return AbiValueKind::kVersionStructPointer;
        }

        if (parameter_name == "error_callback")
        {
            return AbiValueKind::kErrorCallback;
        }

        if (parameter_name.ends_with("_callback") || parameter_name == "callback_fn")
        {
            return AbiValueKind::kNotificationCallback;
        }

        if (parameter_name == "port")
        {
            return AbiValueKind::kUtf8CString;
        }

        if (parameter_name == "buffer")
        {
            return isConstPointer(type_info) ? AbiValueKind::kConstBuffer : AbiValueKind::kMutableBuffer;
        }

        if (parameter_name == "sequence" || parameter_name == "until_char")
        {
            return AbiValueKind::kConstBuffer;
        }

        if (std::meta::display_string_of(std::meta::remove_pointer(type_info)) == "char")
        {
            return AbiValueKind::kUtf8CString;
        }

        return AbiValueKind::kOpaquePointer;
    }

    if (std::meta::is_integral_type(type_info))
    {
        if (parameter_name == "handle")
        {
            return AbiValueKind::kOpaqueHandle;
        }

        return std::meta::size_of(type_info) > sizeof(std::int32_t) ? AbiValueKind::kInt64 : AbiValueKind::kInt32;
    }

    if (std::meta::is_void_type(type_info))
    {
        return AbiValueKind::kVoid;
    }

    return AbiValueKind::kOpaquePointer;
}

[[nodiscard]] consteval auto inferParameterAbiValueKind(std::string_view function_name, std::string_view parameter_name,
                                                        std::meta::info type_info) -> AbiValueKind
{
    if (const auto override = overrideParameterAbiValueKind(function_name, parameter_name, type_info))
    {
        return *override;
    }

    return inferAbiValueKind(type_info, parameter_name);
}

[[nodiscard]] consteval auto overrideReturnAbiValueKind(std::string_view function_name)
    -> std::optional<AbiValueKind>
{
    if (function_name == "serialOpen")
    {
        return AbiValueKind::kOpaqueHandle;
    }

    return std::nullopt;
}

[[nodiscard]] consteval auto inferReturnAbiValueKind(std::string_view function_name, std::meta::info return_type_info)
    -> AbiValueKind
{
    if (std::meta::is_void_type(return_type_info))
    {
        return AbiValueKind::kVoid;
    }

    if (const auto override = overrideReturnAbiValueKind(function_name))
    {
        return *override;
    }

    if (std::meta::is_integral_type(return_type_info))
    {
        return std::meta::size_of(return_type_info) > sizeof(std::int32_t) ? AbiValueKind::kInt64 : AbiValueKind::kInt32;
    }

    return AbiValueKind::kOpaquePointer;
}

[[nodiscard]] consteval auto overrideParameterDirection(std::string_view function_name, std::string_view parameter_name)
    -> std::optional<ParameterDirection>
{
    if (function_name == "getVersion" && parameter_name == "out")
    {
        return ParameterDirection::kOut;
    }

    return std::nullopt;
}

[[nodiscard]] consteval auto inferParameterDirection(std::string_view function_name, std::string_view parameter_name,
                                                     std::meta::info type_info) -> ParameterDirection
{
    if (const auto override = overrideParameterDirection(function_name, parameter_name))
    {
        return *override;
    }

    if (parameter_name.ends_with("_callback") || parameter_name == "callback_fn" || parameter_name == "error_callback")
    {
        return ParameterDirection::kIn;
    }

    if (parameter_name == "out")
    {
        return ParameterDirection::kOut;
    }

    if (parameter_name == "buffer")
    {
        if (function_name.starts_with("serialRead"))
        {
            return ParameterDirection::kOut;
        }

        if (function_name.starts_with("serialWrite"))
        {
            return ParameterDirection::kIn;
        }
    }

    if (parameter_name == "sequence" || parameter_name == "until_char" || parameter_name == "port")
    {
        return ParameterDirection::kIn;
    }

    if (std::meta::is_pointer_type(type_info))
    {
        return isConstPointer(type_info) ? ParameterDirection::kIn : ParameterDirection::kInOut;
    }

    return ParameterDirection::kIn;
}

[[nodiscard]] consteval auto overrideResultModel(std::string_view function_name)
    -> std::optional<FunctionResultModel>
{
    if (function_name == "serialOpen")
    {
        return FunctionResultModel::kHandleOrStatus;
    }

    return std::nullopt;
}

[[nodiscard]] consteval auto inferResultModel(std::string_view function_name, std::meta::info return_type_info)
    -> FunctionResultModel
{
    if (std::meta::is_void_type(return_type_info))
    {
        return FunctionResultModel::kVoid;
    }

    if (const auto override = overrideResultModel(function_name))
    {
        return *override;
    }

    if (function_name.starts_with("serialRead") || function_name.starts_with("serialWrite")
        || function_name.starts_with("serialGet") || function_name.starts_with("serialInBytes")
        || function_name.starts_with("serialOutBytes") || function_name == "serialListPorts")
    {
        return FunctionResultModel::kValueOrStatus;
    }

    return FunctionResultModel::kStatusCode;
}

[[nodiscard]] consteval auto makeParameterDescriptor(std::string_view function_name, std::meta::info parameter_info)
    -> ParameterDescriptor
{
    const auto type_info = std::meta::type_of(parameter_info);
    const auto parameter_name = std::meta::identifier_of(parameter_info);

    return ParameterDescriptor{
        .name = parameter_name,
        .cpp_type = std::meta::display_string_of(type_info),
        .abi_kind = inferParameterAbiValueKind(function_name, parameter_name, type_info),
        .direction = inferParameterDirection(function_name, parameter_name, type_info),
        .optional = std::meta::has_default_argument(parameter_info),
    };
}

template <std::meta::info FunctionInfo, std::size_t... Index>
[[nodiscard]] consteval auto makeParameterDescriptorsForFunctionImpl(std::index_sequence<Index...>)
{
    constexpr auto params = std::define_static_array(std::meta::parameters_of(FunctionInfo));
    return std::array<ParameterDescriptor, sizeof...(Index)>{
        makeParameterDescriptor(std::meta::identifier_of(FunctionInfo), *(params.data() + Index))...,
    };
}

template <std::meta::info FunctionInfo>
inline constexpr auto kParameterDescriptorsForFunction =
    makeParameterDescriptorsForFunctionImpl<FunctionInfo>(
        std::make_index_sequence<std::meta::parameters_of(FunctionInfo).size()>{});

template <std::size_t ParamCount>
[[nodiscard]] consteval auto makeFunctionDescriptor(std::meta::info function_info,
                                                    const std::array<ParameterDescriptor, ParamCount> &params)
    -> FunctionDescriptor
{
    const auto function_name = std::meta::identifier_of(function_info);
    const auto return_type = std::meta::return_type_of(function_info);

    return FunctionDescriptor{
        .name = function_name,
        .return_cpp_type = std::meta::display_string_of(return_type),
        .signature = std::meta::display_string_of(std::meta::type_of(function_info)),
        .return_abi_kind = inferReturnAbiValueKind(function_name, return_type),
        .result_model = inferResultModel(function_name, return_type),
        .parameters = std::span<const ParameterDescriptor>(params),
    };
}

template <std::meta::info FunctionInfo> [[nodiscard]] consteval auto makeFunctionDescriptorFromInfo() -> FunctionDescriptor
{
    return makeFunctionDescriptor(FunctionInfo, kParameterDescriptorsForFunction<FunctionInfo>);
}

template <typename Struct, std::size_t... Index>
[[nodiscard]] consteval auto makeFieldDescriptorsImpl(const std::vector<std::meta::info> &fields,
                                                      std::index_sequence<Index...>)
{
    return std::array<StructFieldDescriptor, sizeof...(Index)>{
        StructFieldDescriptor{
            .name = std::meta::identifier_of(fields[Index]),
            .cpp_type = std::meta::display_string_of(std::meta::type_of(fields[Index])),
            .abi_kind = inferAbiValueKind(std::meta::type_of(fields[Index]), std::meta::identifier_of(fields[Index])),
            .offset = static_cast<std::size_t>(std::meta::offset_of(fields[Index]).bytes),
            .size = std::meta::size_of(std::meta::type_of(fields[Index])),
        }...,
    };
}

[[nodiscard]] consteval auto makeOperationDescriptor(std::meta::info function_info) -> OperationDescriptor
{
    const auto function_name = std::meta::identifier_of(function_info);
    return OperationDescriptor{
        .name = function_name,
        .function_name = function_name,
    };
}

template <std::meta::info FunctionInfo> [[nodiscard]] consteval auto makeOperationDescriptorFromInfo() -> OperationDescriptor
{
    return makeOperationDescriptor(FunctionInfo);
}

template <typename Code> [[nodiscard]] consteval auto makeStatusCodeDescriptor(const Code &code_value) -> StatusCodeDescriptor
{
    return StatusCodeDescriptor{
        .category = code_value.category(),
        .name = code_value.name(),
        .value = static_cast<StatusCodeValue>(code_value),
    };
}

template <std::size_t... Index>
[[nodiscard]] consteval auto makeAbiFunctionDescriptorsImpl(std::index_sequence<Index...>)
    -> std::array<FunctionDescriptor, sizeof...(Index)>
{
    constexpr AbiFunctionRegistry registry{};
    constexpr auto fields = std::define_static_array(
        std::meta::nonstatic_data_members_of(^^AbiFunctionRegistry, std::meta::access_context::unchecked()));

    return std::array<FunctionDescriptor, sizeof...(Index)>{
        makeFunctionDescriptorFromInfo<registry.[:*(fields.data() + Index):]>()...,
    };
}

template <std::size_t... Index>
[[nodiscard]] consteval auto makeAbiOperationDescriptorsImpl(std::index_sequence<Index...>)
    -> std::array<OperationDescriptor, sizeof...(Index)>
{
    constexpr AbiFunctionRegistry registry{};
    constexpr auto fields = std::define_static_array(
        std::meta::nonstatic_data_members_of(^^AbiFunctionRegistry, std::meta::access_context::unchecked()));

    return std::array<OperationDescriptor, sizeof...(Index)>{
        makeOperationDescriptorFromInfo<registry.[:*(fields.data() + Index):]>()...,
    };
}

} // namespace detail

inline constexpr auto kFunctionDescriptors = []() consteval
{
    return detail::makeAbiFunctionDescriptorsImpl(
        std::make_index_sequence<std::meta::nonstatic_data_members_of(^^detail::AbiFunctionRegistry,
                                                                       std::meta::access_context::unchecked())
                                     .size()>{});
}();

inline constexpr auto kOperationDescriptors = []() consteval
{
    return detail::makeAbiOperationDescriptorsImpl(
        std::make_index_sequence<std::meta::nonstatic_data_members_of(^^detail::AbiFunctionRegistry,
                                                                       std::meta::access_context::unchecked())
                                     .size()>{});
}();

#include "ffi/status_code_descriptors.hpp"

inline constexpr auto kSerialConfigFieldDescriptors = []() consteval
{
    auto fields = std::meta::nonstatic_data_members_of(^^SerialConfig, std::meta::access_context::unchecked());
    return detail::makeFieldDescriptorsImpl<SerialConfig>(
        fields, std::make_index_sequence<std::meta::nonstatic_data_members_of(^^SerialConfig,
                                                                              std::meta::access_context::unchecked())
                                             .size()>{});
}();

inline constexpr auto kVersionFieldDescriptors = []() consteval
{
    auto fields = std::meta::nonstatic_data_members_of(^^Version, std::meta::access_context::unchecked());
    return detail::makeFieldDescriptorsImpl<Version>(
        fields,
        std::make_index_sequence<std::meta::nonstatic_data_members_of(^^Version, std::meta::access_context::unchecked())
                                     .size()>{});
}();

[[nodiscard]] constexpr auto functionDescriptors() noexcept -> std::span<const FunctionDescriptor>
{
    return kFunctionDescriptors;
}

[[nodiscard]] constexpr auto operationDescriptors() noexcept -> std::span<const OperationDescriptor>
{
    return kOperationDescriptors;
}

[[nodiscard]] constexpr auto statusCodeDescriptors() noexcept -> std::span<const StatusCodeDescriptor>
{
    return kStatusCodeDescriptors;
}

[[nodiscard]] constexpr auto serialConfigFieldDescriptors() noexcept -> std::span<const StructFieldDescriptor>
{
    return kSerialConfigFieldDescriptors;
}

[[nodiscard]] constexpr auto versionFieldDescriptors() noexcept -> std::span<const StructFieldDescriptor>
{
    return kVersionFieldDescriptors;
}

[[nodiscard]] constexpr auto findFunctionDescriptor(std::string_view function_name) noexcept -> const FunctionDescriptor *
{
    for (const auto &descriptor : kFunctionDescriptors)
    {
        if (descriptor.name == function_name)
        {
            return &descriptor;
        }
    }

    return nullptr;
}

[[nodiscard]] constexpr auto findOperationDescriptor(std::string_view operation_name) noexcept -> const OperationDescriptor *
{
    for (const auto &descriptor : kOperationDescriptors)
    {
        if (descriptor.name == operation_name)
        {
            return &descriptor;
        }
    }

    return nullptr;
}

[[nodiscard]] constexpr auto findStatusCodeDescriptor(StatusCodeValue code) noexcept -> const StatusCodeDescriptor *
{
    for (const auto &descriptor : kStatusCodeDescriptors)
    {
        if (descriptor.value == code)
        {
            return &descriptor;
        }
    }

    return nullptr;
}

} // namespace cpp_core
