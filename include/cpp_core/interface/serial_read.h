#pragma once
#include "../error_callback.h"
#include "../module_api.h"
#include "../serial_config.hpp"
#include <cstdint>

/**
 * @brief Read raw bytes from the serial port.
 *
 * The call blocks for at most `timeout_config->timeout_ms` milliseconds while waiting for
 * the FIRST byte. For every subsequent byte the individual timeout is
 * calculated as `timeout_ms * multiplier` from @p timeout_config.
 *
 * @param handle Port handle.
 * @param buffer Destination buffer (must not be `nullptr`).
 * @param buffer_size Size of @p buffer in bytes (> 0).
 * @param timeout_config Timeout settings for this operation. Passing `nullptr` results in a failure.
 * @param error_callback [optional] Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`.
 * @return Bytes read (0 on timeout) or a negative error code from ::cpp_core::StatusCode on error.
 */
MODULE_API auto serialRead(int64_t handle, std::uint8_t *buffer, int buffer_size,
                           const cpp_core::SerialTimeoutConfig *timeout_config, ErrorCallbackT error_callback = nullptr)
    -> int;
