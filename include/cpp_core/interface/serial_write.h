#pragma once
#include "../error_callback.h"
#include "../module_api.h"
#include "../serial_config.hpp"
#include <cstdint>

/**
 * @brief Write raw bytes to the serial port.
 *
 * Timeout handling mirrors serialRead(): `timeout_config->timeout_ms` applies
 * to the first byte, `timeout_ms * multiplier` to every subsequent one.
 *
 * @param handle Port handle.
 * @param buffer Data to transmit (must not be `nullptr`).
 * @param buffer_size Number of bytes in @p buffer (> 0).
 * @param timeout_config Timeout settings for this operation. Passing `nullptr` results in a failure.
 * @param error_callback [optional] Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`.
 * @return Bytes written (may be 0 on timeout) or a negative error code from ::cpp_core::StatusCode on error.
 */
MODULE_API auto serialWrite(int64_t handle, const std::uint8_t *buffer, int buffer_size,
                            const cpp_core::SerialTimeoutConfig *timeout_config,
                            ErrorCallbackT error_callback = nullptr) -> int;
