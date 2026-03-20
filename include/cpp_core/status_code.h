#pragma once

#include <cstdint>
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
        static_assert(LocalCode >= 0, "Code index must not be negative");
        static_assert(LocalCode < kCategoryMultiplier, "Category overflow (max 99 codes)");
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
using ::cpp_core::status_codes::StatusCode;
} // namespace cpp_core

// Test cases
static_assert(cpp_core::StatusCode::kSuccess == 0);
static_assert(cpp_core::StatusCode::isSuccess(cpp_core::StatusCode::kSuccess));
static_assert(!cpp_core::StatusCode::isError(cpp_core::StatusCode::kSuccess));

static_assert(cpp_core::StatusCode::isError(cpp_core::StatusCode::Configuration::kSetBaudrateError));

static_assert(cpp_core::StatusCode::Configuration::kSetBaudrateError == -100);
static_assert(cpp_core::StatusCode::Configuration::kSetDataBitsError == -101);
static_assert(cpp_core::StatusCode::Configuration::kSetParityError == -102);
static_assert(cpp_core::StatusCode::Configuration::kSetStopBitsError == -103);
static_assert(cpp_core::StatusCode::Configuration::kSetFlowControlError == -104);
static_assert(cpp_core::StatusCode::Configuration::kSetTimeoutError == -105);

static_assert(cpp_core::StatusCode::Connection::kNotFoundError == -200);
static_assert(cpp_core::StatusCode::Connection::kInvalidHandleError == -201);
static_assert(cpp_core::StatusCode::Connection::kCloseHandleError == -202);

static_assert(cpp_core::StatusCode::Io::kReadError == -300);
static_assert(cpp_core::StatusCode::Io::kWriteError == -301);
static_assert(cpp_core::StatusCode::Io::kAbortReadError == -302);
static_assert(cpp_core::StatusCode::Io::kAbortWriteError == -303);
static_assert(cpp_core::StatusCode::Io::kBufferError == -304);
static_assert(cpp_core::StatusCode::Io::kClearBufferInError == -305);
static_assert(cpp_core::StatusCode::Io::kClearBufferOutError == -306);

static_assert(cpp_core::StatusCode::Control::kSetDtrError == -400);
static_assert(cpp_core::StatusCode::Control::kSetRtsError == -401);
static_assert(cpp_core::StatusCode::Control::kGetModemStatusError == -402);
static_assert(cpp_core::StatusCode::Control::kSendBreakError == -403);
static_assert(cpp_core::StatusCode::Control::kGetStateError == -404);
static_assert(cpp_core::StatusCode::Control::kSetStateError == -405);
