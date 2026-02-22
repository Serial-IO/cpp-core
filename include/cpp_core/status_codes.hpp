#pragma once

#include "status_codes.h"

#include <format>
#include <string_view>

namespace cpp_core
{

// constexpr toString

[[nodiscard]] constexpr auto toString(StatusCodes code) noexcept -> std::string_view
{
    switch (code)
    {
    case StatusCodes::kSuccess:
        return "Success";
    case StatusCodes::kCloseHandleError:
        return "CloseHandleError";
    case StatusCodes::kInvalidHandleError:
        return "InvalidHandleError";
    case StatusCodes::kReadError:
        return "ReadError";
    case StatusCodes::kWriteError:
        return "WriteError";
    case StatusCodes::kGetStateError:
        return "GetStateError";
    case StatusCodes::kSetStateError:
        return "SetStateError";
    case StatusCodes::kSetTimeoutError:
        return "SetTimeoutError";
    case StatusCodes::kBufferError:
        return "BufferError";
    case StatusCodes::kNotFoundError:
        return "NotFoundError";
    case StatusCodes::kClearBufferInError:
        return "ClearBufferInError";
    case StatusCodes::kClearBufferOutError:
        return "ClearBufferOutError";
    case StatusCodes::kAbortReadError:
        return "AbortReadError";
    case StatusCodes::kAbortWriteError:
        return "AbortWriteError";
    }
    return "Unknown";
}

[[nodiscard]] constexpr auto isError(StatusCodes code) noexcept -> bool
{
    return static_cast<int>(code) < 0;
}

[[nodiscard]] constexpr auto isSuccess(StatusCodes code) noexcept -> bool
{
    return code == StatusCodes::kSuccess;
}

/// Convert a raw int (as returned by C API functions) back to a StatusCodes.
/// Returns kSuccess for any non-negative value.
[[nodiscard]] constexpr auto fromInt(int value) noexcept -> StatusCodes
{
    if (value >= 0)
    {
        return StatusCodes::kSuccess;
    }
    return static_cast<StatusCodes>(value);
}

} // namespace cpp_core

// std::format support
// Enables: std::format("Port open failed: {}", StatusCodes::kNotFoundError)
//      =>  "Port open failed: NotFoundError (-9)"

template <> struct std::formatter<cpp_core::StatusCodes> : std::formatter<std::string_view>
{
    auto format(cpp_core::StatusCodes code, auto &ctx) const
    {
        auto name = cpp_core::toString(code);
        auto val = static_cast<int>(code);
        if (val == 0)
        {
            return std::formatter<std::string_view>::format(name, ctx);
        }
        return std::format_to(ctx.out(), "{} ({})", name, val);
    }
};
