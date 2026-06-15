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
struct DataBitsTag
{
};
struct TimeoutMsTag
{
};
struct MultiplierTag
{
};

using Baudrate = StrongInt<BaudrateTag>;
using DataBits = StrongInt<DataBitsTag>;
using TimeoutMs = StrongInt<TimeoutMsTag>;
using Multiplier = StrongInt<MultiplierTag>;

// Parity & StopBits enums

enum class Parity : int
{
    kNone = 0,
    kEven = 1,
    kOdd = 2,
};

enum class StopBits : int
{
    kOne = 0,
    kTwo = 2,
};

enum class FlowControl : int
{
    kNone = 0,
    kRtsCts = 1,
    kXonXoff = 2,
};

template <typename Enum>
requires std::is_enum_v<Enum>
[[nodiscard]] constexpr auto toInt(Enum value) noexcept -> int
{
    return std::to_underlying(value);
}

} // namespace cpp_core
