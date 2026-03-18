#pragma once

#include <concepts>
#include <cstdint>
#include <type_traits>

namespace cpp_core::status_codes
{

namespace detail
{
using ValueType = std::int64_t;

template <ValueType Category> struct CategoryBase
{
  private:
    constexpr CategoryBase() = default;

  protected:
    static constexpr ValueType kCategoryMultiplier = 100;

    template <ValueType Code> struct GenCode
    {
        static constexpr ValueType kValue = -((Category * kCategoryMultiplier) + Code);
        constexpr explicit operator ValueType() const noexcept
        {
            return kValue;
        }
    };

  public:
    static constexpr auto kCategoryCode = Category;
};
} // namespace detail

struct StatusCode
{
    static constexpr detail::ValueType kSuccess = 0;

    struct Configuration : detail::CategoryBase<1>
    {
        static constexpr GenCode<0> kSetBaudrateError;
        static constexpr GenCode<1> kSetDataBitsError;
        static constexpr GenCode<2> kSetParityError;
        static constexpr GenCode<3> kSetStopBitsError;
        static constexpr GenCode<4> kSetFlowControlError;
        static constexpr GenCode<5> kSetTimeoutError;
    };

    struct Connection : detail::CategoryBase<2>
    {
        static constexpr GenCode<0> kNotFoundError;
        static constexpr GenCode<1> kInvalidHandleError;
        static constexpr GenCode<2> kCloseHandleError;
    };

    struct Io : detail::CategoryBase<3>
    {
        static constexpr GenCode<0> kReadError;
        static constexpr GenCode<1> kWriteError;
        static constexpr GenCode<2> kAbortReadError;
        static constexpr GenCode<3> kAbortWriteError;
        static constexpr GenCode<4> kBufferError;
        static constexpr GenCode<5> kClearBufferInError;
        static constexpr GenCode<6> kClearBufferOutError;
    };

    struct Control : detail::CategoryBase<4>
    {
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
