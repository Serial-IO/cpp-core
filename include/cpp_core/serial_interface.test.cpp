#include "cpp_core/serial.h"
#include "cpp_core/serial_config.hpp"
#include "cpp_core/validation.hpp"

#include <cstdint>
#include <type_traits>

namespace cpp_core::tests::serial_interface
{

using OpenFn = intptr_t (*)(void *, const SerialConfig *, ErrorCallbackT);
using ReadFn = int (*)(int64_t, void *, int, const SerialTimeoutConfig *, ErrorCallbackT);
using ReadUntilFn = int (*)(int64_t, void *, int, const SerialTimeoutConfig *, void *, ErrorCallbackT);
using WriteFn = int (*)(int64_t, const void *, int, const SerialTimeoutConfig *, ErrorCallbackT);

static_assert(std::is_same_v<decltype(&::serialOpen), OpenFn>);
static_assert(std::is_same_v<decltype(&::serialRead), ReadFn>);
static_assert(std::is_same_v<decltype(&::serialReadLine), ReadFn>);
static_assert(std::is_same_v<decltype(&::serialReadUntil), ReadUntilFn>);
static_assert(std::is_same_v<decltype(&::serialReadUntilSequence), ReadUntilFn>);
static_assert(std::is_same_v<decltype(&::serialWrite), WriteFn>);

static_assert(std::is_standard_layout_v<SerialConfig>);
static_assert(std::is_trivially_copyable_v<SerialConfig>);
static_assert(sizeof(SerialConfig) == 5 * sizeof(int));
static_assert(std::is_standard_layout_v<SerialTimeoutConfig>);
static_assert(std::is_trivially_copyable_v<SerialTimeoutConfig>);
static_assert(sizeof(SerialTimeoutConfig) == 2 * sizeof(int));

inline int portMarker;
constexpr auto kSerialConfig = SerialConfig::make<115'200, 8>();
constexpr auto kTimeoutConfig = SerialTimeoutConfig::make<50, 1>();

static_assert(validateOpenParams<int>(&portMarker, &kSerialConfig, nullptr) == StatusCode::kSuccess);
static_assert(validateOpenParams<int>(&portMarker, nullptr, nullptr) == StatusCode::Control::kSetStateError);
static_assert(validateTimeoutConfig<int>(&kTimeoutConfig, nullptr) == StatusCode::kSuccess);
static_assert(validateTimeoutConfig<int>(nullptr, nullptr) == StatusCode::Configuration::kSetTimeoutError);

} // namespace cpp_core::tests::serial_interface
