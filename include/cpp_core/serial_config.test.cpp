#include "cpp_core/reflection.hpp"
#include "cpp_core/serial_config.hpp"

namespace cpp_core::tests::serial_config
{

constexpr auto kCompileTimeConfig = SerialConfig::make<115'200, 8, Parity::kEven, StopBits::kTwo>();
static_assert(kCompileTimeConfig.isValid());
static_assert(kCompileTimeConfig.baudrateValue() == Baudrate{115'200});
static_assert(kCompileTimeConfig.dataBitsValue() == DataBits{8});
static_assert(kCompileTimeConfig.parityInt() == 1);
static_assert(kCompileTimeConfig.stopBitsInt() == 2);

constexpr auto kRuntimeLikeConfig = SerialConfig::tryMake(Baudrate{57'600}, DataBits{7}, Parity::kOdd, StopBits::kOne);
static_assert(kRuntimeLikeConfig.has_value());
static_assert(kRuntimeLikeConfig->baudrateValue() == Baudrate{57'600});
static_assert(kRuntimeLikeConfig->dataBitsValue() == DataBits{7});

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

static_assert(rejectsBadBaudrate());
static_assert(rejectsBadDataBits());
static_assert(rejectsBadParity());

constexpr auto kRetunedConfig = kCompileTimeConfig.withBaudrate(Baudrate{230'400});
static_assert(kRetunedConfig.has_value());
static_assert(kRetunedConfig->baudrateValue() == Baudrate{230'400});

static_assert(cpp_core::reflection::publicFieldName<SerialConfig, 0>() == "baudrate");
static_assert(cpp_core::reflection::publicFieldName<SerialConfig, 1>() == "data_bits");

} // namespace cpp_core::tests::serial_config
