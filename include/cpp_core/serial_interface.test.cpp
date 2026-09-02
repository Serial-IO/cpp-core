#include "cpp_core/serial.h"
#include "cpp_core/serial_config.hpp"
#include "cpp_core/validation.hpp"

#include <array>
#include <cstdint>
#include <type_traits>

namespace cpp_core::tests::serial_interface
{

using OpenFn = intptr_t (*)(const char *, const SerialConfig *, ErrorCallbackT);
using ReadFn = int (*)(int64_t, std::uint8_t *, int, const SerialTimeoutConfig *, ErrorCallbackT);
using ReadUntilFn = int (*)(int64_t, std::uint8_t *, int, const SerialTimeoutConfig *, std::uint8_t, ErrorCallbackT);
using ReadUntilSequenceFn = int (*)(int64_t, std::uint8_t *, int, const SerialTimeoutConfig *, const std::uint8_t *,
                                    int, ErrorCallbackT);
using WriteFn = int (*)(int64_t, const std::uint8_t *, int, const SerialTimeoutConfig *, ErrorCallbackT);

static_assert(std::is_same_v<decltype(&::serialOpen), OpenFn>);
static_assert(std::is_same_v<decltype(&::serialRead), ReadFn>);
static_assert(std::is_same_v<decltype(&::serialReadLine), ReadFn>);
static_assert(std::is_same_v<decltype(&::serialReadUntil), ReadUntilFn>);
static_assert(std::is_same_v<decltype(&::serialReadUntilSequence), ReadUntilSequenceFn>);
static_assert(std::is_same_v<decltype(&::serialWrite), WriteFn>);

static_assert(std::is_standard_layout_v<SerialConfig>);
static_assert(std::is_trivially_copyable_v<SerialConfig>);
static_assert(sizeof(SerialConfig) == 5 * sizeof(int));
static_assert(std::is_standard_layout_v<SerialTimeoutConfig>);
static_assert(std::is_trivially_copyable_v<SerialTimeoutConfig>);
static_assert(sizeof(SerialTimeoutConfig) == 2 * sizeof(int));
static_assert(ByteBuffer<std::array<std::uint8_t, 8>>);
static_assert(ConstByteBuffer<std::array<std::uint8_t, 8>>);
static_assert(!ByteBuffer<std::array<char, 8>>);
static_assert(!ConstByteBuffer<std::array<char, 8>>);

inline constexpr char kPort[] = "/dev/ttyUSB0";
constexpr auto kSerialConfig = SerialConfig::make<115'200, 8>();
constexpr auto kTimeoutConfig = SerialTimeoutConfig::make<50, 1>();
inline constexpr std::uint8_t kByte{};

static_assert(validateOpenParams<int>(kPort, &kSerialConfig, nullptr) == StatusCode::kSuccess);
static_assert(validateOpenParams<int>(kPort, nullptr, nullptr) == StatusCode::Control::kSetStateError);
static_assert(validateTimeoutConfig<int>(&kTimeoutConfig, nullptr) == StatusCode::kSuccess);
static_assert(validateTimeoutConfig<int>(nullptr, nullptr) == StatusCode::Configuration::kSetTimeoutError);
static_assert(validateBuffer<int>(&kByte, 1, nullptr) == StatusCode::kSuccess);

} // namespace cpp_core::tests::serial_interface
