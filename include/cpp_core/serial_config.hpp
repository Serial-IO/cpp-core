#pragma once

#include <compare>
#include <concepts>
#include <type_traits>

namespace cpp_core
{

// Strong types for serial parameters

/// Phantom-tagged strong type. Prevents mixing up baudrate, timeout, etc.
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

// Compile-time validation

namespace detail
{

consteval auto validateBaudrate(int baud) -> bool
{
    return baud >= 300;
}

consteval auto validateDataBits(int bits) -> bool
{
    return bits >= 5 && bits <= 8;
}

} // namespace detail

/// Compile-time validated serial configuration.
/// Invalid configs are rejected at compile time — no runtime overhead.
///
///   constexpr auto kCfg = SerialConfig::make<9600, 8, Parity::kNone, StopBits::kOne>();
struct SerialConfig
{
    int baudrate;
    int data_bits;
    Parity parity;
    StopBits stop_bits;

    template <int Baud, int DataBitsVal, Parity P = Parity::kNone, StopBits S = StopBits::kOne>
    static consteval auto make() -> SerialConfig
    {
        static_assert(detail::validateBaudrate(Baud), "Baudrate must be >= 300");
        static_assert(detail::validateDataBits(DataBitsVal), "DataBits must be 5-8");

        return SerialConfig{
            .baudrate = Baud,
            .data_bits = DataBitsVal,
            .parity = P,
            .stop_bits = S,
        };
    }

    [[nodiscard]] static constexpr auto tryMake(int baud, int data_bits_val, Parity parity = Parity::kNone,
                                                StopBits stop_bits = StopBits::kOne) -> SerialConfig
    {
        return SerialConfig{
            .baudrate = baud,
            .data_bits = data_bits_val,
            .parity = parity,
            .stop_bits = stop_bits,
        };
    }

    [[nodiscard]] constexpr auto isValid() const noexcept -> bool
    {
        return baudrate >= 300 && data_bits >= 5 && data_bits <= 8;
    }

    [[nodiscard]] constexpr auto parityInt() const noexcept -> int
    {
        return static_cast<int>(parity);
    }

    [[nodiscard]] constexpr auto stopBitsInt() const noexcept -> int
    {
        return static_cast<int>(stop_bits);
    }

    [[nodiscard]] constexpr auto operator<=>(const SerialConfig &) const noexcept = default;
};

// Concepts for serial port operations

// clang-format off

/// A type that behaves like a native handle (int fd or HANDLE).
template <typename H>
concept NativeHandle = (std::is_integral_v<H> || std::is_pointer_v<H>)
    && !std::is_same_v<H, bool>;

/// A type that can serve as a mutable byte buffer for read operations.
template <typename B>
concept ByteBuffer = requires(B buf) {
    { buf.data() } -> std::convertible_to<void *>;
    { buf.size() } -> std::convertible_to<std::size_t>;
};

/// Read-only byte source for write operations.
template <typename B>
concept ConstByteBuffer = requires(const B buf) {
    { buf.data() } -> std::convertible_to<const void *>;
    { buf.size() } -> std::convertible_to<std::size_t>;
};

// clang-format on

// Common baud rate constants

namespace baud
{
inline constexpr int k300 = 300;
inline constexpr int k1200 = 1200;
inline constexpr int k2400 = 2400;
inline constexpr int k4800 = 4800;
inline constexpr int k9600 = 9600;
inline constexpr int k19200 = 19200;
inline constexpr int k38400 = 38400;
inline constexpr int k57600 = 57600;
inline constexpr int k115200 = 115200;
inline constexpr int k230400 = 230400;
inline constexpr int k460800 = 460800;
inline constexpr int k921600 = 921600;
} // namespace baud

} // namespace cpp_core
