#pragma once

#include <compare>
#include <concepts>
#include <utility>

namespace cpp_core
{

// Phantom-tagged strong type. Prevents mixing up baudrate, timeout, etc.
template <typename Tag, std::integral Underlying = int> struct StrongInt
{
    using TagType = Tag;
    using ValueType = Underlying;

    Underlying value{};

    constexpr StrongInt() noexcept = default;

    constexpr explicit StrongInt(Underlying val) noexcept : value(val)
    {
    }

    [[nodiscard]] constexpr auto get() const noexcept -> Underlying
    {
        return value;
    }

    constexpr auto operator+=(StrongInt other) noexcept -> StrongInt &
    {
        value += other.value;
        return *this;
    }

    constexpr auto operator-=(StrongInt other) noexcept -> StrongInt &
    {
        value -= other.value;
        return *this;
    }

    [[nodiscard]] friend constexpr auto operator+(StrongInt lhs, StrongInt rhs) noexcept -> StrongInt
    {
        lhs += rhs;
        return lhs;
    }

    [[nodiscard]] friend constexpr auto operator-(StrongInt lhs, StrongInt rhs) noexcept -> StrongInt
    {
        lhs -= rhs;
        return lhs;
    }

    [[nodiscard]] constexpr auto operator<=>(const StrongInt &) const noexcept = default;

    [[nodiscard]] constexpr explicit operator Underlying() const noexcept
    {
        return value;
    }
};

struct BaudrateTag
{
};
struct TimeoutMsTag
{
};
struct MultiplierTag
{
};

using Baudrate = StrongInt<BaudrateTag>;
using TimeoutMs = StrongInt<TimeoutMsTag>;
using Multiplier = StrongInt<MultiplierTag>;

/**
 * Number of data bits contained in each serial frame.
 */
enum class DataBits : int
{
    kFive = 5,  ///< Use five data bits.
    kSix = 6,   ///< Use six data bits.
    kSeven = 7, ///< Use seven data bits.
    kEight = 8, ///< Use eight data bits.
};

/**
 * Parity mode used for serial communication.
 */
enum class Parity : int
{
    kNone = 0, ///< Disable parity checking and generation.
    kEven = 1, ///< Use even parity.
    kOdd = 2,  ///< Use odd parity.
};

/**
 * Number of stop bits appended to each serial frame.
 */
enum class StopBits : int
{
    kOne = 0, ///< Use one stop bit.
    kTwo = 2, ///< Use two stop bits.
};

/**
 * Flow-control mode used by the serial port.
 */
enum class FlowControl : int
{
    kNone = 0,    ///< Disable flow control.
    kRtsCts = 1,  ///< Use hardware RTS/CTS flow control.
    kXonXoff = 2, ///< Use software XON/XOFF flow control.
};

template <typename Enum>
requires std::is_enum_v<Enum>
[[nodiscard]] constexpr auto toInt(Enum value) noexcept -> int
{
    return std::to_underlying(value);
}

} // namespace cpp_core
