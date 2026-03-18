#pragma once

#include <concepts>
#include <cstdint>
#include <type_traits>

namespace cpp_core::status_codes
{

namespace detail
{
using ValueType = std::int64_t;

template <typename Derived> struct CategoryBase
{
  private:
    constexpr CategoryBase() = default;

    static constexpr ValueType kCategoryMultiplier{100};

    template <ValueType Code> struct GenCode
    {
        static constexpr ValueType kValue = -((Derived::kCategoryCode * kCategoryMultiplier) + Code);
        constexpr explicit operator ValueType() const noexcept
        {
            return kValue;
        }
    };
    friend Derived;
};
} // namespace detail

struct StatusCode
{
    static constexpr detail::ValueType kSuccess = 0;

    struct Configuration : detail::CategoryBase<Configuration>
    {
        static constexpr auto kCategoryCode{1};

        static constexpr GenCode<0> kSetBaudrateError;
        static constexpr GenCode<1> kSetDataBitsError;
        static constexpr GenCode<2> kSetParityError;
        static constexpr GenCode<3> kSetStopBitsError;
        static constexpr GenCode<4> kSetFlowControlError;
        static constexpr GenCode<5> kSetTimeoutError;
    };

    struct Connection : detail::CategoryBase<Connection>
    {
        static constexpr auto kCategoryCode{2};

        static constexpr GenCode<0> kNotFoundError;
        static constexpr GenCode<1> kInvalidHandleError;
        static constexpr GenCode<2> kCloseHandleError;
    };

    struct Io : detail::CategoryBase<Io>
    {
        static constexpr auto kCategoryCode{3};

        static constexpr GenCode<0> kReadError;
        static constexpr GenCode<1> kWriteError;
        static constexpr GenCode<2> kAbortReadError;
        static constexpr GenCode<3> kAbortWriteError;
        static constexpr GenCode<4> kBufferError;
        static constexpr GenCode<5> kClearBufferInError;
        static constexpr GenCode<6> kClearBufferOutError;
    };

    struct Control : detail::CategoryBase<Control>
    {
        static constexpr auto kCategoryCode{4};

        static constexpr GenCode<0> kSetDtrError;
        static constexpr GenCode<1> kSetRtsError;
        static constexpr GenCode<2> kGetModemStatusError;
        static constexpr GenCode<3> kSendBreakError;
        static constexpr GenCode<4> kGetStateError;
        static constexpr GenCode<5> kSetStateError;
    };
};

} // namespace cpp_core::status_codes

namespace cpp_core
{
using StatusCodes = ::cpp_core::status_codes::detail::ValueType;
using StatusCode = ::cpp_core::status_codes::StatusCode;
} // namespace cpp_core
