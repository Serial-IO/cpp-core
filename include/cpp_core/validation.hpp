#pragma once

#include "error_handling.hpp"
#include "status_codes.h"

#include <cstdint>
#include <limits>

namespace cpp_core
{

/**
 * Shared handle validation for both platforms.
 * The C API passes handles as int64_t; internally they must fit into int (Linux fd)
 * or intptr_t (Windows HANDLE). This helper checks the common constraints.
 */
template <StatusConvertible Ret, ErrorCallback Callback>
constexpr auto validateHandle(int64_t handle, Callback &&error_callback) -> Ret
{
    if (handle <= 0 || handle > std::numeric_limits<int>::max())
    {
        return failMsg<Ret>(std::forward<Callback>(error_callback),
                            static_cast<StatusCodes>(StatusCode::Connection::kInvalidHandleError), "Invalid handle");
    }
    return static_cast<Ret>(StatusCode::kSuccess);
}

/**
 * Shared parameter validation for serialOpen.
 * Returns kSuccess (0) if all params are valid, or the appropriate negative error code.
 */
template <StatusConvertible Ret, ErrorCallback Callback>
constexpr auto validateOpenParams(void *port, int baudrate, int data_bits, Callback &&error_callback) -> Ret
{
    if (port == nullptr)
    {
        return failMsg<Ret>(std::forward<Callback>(error_callback),
                            static_cast<StatusCodes>(StatusCode::Connection::kNotFoundError),
                            "Port parameter is nullptr");
    }
    if (baudrate < 300)
    {
        return failMsg<Ret>(std::forward<Callback>(error_callback),
                            static_cast<StatusCodes>(StatusCode::Control::kSetStateError),
                            "Invalid baudrate: must be >= 300");
    }
    if (data_bits < 5 || data_bits > 8)
    {
        return failMsg<Ret>(std::forward<Callback>(error_callback),
                            static_cast<StatusCodes>(StatusCode::Control::kSetStateError),
                            "Invalid data bits: must be 5-8");
    }
    return static_cast<Ret>(StatusCode::kSuccess);
}

// Validate buffer + size for read/write calls.
template <StatusConvertible Ret, ErrorCallback Callback>
constexpr auto validateBuffer(const void *buffer, int buffer_size, Callback &&error_callback) -> Ret
{
    if (buffer == nullptr || buffer_size <= 0)
    {
        return failMsg<Ret>(std::forward<Callback>(error_callback),
                            static_cast<StatusCodes>(StatusCode::Io::kBufferError),
                            "Invalid buffer or buffer_size");
    }
    return static_cast<Ret>(StatusCode::kSuccess);
}

// Clamp timeout to non-negative.
constexpr auto clampTimeout(int timeout_ms) -> int
{
    return timeout_ms < 0 ? 0 : timeout_ms;
}

} // namespace cpp_core
