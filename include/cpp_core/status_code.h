#pragma once

#include <cstdint>
#include <limits>
#include <string_view>

namespace cpp_core::status_codes
{

namespace detail
{

using ValueType = std::int64_t;

inline constexpr ValueType kCategoryMultiplier{100};

template <typename Category, ValueType NumericValue> struct Code
{
    static constexpr ValueType kValue = NumericValue;
    std::string_view kName; // NOLINT(readability-identifier-naming)

    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    constexpr operator ValueType() const noexcept
    {
        return kValue;
    }
    [[nodiscard]] constexpr auto value() const noexcept -> ValueType
    {
        return kValue;
    }
    [[nodiscard]] constexpr auto name() const noexcept -> std::string_view
    {
        return kName;
    }
    [[nodiscard]] constexpr auto category() const noexcept -> std::string_view
    {
        return Category::kCategoryName;
    }
};

template <typename Derived> struct CategoryBase
{
  private:
    constexpr CategoryBase() = default;

  protected:
    template <ValueType LocalCode> static consteval auto computeValue() -> ValueType
    {
        static_assert(Derived::kCategoryCode >= 0, "Category code must not be negative");
        static_assert(LocalCode >= 0, "Code index must not be negative");
        static_assert(LocalCode < kCategoryMultiplier, "Category overflow (max 99 codes)");
        static_assert(Derived::kCategoryCode <=
                          (std::numeric_limits<ValueType>::max() - kCategoryMultiplier + 1) / kCategoryMultiplier,
                      "Category code too large, multiplication would overflow");
        return -((Derived::kCategoryCode * kCategoryMultiplier) + LocalCode);
    }

    template <ValueType LocalCode> using Code = detail::Code<Derived, computeValue<LocalCode>()>;

    friend Derived;
};

} // namespace detail

struct StatusCode
{
    using ValueType = detail::ValueType;
    static constexpr ValueType kSuccess = 0;

    struct Configuration : detail::CategoryBase<Configuration>
    {
        static constexpr ValueType kCategoryCode = 1;
        static constexpr std::string_view kCategoryName{"Configuration"};

        static constexpr Code<0> kSetBaudrateError{"SetBaudrateError"};
        static constexpr Code<1> kSetDataBitsError{"SetDataBitsError"};
        static constexpr Code<2> kSetParityError{"SetParityError"};
        static constexpr Code<3> kSetStopBitsError{"SetStopBitsError"};
        static constexpr Code<4> kSetFlowControlError{"SetFlowControlError"};
        static constexpr Code<5> kSetTimeoutError{"SetTimeoutError"};
    };

    struct Connection : detail::CategoryBase<Connection>
    {
        static constexpr ValueType kCategoryCode = 2;
        static constexpr std::string_view kCategoryName{"Connection"};

        static constexpr Code<0> kNotFoundError{"NotFoundError"};
        static constexpr Code<1> kInvalidHandleError{"InvalidHandleError"};
        static constexpr Code<2> kCloseHandleError{"CloseHandleError"};
    };

    struct Io : detail::CategoryBase<Io>
    {
        static constexpr ValueType kCategoryCode = 3;
        static constexpr std::string_view kCategoryName{"Io"};

        static constexpr Code<0> kReadError{"ReadError"};
        static constexpr Code<1> kWriteError{"WriteError"};
        static constexpr Code<2> kAbortReadError{"AbortReadError"};
        static constexpr Code<3> kAbortWriteError{"AbortWriteError"};
        static constexpr Code<4> kBufferError{"BufferError"};
        static constexpr Code<5> kClearBufferInError{"ClearBufferInError"};
        static constexpr Code<6> kClearBufferOutError{"ClearBufferOutError"};
    };

    struct Control : detail::CategoryBase<Control>
    {
        static constexpr ValueType kCategoryCode = 4;
        static constexpr std::string_view kCategoryName{"Control"};

