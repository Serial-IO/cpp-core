#pragma once

#include "error_handling.hpp"
#include "serial_config.hpp"
#include "status_code.h"

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
                            static_cast<StatusCodeValue>(StatusCode::Connection::kInvalidHandleError),
                            "Invalid handle");
    }
    return static_cast<Ret>(StatusCode::kSuccess);
}

/**
 * Shared parameter validation for serialOpen.
 * Returns kSuccess (0) if all params are valid, or the appropriate negative error code.
 */
template <StatusConvertible Ret, ErrorCallback Callback>
constexpr auto validateOpenParams(const char *port, const SerialConfig *config, Callback &&error_callback) -> Ret
{
    if (port == nullptr)
    {
        return failMsg<Ret>(std::forward<Callback>(error_callback),
                            static_cast<StatusCodeValue>(StatusCode::Connection::kNotFoundError),
                            "Port parameter is nullptr");
    }
    if (config == nullptr)
    {
        return failMsg<Ret>(std::forward<Callback>(error_callback),
                            static_cast<StatusCodeValue>(StatusCode::Control::kSetStateError),
                            "Config parameter is nullptr");
    }
    if (!detail::validateBaudrate(config->baudrate))
    {
        return failMsg<Ret>(std::forward<Callback>(error_callback),
                            static_cast<StatusCodeValue>(StatusCode::Configuration::kSetBaudrateError),
                            "Invalid baudrate: must be >= 300");
    }
    if (!detail::validateDataBits(config->data_bits))
    {
        return failMsg<Ret>(std::forward<Callback>(error_callback),
                            static_cast<StatusCodeValue>(StatusCode::Configuration::kSetDataBitsError),
                            "Invalid data bits: must be 5-8");
    }
    if (!detail::validateParity(config->parity))
    {
        return failMsg<Ret>(std::forward<Callback>(error_callback),
                            static_cast<StatusCodeValue>(StatusCode::Configuration::kSetParityError),
                            "Invalid parity mode");
    }
    if (!detail::validateStopBits(config->stop_bits))
    {
        return failMsg<Ret>(std::forward<Callback>(error_callback),
                            static_cast<StatusCodeValue>(StatusCode::Configuration::kSetStopBitsError),
                            "Invalid stop bits mode");
    }
    if (!detail::validateFlowControl(config->flow_mode))
    {
        return failMsg<Ret>(std::forward<Callback>(error_callback),
                            static_cast<StatusCodeValue>(StatusCode::Configuration::kSetFlowControlError),
                            "Invalid flow-control mode");
    }
    return static_cast<Ret>(StatusCode::kSuccess);
}

/**
 * Shared timeout validation for serial read and write operations.
 */
template <StatusConvertible Ret, ErrorCallback Callback>
constexpr auto validateTimeoutConfig(const SerialTimeoutConfig *config, Callback &&error_callback) -> Ret
{
    if (config == nullptr)
    {
        return failMsg<Ret>(std::forward<Callback>(error_callback),
                            static_cast<StatusCodeValue>(StatusCode::Configuration::kSetTimeoutError),
                            "Timeout config parameter is nullptr");
    }
    if (!config->isValid())
    {
        return failMsg<Ret>(std::forward<Callback>(error_callback),
                            static_cast<StatusCodeValue>(StatusCode::Configuration::kSetTimeoutError),
                            "Timeout and multiplier must be non-negative and their product must fit into int");
    }
    return static_cast<Ret>(StatusCode::kSuccess);
}

// Validate buffer + size for read/write calls.
template <StatusConvertible Ret, ErrorCallback Callback>
constexpr auto validateBuffer(const std::uint8_t *buffer, int buffer_size, Callback &&error_callback) -> Ret
{
    if (buffer == nullptr || buffer_size <= 0)
    {
        return failMsg<Ret>(std::forward<Callback>(error_callback),
                            static_cast<StatusCodeValue>(StatusCode::Io::kBufferError),
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
