#pragma once

#include "status_codes.h"

#include <concepts>
#include <string>
#include <string_view>
#include <type_traits>

namespace cpp_core
{

// Concepts

// Matches any callable that can receive (int, const char*).
// clang-format off
template <typename F>
concept ErrorCallback = std::is_null_pointer_v<std::remove_cvref_t<F>>
    || std::is_same_v<std::remove_cvref_t<F>, std::nullptr_t>
    || requires(F &&func, int code, const char *msg) {
        { func(code, msg) } -> std::same_as<void>;
    };
// clang-format on

// Matches anything implicitly convertible to a C-style function pointer (ErrorCallbackT).
template <typename F>
concept LegacyErrorCallback = std::is_convertible_v<F, void (*)(int, const char *)>;

// Matches any type whose value can be returned as a status/result code.
template <typename T>
concept StatusConvertible = std::is_arithmetic_v<T> && requires(StatusCodes code) { static_cast<T>(code); };

// Error invocation

// Safely invoke an error callback, does nothing if callback is nullptr.
template <ErrorCallback Callback>
constexpr auto invokeError(Callback &&callback, StatusCodes code, std::string_view message) noexcept -> void
{
    if constexpr (std::is_null_pointer_v<std::remove_cvref_t<Callback>>)
    {
        (void)callback;
        (void)code;
        (void)message;
        return;
    }
    else
    {
        if (callback != nullptr)
        {
            const std::string term(message);
            callback(static_cast<int>(code), term.c_str());
        }
    }
}

// Fail helpers (concept-constrained)

// Report failure through callback and return the status code cast to Ret.
template <StatusConvertible Ret, ErrorCallback Callback>
constexpr auto failMsg(Callback &&callback, StatusCodes code, std::string_view message) -> Ret
{
    invokeError(std::forward<Callback>(callback), code, message);
    return static_cast<Ret>(code);
}

// Overload that builds the message by concatenating a prefix and a detail string.
template <StatusConvertible Ret, ErrorCallback Callback>
auto failMsg(Callback &&callback, StatusCodes code, std::string_view prefix, std::string_view detail) -> Ret
{
    std::string full;
    full.reserve(prefix.size() + 2 + detail.size());
    full.append(prefix);
    full.append(": ");
    full.append(detail);
    invokeError(std::forward<Callback>(callback), code, full);
    return static_cast<Ret>(code);
}

// Pipe-style error chaining

/**
 * Chain operations that return a status-code integer.
 * Stops at the first non-success result and returns it.
 *   auto result = chainStatus(
 *       [&] { return configureBaudrate(h, 9600); },
 *       [&] { return configureParity(h, 0); },
 *       [&] { return configureStopBits(h, 1); }
 *   );
 */
template <std::invocable... Fns>
requires(std::is_convertible_v<std::invoke_result_t<Fns>, int> && ...)
constexpr auto chainStatus(Fns &&...fns) -> int
{
    int result = 0;
    ((static_cast<void>(result = static_cast<int>(std::forward<Fns>(fns)())), result < 0) || ...);
    return result;
}

} // namespace cpp_core
