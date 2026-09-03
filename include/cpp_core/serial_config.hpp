#pragma once

#include "result.hpp"
#include "strong_types.hpp"

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>

namespace cpp_core
{

// Compile-time validation

namespace detail
{

constexpr auto validateBaudrate(int baud) -> bool
{
    return baud >= 300;
}

constexpr auto validateDataBits(DataBits bits) -> bool
{
    return bits == DataBits::kFive || bits == DataBits::kSix || bits == DataBits::kSeven || bits == DataBits::kEight;
}

constexpr auto validateParity(Parity parity) -> bool
{
    return parity == Parity::kNone || parity == Parity::kEven || parity == Parity::kOdd;
}

constexpr auto validateStopBits(StopBits stop_bits) -> bool
{
    return stop_bits == StopBits::kOne || stop_bits == StopBits::kTwo;
}

constexpr auto validateFlowControl(FlowControl flow_mode) -> bool
{
    return flow_mode == FlowControl::kNone || flow_mode == FlowControl::kRtsCts || flow_mode == FlowControl::kXonXoff;
}

constexpr auto validateTimeout(int timeout_ms, int multiplier) -> bool
{
    return timeout_ms >= 0 && multiplier >= 0 &&
           (timeout_ms == 0 || multiplier <= std::numeric_limits<int>::max() / timeout_ms);
}

} // namespace detail

/**
 * Compile-time validated serial configuration.
 * Invalid configs are rejected at compile time - no runtime overhead.
 *   constexpr auto kCfg =
 *       SerialConfig::make<9600, DataBits::kEight, Parity::kNone, StopBits::kOne, FlowControl::kNone>();
 */
struct SerialConfig
{
    int baudrate;          ///< Baud rate in bit/s (>= 300).
    DataBits data_bits;    ///< Number of data bits.
    Parity parity;         ///< Parity mode.
    StopBits stop_bits;    ///< Stop-bit mode.
    FlowControl flow_mode; ///< None, hardware RTS/CTS, or software XON/XOFF flow control.

    template <int Baud, DataBits D, Parity P = Parity::kNone, StopBits S = StopBits::kOne,
              FlowControl F = FlowControl::kNone>
    static consteval auto make() -> SerialConfig
    {
        static_assert(detail::validateBaudrate(Baud), "Baudrate must be >= 300");
        static_assert(detail::validateDataBits(D), "DataBits must be 5-8");
        static_assert(detail::validateParity(P), "Parity must be none, even, or odd");
        static_assert(detail::validateStopBits(S), "StopBits must be one or two");
        static_assert(detail::validateFlowControl(F), "FlowControl must be none, RTS/CTS, or XON/XOFF");

        return SerialConfig{
            .baudrate = Baud,
            .data_bits = D,
            .parity = P,
            .stop_bits = S,
            .flow_mode = F,
        };
    }

    [[nodiscard]] static constexpr auto tryMake(Baudrate baud, DataBits data_bits, Parity parity = Parity::kNone,
                                                StopBits stop_bits = StopBits::kOne,
                                                FlowControl flow_mode = FlowControl::kNone) -> Result<SerialConfig>
    {
        return tryMake(baud.get(), data_bits, parity, stop_bits, flow_mode);
    }

    [[nodiscard]] static constexpr auto tryMake(int baud, DataBits data_bits, Parity parity = Parity::kNone,
                                                StopBits stop_bits = StopBits::kOne,
                                                FlowControl flow_mode = FlowControl::kNone) -> Result<SerialConfig>
    {
        if (!detail::validateBaudrate(baud))
        {
            return fail<SerialConfig>(StatusCode::Configuration::kSetBaudrateError);
        }
        if (!detail::validateDataBits(data_bits))
        {
            return fail<SerialConfig>(StatusCode::Configuration::kSetDataBitsError);
        }
        if (!detail::validateParity(parity))
        {
            return fail<SerialConfig>(StatusCode::Configuration::kSetParityError);
        }
        if (!detail::validateStopBits(stop_bits))
        {
            return fail<SerialConfig>(StatusCode::Configuration::kSetStopBitsError);
        }
        if (!detail::validateFlowControl(flow_mode))
        {
            return fail<SerialConfig>(StatusCode::Configuration::kSetFlowControlError);
        }
        return ok(SerialConfig{
            .baudrate = baud,
            .data_bits = data_bits,
            .parity = parity,
            .stop_bits = stop_bits,
            .flow_mode = flow_mode,
        });
    }

    [[nodiscard]] constexpr auto isValid() const noexcept -> bool
    {
        return detail::validateBaudrate(baudrate) && detail::validateDataBits(data_bits) &&
               detail::validateParity(parity) && detail::validateStopBits(stop_bits) &&
               detail::validateFlowControl(flow_mode);
    }

