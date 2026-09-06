#pragma once
#include "../error_callback.h"
#include "../module_api.h"
#include "../strong_types.hpp"
#include <cstdint>

/**
 * @brief Change the stop-bit setting on an already-open serial port.
 *
 * Takes effect immediately. All other line settings remain unchanged.
 *
 * @param handle Port handle obtained from serialOpen().
 * @param stop_bits Stop-bit mode.
 * @param error_callback [optional] Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`.
 * @return 0 on success or a negative error code from ::cpp_core::StatusCode on error.
 */
MODULE_API auto serialSetStopBits(int64_t handle, cpp_core::StopBits stop_bits, ErrorCallbackT error_callback = nullptr)
    -> int;
