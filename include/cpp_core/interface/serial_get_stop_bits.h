#pragma once
#include "../error_callback.h"
#include "../module_api.h"
#include "../strong_types.hpp"
#include <cstdint>

/**
 * @brief Query the current stop-bit setting of an open serial port.
 *
 * @param handle Port handle obtained from serialOpen().
 * @param error_callback [optional] Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`.
 * @return Current stop-bit setting. On error, the underlying integer value is a negative error code from
 * ::cpp_core::StatusCode.
 */
MODULE_API auto serialGetStopBits(int64_t handle, ErrorCallbackT error_callback = nullptr) -> cpp_core::StopBits;
