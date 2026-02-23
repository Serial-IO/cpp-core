#pragma once

#include "strong_types.hpp"

#include <concepts>
#include <type_traits>

namespace cpp_core
{

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

// Compile-time validated serial configuration.
// Invalid configs are rejected at compile time - no runtime overhead.
//
//   constexpr auto kCfg = SerialConfig::make<9600, 8, Parity::kNone, StopBits::kOne>();
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

// A type that behaves like a native handle (int fd or HANDLE).
template <typename H>
concept NativeHandle = (std::is_integral_v<H> || std::is_pointer_v<H>)
    && !std::is_same_v<H, bool>;

// A type that can serve as a mutable byte buffer for read operations.
template <typename B>
concept ByteBuffer = requires(B buf) {
    { buf.data() } -> std::convertible_to<void *>;
    { buf.size() } -> std::convertible_to<std::size_t>;
};

// Read-only byte source for write operations.
template <typename B>
concept ConstByteBuffer = requires(const B buf) {
    { buf.data() } -> std::convertible_to<const void *>;
    { buf.size() } -> std::convertible_to<std::size_t>;
};

// clang-format on

} // namespace cpp_core
