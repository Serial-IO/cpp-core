#pragma once
#include "../error_callback.h"
#include "../module_api.h"
#include <cstdint>

/**
 * @brief Wait until the operating-system driver has physically sent every queued byte.
 *
 * The function blocks the *calling thread* until the device driver reports
 * that the transmit FIFO is **empty** - i.e. all bytes handed to previous
 * `serialWrite*` calls have been shifted out on the wire.  It does *not*
 * flush higher-level protocol buffers you may have implemented yourself.
 *
 * Typical use-case: ensure a complete command frame has left the UART
 * before toggling RTS/DTR or powering down the device.
 *
 * @code{.cpp}
 * // Send a frame and make sure it actually hits the line
 * constexpr cpp_core::SerialTimeoutConfig timeout{.timeout_ms = 50, .multiplier = 1};
 * serialWrite(h, frame, frame_len, &timeout);
 * if (serialDrain(h) < 0) {
 *     fprintf(stderr, "drain failed\n");
 * }
 * @endcode
 *
 * @param handle Port handle.
 * @param error_callback [optional] Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`.
 * @return 0 on success or a negative error code from ::cpp_core::StatusCode on error.
 */
MODULE_API auto serialDrain(int64_t handle, ErrorCallbackT error_callback = nullptr) -> int;
