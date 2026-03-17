#pragma once

#include <concepts>
#include <cstdint>
#include <type_traits>

namespace cpp_core
{

using ValueType = std::int32_t;

template <ValueType Category> struct CategoryBase
{
  private:
    constexpr CategoryBase() = default;

  public:
    static constexpr auto make(ValueType detail) -> ValueType
    {
        return -((Category * 100) + detail);
    }
};

struct StatusCode
{
    static constexpr ValueType kSuccess = 0;

    struct Configuration : CategoryBase<1>
    {
        static constexpr ValueType kSetBaudrateError = make(0);
        static constexpr ValueType kSetDataBitsError = make(1);
        static constexpr ValueType kSetParityError = make(2);
        static constexpr ValueType kSetStopBitsError = make(3);
        static constexpr ValueType kSetFlowControlError = make(4);
        static constexpr ValueType kSetTimeoutError = make(5);
    };

    struct Connection : CategoryBase<2>
    {
        static constexpr ValueType kNotFoundError = make(0);
        static constexpr ValueType kInvalidHandleError = make(1);
        static constexpr ValueType kCloseHandleError = make(2);
    };

    struct Io : CategoryBase<3>
    {
        static constexpr ValueType kReadError = make(0);
        static constexpr ValueType kWriteError = make(1);
        static constexpr ValueType kAbortReadError = make(2);
        static constexpr ValueType kAbortWriteError = make(3);
        static constexpr ValueType kBufferError = make(4);
        static constexpr ValueType kClearBufferInError = make(5);
        static constexpr ValueType kClearBufferOutError = make(6);
    };

    struct Control : CategoryBase<4>
    {
        static constexpr ValueType kSetDtrError = make(0);
        static constexpr ValueType kSetRtsError = make(1);
        static constexpr ValueType kGetModemStatusError = make(2);
        static constexpr ValueType kSendBreakError = make(3);
        static constexpr ValueType kGetStateError = make(4);
        static constexpr ValueType kSetStateError = make(5);
    };

    struct Monitor : CategoryBase<5>
    {
        static constexpr ValueType kMonitorError = make(0);
    };
};

using StatusCodes = ValueType;
} // namespace cpp_core
