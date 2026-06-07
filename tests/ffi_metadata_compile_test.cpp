#include "cpp_core/ffi_metadata.hpp"

static_assert(__cpp_impl_reflection >= 202603L);

static_assert(!cpp_core::kFunctionDescriptors.empty());
static_assert(cpp_core::kFunctionDescriptors.size() == cpp_core::kOperationDescriptors.size());
static_assert(!cpp_core::kStatusCodeDescriptors.empty());
static_assert(cpp_core::kSerialConfigFieldDescriptors.size() == 4);
static_assert(cpp_core::kVersionFieldDescriptors.size() == 8);

constexpr auto kGetVersion = cpp_core::detail::kDescriptor_getVersion;
static_assert(kGetVersion.result_model == cpp_core::FunctionResultModel::kVoid);
static_assert(kGetVersion.parameters.size() == 1);
static_assert(kGetVersion.parameters[0].name == "out");
static_assert(kGetVersion.parameters[0].abi_kind == cpp_core::AbiValueKind::kVersionStructPointer);
static_assert(kGetVersion.parameters[0].direction == cpp_core::ParameterDirection::kOut);

constexpr auto kSerialOpen = cpp_core::detail::kDescriptor_serialOpen;
static_assert(kSerialOpen.result_model == cpp_core::FunctionResultModel::kHandleOrStatus);
static_assert(kSerialOpen.return_abi_kind == cpp_core::AbiValueKind::kOpaqueHandle);
static_assert(kSerialOpen.parameters.size() == 6);
static_assert(kSerialOpen.parameters[0].name == "port");
static_assert(kSerialOpen.parameters[0].abi_kind == cpp_core::AbiValueKind::kUtf8CString);
static_assert(kSerialOpen.parameters[4].optional);
static_assert(kSerialOpen.parameters[5].name == "error_callback");
static_assert(kSerialOpen.parameters[5].abi_kind == cpp_core::AbiValueKind::kErrorCallback);

constexpr auto kSerialRead = cpp_core::detail::kDescriptor_serialRead;
static_assert(kSerialRead.result_model == cpp_core::FunctionResultModel::kValueOrStatus);
static_assert(kSerialRead.parameters[1].name == "buffer");
static_assert(kSerialRead.parameters[1].abi_kind == cpp_core::AbiValueKind::kMutableBuffer);
static_assert(kSerialRead.parameters[1].direction == cpp_core::ParameterDirection::kOut);

constexpr auto kSetErrorCallback = cpp_core::detail::kDescriptor_serialSetErrorCallback;
static_assert(kSetErrorCallback.result_model == cpp_core::FunctionResultModel::kVoid);
static_assert(kSetErrorCallback.parameters.size() == 1);
static_assert(kSetErrorCallback.parameters[0].abi_kind == cpp_core::AbiValueKind::kErrorCallback);

static_assert(cpp_core::kStatusCodeDescriptors[10].category == "Io");
static_assert(cpp_core::kStatusCodeDescriptors[10].name == "ReadError");
static_assert(cpp_core::kStatusCodeDescriptors[23].category == "Monitor");
static_assert(cpp_core::kStatusCodeDescriptors[23].name == "MonitorError");

static_assert(cpp_core::kSerialConfigFieldDescriptors[0].name == "baudrate");
static_assert(cpp_core::kSerialConfigFieldDescriptors[1].name == "data_bits");
static_assert(cpp_core::kSerialConfigFieldDescriptors[2].name == "parity");
static_assert(cpp_core::kSerialConfigFieldDescriptors[3].name == "stop_bits");

static_assert(cpp_core::kVersionFieldDescriptors[0].name == "major");
static_assert(cpp_core::kVersionFieldDescriptors[3].name == "commit_hash_short");
static_assert(cpp_core::kVersionFieldDescriptors[7].name == "version_string");

static_assert(cpp_core::findFunctionDescriptor("serialOpen") != nullptr);
static_assert(cpp_core::findFunctionDescriptor("serialOpen")->name == "serialOpen");
static_assert(cpp_core::findFunctionDescriptor("doesNotExist") == nullptr);
static_assert(cpp_core::findOperationDescriptor("serialOpen") != nullptr);
static_assert(cpp_core::findOperationDescriptor("serialOpen")->function_name == "serialOpen");
static_assert(cpp_core::findStatusCodeDescriptor(cpp_core::StatusCodes::kMonitorError) != nullptr);
