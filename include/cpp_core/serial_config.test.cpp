#include "cpp_core/serial_config.hpp"
#include "cpp_core/reflection.hpp"

namespace cpp_core::tests::serial_config
{

constexpr auto kCompileTimeConfig =
    SerialConfig::make<115'200, 8, Parity::kEven, StopBits::kTwo, FlowControl::kRtsCts>();
static_assert(kCompileTimeConfig.isValid());
static_assert(kCompileTimeConfig.baudrateValue() == Baudrate{115'200});
static_assert(kCompileTimeConfig.dataBitsValue() == DataBits{8});
static_assert(kCompileTimeConfig.parityInt() == 1);
static_assert(kCompileTimeConfig.stopBitsInt() == 2);
static_assert(kCompileTimeConfig.flowModeInt() == 1);

constexpr auto kRuntimeLikeConfig =
    SerialConfig::tryMake(Baudrate{57'600}, DataBits{7}, Parity::kOdd, StopBits::kOne, FlowControl::kXonXoff);
static_assert(kRuntimeLikeConfig.has_value());
static_assert(kRuntimeLikeConfig->baudrateValue() == Baudrate{57'600});
static_assert(kRuntimeLikeConfig->dataBitsValue() == DataBits{7});
static_assert(kRuntimeLikeConfig->flow_mode == FlowControl::kXonXoff);

consteval auto rejectsBadBaudrate() -> bool
{
    return !SerialConfig::tryMake(Baudrate{299}, DataBits{8}).has_value();
}

consteval auto rejectsBadDataBits() -> bool
{
    return !SerialConfig::tryMake(9'600, 4).has_value();
}

consteval auto rejectsBadParity() -> bool
{
    return !SerialConfig::tryMake(9'600, 8, static_cast<Parity>(77)).has_value();
}

consteval auto rejectsBadFlowMode() -> bool
{
    return !SerialConfig::tryMake(9'600, 8, Parity::kNone, StopBits::kOne, static_cast<FlowControl>(77)).has_value();
}

static_assert(rejectsBadBaudrate());
static_assert(rejectsBadDataBits());
static_assert(rejectsBadParity());
static_assert(rejectsBadFlowMode());

constexpr auto kRetunedConfig = kCompileTimeConfig.withBaudrate(Baudrate{230'400});
static_assert(kRetunedConfig.has_value());
static_assert(kRetunedConfig->baudrateValue() == Baudrate{230'400});

constexpr auto kFlowModeConfig = kCompileTimeConfig.withFlowMode(FlowControl::kXonXoff);
static_assert(kFlowModeConfig.has_value());
static_assert(kFlowModeConfig->flow_mode == FlowControl::kXonXoff);

constexpr auto kTimeoutConfig = SerialTimeoutConfig::make<50, 2>();
static_assert(kTimeoutConfig.isValid());
static_assert(kTimeoutConfig.timeoutValue() == TimeoutMs{50});
static_assert(kTimeoutConfig.multiplierValue() == Multiplier{2});

constexpr auto kRuntimeTimeoutConfig = SerialTimeoutConfig::tryMake(TimeoutMs{100}, Multiplier{3});
static_assert(kRuntimeTimeoutConfig.has_value());
static_assert(kRuntimeTimeoutConfig->timeout_ms == 100);

static_assert(!SerialTimeoutConfig::tryMake(-1, 1).has_value());
static_assert(!SerialTimeoutConfig::tryMake(1'000'000'000, 3).has_value());

static_assert(cpp_core::reflection::publicFieldName<SerialConfig, 0>() == "baudrate");
static_assert(cpp_core::reflection::publicFieldName<SerialConfig, 1>() == "data_bits");
static_assert(cpp_core::reflection::publicFieldName<SerialConfig, 4>() == "flow_mode");
static_assert(cpp_core::reflection::publicFieldName<SerialTimeoutConfig, 0>() == "timeout_ms");

} // namespace cpp_core::tests::serial_config