    [[nodiscard]] constexpr auto baudrateValue() const noexcept -> Baudrate
    {
        return Baudrate{baudrate};
    }

    [[nodiscard]] constexpr auto dataBitsValue() const noexcept -> DataBits
    {
        return data_bits;
    }

    [[nodiscard]] constexpr auto parityInt() const noexcept -> int
    {
        return toInt(parity);
    }

    [[nodiscard]] constexpr auto stopBitsInt() const noexcept -> int
    {
        return toInt(stop_bits);
    }

    [[nodiscard]] constexpr auto flowModeInt() const noexcept -> int
    {
        return toInt(flow_mode);
    }

    [[nodiscard]] constexpr auto withBaudrate(Baudrate baud) const -> Result<SerialConfig>
    {
        return tryMake(baud, dataBitsValue(), parity, stop_bits, flow_mode);
    }

    [[nodiscard]] constexpr auto withDataBits(DataBits bits) const -> Result<SerialConfig>
    {
        return tryMake(baudrateValue(), bits, parity, stop_bits, flow_mode);
    }

    [[nodiscard]] constexpr auto withParity(Parity new_parity) const -> Result<SerialConfig>
    {
        return tryMake(baudrateValue(), dataBitsValue(), new_parity, stop_bits, flow_mode);
    }

    [[nodiscard]] constexpr auto withStopBits(StopBits new_stop_bits) const -> Result<SerialConfig>
    {
        return tryMake(baudrateValue(), dataBitsValue(), parity, new_stop_bits, flow_mode);
    }

    [[nodiscard]] constexpr auto withFlowMode(FlowControl new_flow_mode) const -> Result<SerialConfig>
    {
        return tryMake(baudrateValue(), dataBitsValue(), parity, stop_bits, new_flow_mode);
    }

    [[nodiscard]] constexpr auto operator<=>(const SerialConfig &) const noexcept = default;
};

/**
 * Timeout configuration shared by serial read and write operations.
 * The base timeout applies to the first byte; subsequent bytes use
 * `timeout_ms * multiplier`.
 */
struct SerialTimeoutConfig
{
    int timeout_ms; ///< Base timeout per byte in milliseconds.
    int multiplier; ///< Factor applied to the timeout after the first byte.

    template <int TimeoutMsVal, int MultiplierVal> static consteval auto make() -> SerialTimeoutConfig
    {
        static_assert(detail::validateTimeout(TimeoutMsVal, MultiplierVal),
                      "Timeout and multiplier must be non-negative and their product must fit into int");
        return SerialTimeoutConfig{
            .timeout_ms = TimeoutMsVal,
            .multiplier = MultiplierVal,
        };
    }

    [[nodiscard]] static constexpr auto tryMake(TimeoutMs timeout, Multiplier timeout_multiplier)
        -> Result<SerialTimeoutConfig>
    {
        return tryMake(timeout.get(), timeout_multiplier.get());
    }

    [[nodiscard]] static constexpr auto tryMake(int timeout, int timeout_multiplier) -> Result<SerialTimeoutConfig>
    {
        if (!detail::validateTimeout(timeout, timeout_multiplier))
        {
            return fail<SerialTimeoutConfig>(StatusCode::Configuration::kSetTimeoutError);
        }
        return ok(SerialTimeoutConfig{
            .timeout_ms = timeout,
            .multiplier = timeout_multiplier,
        });
    }

    [[nodiscard]] constexpr auto isValid() const noexcept -> bool
    {
        return detail::validateTimeout(timeout_ms, multiplier);
    }

    [[nodiscard]] constexpr auto timeoutValue() const noexcept -> TimeoutMs
    {
        return TimeoutMs{timeout_ms};
    }

    [[nodiscard]] constexpr auto multiplierValue() const noexcept -> Multiplier
    {
        return Multiplier{multiplier};
    }

    [[nodiscard]] constexpr auto operator<=>(const SerialTimeoutConfig &) const noexcept = default;
};

// Concepts for serial port operations

// clang-format off

// A type that behaves like a native handle (int fd or HANDLE).
template <typename H>
concept NativeHandle = (std::is_integral_v<H> || std::is_pointer_v<H>)
    && !std::is_same_v<H, bool>;

// A type that can serve as a mutable byte buffer for read operations.
template <typename B>
concept ByteBuffer = requires(B buf) {
    { buf.data() } -> std::convertible_to<std::uint8_t *>;
    { buf.size() } -> std::convertible_to<std::size_t>;
};

// Read-only byte source for write operations.
template <typename B>
concept ConstByteBuffer = requires(const B buf) {
    { buf.data() } -> std::convertible_to<const std::uint8_t *>;
    { buf.size() } -> std::convertible_to<std::size_t>;
};

// clang-format on

} // namespace cpp_core
