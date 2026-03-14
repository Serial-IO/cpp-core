#pragma once

#include <compare>
#include <concepts>

namespace cpp_core
{

// Phantom-tagged strong type. Prevents mixing up baudrate, timeout, etc.
template <typename Tag, std::integral Underlying = int> struct StrongInt
{
    using TagType = Tag;
    using ValueType = Underlying;

    Underlying value;

    constexpr explicit StrongInt(Underlying val) noexcept : value(val)
    {
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

} // namespace cpp_core
