#pragma once

#include "status_codes.h"

#include <concepts>
#include <expected>
#include <format>
#include <string>
#include <type_traits>
#include <utility>

namespace cpp_core
{

/// Enriched error type carrying a StatusCode and an optional message.
struct Error
{
    StatusCodes code;
    std::string message;

    constexpr explicit Error(StatusCodes code_in) noexcept : code(code_in)
    {
    }

    Error(StatusCodes code_in, std::string msg) : code(code_in), message(std::move(msg))
    {
    }

    [[nodiscard]] constexpr auto status() const noexcept -> int
    {
        return static_cast<int>(code);
    }

    [[nodiscard]] constexpr auto operator==(const Error &other) const noexcept -> bool
    {
        return code == other.code;
    }

    [[nodiscard]] constexpr auto operator==(StatusCodes code_in) const noexcept -> bool
    {
        return code == code_in;
    }
};

/// Monadic result type: either a value T or a cpp_core::Error.
/// Wraps std::expected with ergonomic helpers tuned for this library.
///
///   auto result = openPort()
///       .transform([](auto h) { return h.fd(); })
///       .or_else([](Error e) -> Result<int> { log(e); return std::unexpected(e); });
template <typename T> using Result = std::expected<T, Error>;

/// Convenience alias for void results (operations that only fail or succeed).
using Status = Result<void>;

// Factory helpers

template <typename T> [[nodiscard]] constexpr auto ok(T &&value) -> Result<std::remove_cvref_t<T>>
{
    return Result<std::remove_cvref_t<T>>{std::forward<T>(value)};
}

[[nodiscard]] constexpr auto ok() -> Status
{
    return {};
}

template <typename T = void> [[nodiscard]] constexpr auto fail(StatusCodes code) -> Result<T>
{
    return std::unexpected(Error{code});
}

template <typename T = void> [[nodiscard]] auto fail(StatusCodes code, std::string message) -> Result<T>
{
    return std::unexpected(Error{code, std::move(message)});
}

// Concept: anything that is a Result<U> for some U

// clang-format off
template <typename R>
concept IsResult = requires {
    typename R::value_type;
    typename R::error_type;
} && std::same_as<typename R::error_type, Error>;
// clang-format on

// TRY macro
// Usage:  auto value = CPP_CORE_TRY(someResultReturningCall());
// Propagates the error automatically if the result holds an error.

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#define CPP_CORE_TRY(expr)                                                                                             \
    ({                                                                                                                 \
        auto _cpp_core_result_ = (expr);                                                                               \
        if (!_cpp_core_result_.has_value())                                                                            \
            return std::unexpected(std::move(_cpp_core_result_).error());                                              \
        std::move(_cpp_core_result_).value();                                                                          \
    })

#define CPP_CORE_TRY_VOID(expr)                                                                                        \
    do                                                                                                                 \
    {                                                                                                                  \
        auto _cpp_core_result_ = (expr);                                                                               \
        if (!_cpp_core_result_.has_value())                                                                            \
            return std::unexpected(std::move(_cpp_core_result_).error());                                              \
    } while (false)
// NOLINTEND(cppcoreguidelines-macro-usage)

// Result → C return code bridge
// Converts a Result<int>/Status back into the C API convention (negative = error)
// and optionally invokes the legacy ErrorCallbackT.

template <typename T, typename Callback>
requires std::is_arithmetic_v<T>
constexpr auto toCResult(const Result<T> &result, Callback error_callback) -> T
{
    if (result.has_value())
    {
        return result.value();
    }
    const auto &err = result.error();
    if (error_callback != nullptr)
    {
        error_callback(err.status(), err.message.c_str());
    }
    return static_cast<T>(err.code);
}

inline auto toCStatus(const Status &result, auto error_callback) -> int
{
    if (result.has_value())
    {
        return 0;
    }
    const auto &err = result.error();
    if (error_callback != nullptr)
    {
        error_callback(err.status(), err.message.c_str());
    }
    return err.status();
}

} // namespace cpp_core