        static constexpr Code<0> kSetDtrError{"SetDtrError"};
        static constexpr Code<1> kSetRtsError{"SetRtsError"};
        static constexpr Code<2> kGetModemStatusError{"GetModemStatusError"};
        static constexpr Code<3> kSendBreakError{"SendBreakError"};
        static constexpr Code<4> kGetStateError{"GetStateError"};
        static constexpr Code<5> kSetStateError{"SetStateError"};
    };

    struct Monitor : detail::CategoryBase<Monitor>
    {
        static constexpr ValueType kCategoryCode = 5;
        static constexpr std::string_view kCategoryName{"Monitor"};

        static constexpr Code<0> kMonitorError{"MonitorError"};
    };

    [[nodiscard]] static constexpr auto isError(ValueType code) noexcept -> bool
    {
        return code < 0;
    }
    [[nodiscard]] static constexpr auto isSuccess(ValueType code) noexcept -> bool
    {
        return code >= 0;
    }
    template <typename Category> [[nodiscard]] static constexpr auto belongsTo(ValueType code) noexcept -> bool
    {
        return code < 0 && (-code) / detail::kCategoryMultiplier == Category::kCategoryCode;
    }
};

} // namespace cpp_core::status_codes

namespace cpp_core
{
using StatusCodeValue = ::cpp_core::status_codes::detail::ValueType;
using ::cpp_core::status_codes::StatusCode;

struct StatusCodes
{
    static constexpr StatusCodeValue kSuccess = StatusCode::kSuccess;

    static constexpr StatusCodeValue kSetBaudrateError = StatusCode::Configuration::kSetBaudrateError;
    static constexpr StatusCodeValue kSetDataBitsError = StatusCode::Configuration::kSetDataBitsError;
    static constexpr StatusCodeValue kSetParityError = StatusCode::Configuration::kSetParityError;
    static constexpr StatusCodeValue kSetStopBitsError = StatusCode::Configuration::kSetStopBitsError;
    static constexpr StatusCodeValue kSetFlowControlError = StatusCode::Configuration::kSetFlowControlError;
    static constexpr StatusCodeValue kSetTimeoutError = StatusCode::Configuration::kSetTimeoutError;

    static constexpr StatusCodeValue kNotFoundError = StatusCode::Connection::kNotFoundError;
    static constexpr StatusCodeValue kInvalidHandleError = StatusCode::Connection::kInvalidHandleError;
    static constexpr StatusCodeValue kCloseHandleError = StatusCode::Connection::kCloseHandleError;

    static constexpr StatusCodeValue kReadError = StatusCode::Io::kReadError;
    static constexpr StatusCodeValue kWriteError = StatusCode::Io::kWriteError;
    static constexpr StatusCodeValue kAbortReadError = StatusCode::Io::kAbortReadError;
    static constexpr StatusCodeValue kAbortWriteError = StatusCode::Io::kAbortWriteError;
    static constexpr StatusCodeValue kBufferError = StatusCode::Io::kBufferError;
    static constexpr StatusCodeValue kClearBufferInError = StatusCode::Io::kClearBufferInError;
    static constexpr StatusCodeValue kClearBufferOutError = StatusCode::Io::kClearBufferOutError;

    static constexpr StatusCodeValue kSetDtrError = StatusCode::Control::kSetDtrError;
    static constexpr StatusCodeValue kSetRtsError = StatusCode::Control::kSetRtsError;
    static constexpr StatusCodeValue kGetModemStatusError = StatusCode::Control::kGetModemStatusError;
    static constexpr StatusCodeValue kSendBreakError = StatusCode::Control::kSendBreakError;
    static constexpr StatusCodeValue kGetStateError = StatusCode::Control::kGetStateError;
    static constexpr StatusCodeValue kSetStateError = StatusCode::Control::kSetStateError;

    static constexpr StatusCodeValue kMonitorError = StatusCode::Monitor::kMonitorError;
};
} // namespace cpp_core
