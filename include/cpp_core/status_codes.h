#pragma once

#include <concepts>
#include <cstdint>
#include <type_traits>

namespace cpp_core
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
    };

  public:
    static constexpr auto kCategoryCode = Category;
};

struct StatusCode
{
    static constexpr ValueType kSuccess = 0;

    struct Configuration : CategoryBase<1>
    {
        static constexpr ValueType kSetBaudrateError = GenCode<0>::kValue;
        static constexpr ValueType kSetDataBitsError = GenCode<1>::kValue;
        static constexpr ValueType kSetParityError = GenCode<2>::kValue;
        static constexpr ValueType kSetStopBitsError = GenCode<3>::kValue;
        static constexpr ValueType kSetFlowControlError = GenCode<4>::kValue;
        static constexpr ValueType kSetTimeoutError = GenCode<5>::kValue;
    };

    struct Connection : CategoryBase<2>
    {
        static constexpr ValueType kNotFoundError = GenCode<0>::kValue;
        static constexpr ValueType kInvalidHandleError = GenCode<1>::kValue;
        static constexpr ValueType kCloseHandleError = GenCode<2>::kValue;
    };

    struct Io : CategoryBase<3>
    {
        static constexpr ValueType kReadError = GenCode<0>::kValue;
        static constexpr ValueType kWriteError = GenCode<1>::kValue;
        static constexpr ValueType kAbortReadError = GenCode<2>::kValue;
        static constexpr ValueType kAbortWriteError = GenCode<3>::kValue;
        static constexpr ValueType kBufferError = GenCode<4>::kValue;
        static constexpr ValueType kClearBufferInError = GenCode<5>::kValue;
        static constexpr ValueType kClearBufferOutError = GenCode<6>::kValue;
    };

    struct Control : CategoryBase<4>
    {
        static constexpr ValueType kSetDtrError = GenCode<0>::kValue;
        static constexpr ValueType kSetRtsError = GenCode<1>::kValue;
        static constexpr ValueType kGetModemStatusError = GenCode<2>::kValue;
        static constexpr ValueType kSendBreakError = GenCode<3>::kValue;
        static constexpr ValueType kGetStateError = GenCode<4>::kValue;
        static constexpr ValueType kSetStateError = GenCode<5>::kValue;
    };

    struct Monitor : CategoryBase<5>
    {
        static constexpr ValueType kMonitorError = GenCode<0>::kValue;
    };
};

using StatusCodes = ValueType;
} // namespace cpp_core
