#include "cpp_core/status_code.h"

#include <cstdint>
#include <limits>

namespace cpp_core::status_codes::detail::tests
{

template <std::int64_t CatCode> struct FakeCategory : CategoryBase<FakeCategory<CatCode>>
{
    static constexpr ValueType kCategoryCode = CatCode;

    template <ValueType Local> static consteval auto call() -> ValueType
    {
        return CategoryBase<FakeCategory>::template computeValue<Local>();
    }
};

static_assert(cpp_core::StatusCode::kSuccess == 0);
static_assert(cpp_core::StatusCode::isSuccess(cpp_core::StatusCode::kSuccess));
static_assert(!cpp_core::StatusCode::isError(cpp_core::StatusCode::kSuccess));
static_assert(cpp_core::StatusCode::isError(cpp_core::StatusCode::Configuration::kSetBaudrateError));

static_assert(cpp_core::StatusCode::Configuration::kSetBaudrateError.category() == "Configuration");
static_assert(cpp_core::StatusCode::Configuration::kSetBaudrateError == -100);
static_assert(cpp_core::StatusCode::Configuration::kSetDataBitsError == -101);
static_assert(cpp_core::StatusCode::Configuration::kSetParityError == -102);
static_assert(cpp_core::StatusCode::Configuration::kSetStopBitsError == -103);
static_assert(cpp_core::StatusCode::Configuration::kSetFlowControlError == -104);
static_assert(cpp_core::StatusCode::Configuration::kSetTimeoutError == -105);

static_assert(cpp_core::StatusCode::Connection::kNotFoundError.category() == "Connection");
static_assert(cpp_core::StatusCode::Connection::kNotFoundError == -200);
static_assert(cpp_core::StatusCode::Connection::kInvalidHandleError == -201);
static_assert(cpp_core::StatusCode::Connection::kCloseHandleError == -202);

static_assert(cpp_core::StatusCode::Io::kReadError.category() == "Io");
static_assert(cpp_core::StatusCode::Io::kReadError == -300);
static_assert(cpp_core::StatusCode::Io::kWriteError == -301);
static_assert(cpp_core::StatusCode::Io::kAbortReadError == -302);
static_assert(cpp_core::StatusCode::Io::kAbortWriteError == -303);
static_assert(cpp_core::StatusCode::Io::kBufferError == -304);
static_assert(cpp_core::StatusCode::Io::kClearBufferInError == -305);
static_assert(cpp_core::StatusCode::Io::kClearBufferOutError == -306);

static_assert(cpp_core::StatusCode::Control::kSetDtrError.category() == "Control");
static_assert(cpp_core::StatusCode::Control::kSetDtrError == -400);
static_assert(cpp_core::StatusCode::Control::kSetRtsError == -401);
static_assert(cpp_core::StatusCode::Control::kGetModemStatusError == -402);
static_assert(cpp_core::StatusCode::Control::kSendBreakError == -403);
static_assert(cpp_core::StatusCode::Control::kGetStateError == -404);
static_assert(cpp_core::StatusCode::Control::kSetStateError == -405);

static_assert(cpp_core::StatusCode::Monitor::kMonitorError.category() == "Monitor");
static_assert(cpp_core::StatusCode::Monitor::kMonitorError == -500);

static_assert(cpp_core::StatusCode::belongsTo<cpp_core::StatusCode::Configuration>(
    cpp_core::StatusCode::Configuration::kSetBaudrateError));
static_assert(!cpp_core::StatusCode::belongsTo<cpp_core::StatusCode::Io>(
    cpp_core::StatusCode::Configuration::kSetBaudrateError));

// Formula: result == -(kCategoryCode * 100 + LocalCode)
static_assert(FakeCategory<1>::call<0>() == -100);
static_assert(FakeCategory<1>::call<42>() == -142);
static_assert(FakeCategory<3>::call<7>() == -307);

// Edge: kCategoryCode == 0 -> call<0>() produces 0 (not negative)
static_assert(FakeCategory<0>::call<0>() == 0);
static_assert(FakeCategory<0>::call<1>() == -1);

// Edge: LocalCode == 99 (max before overflow guard)
static_assert(FakeCategory<2>::call<99>() == -299);

// Consecutive codes differ by exactly -1
static_assert(FakeCategory<1>::call<1>() - FakeCategory<1>::call<0>() == -1);

// Adjacent category ranges don't overlap (last of cat N > first of cat N+1)
static_assert(FakeCategory<1>::call<99>() > FakeCategory<2>::call<0>());

// Overflow: largest safe category still produces correct results
inline constexpr ValueType kMaxSafeCat =
    (std::numeric_limits<ValueType>::max() - kCategoryMultiplier + 1) / kCategoryMultiplier;
static_assert(kMaxSafeCat > 1'000'000);
static_assert(FakeCategory<kMaxSafeCat>::call<0>() == -(kMaxSafeCat * kCategoryMultiplier));
static_assert(FakeCategory<kMaxSafeCat>::call<99>() > std::numeric_limits<ValueType>::min());

} // namespace cpp_core::status_codes::detail::tests
