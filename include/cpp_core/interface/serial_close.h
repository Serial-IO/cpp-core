#pragma once
#include "../error_callback.h"
#include "../module_api.h"
#include <cstdint>

/**
 * @brief Close a previously opened serial port.
 *
 * The handle becomes invalid after the call. Passing an already invalid
 * (<= 0) handle is a no-op.
 *
 * @param handle Handle obtained from serialOpen().
 * @param error_callback [optional] Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`.
 * @return 0 on success or a negative error code from ::cpp_core::StatusCode on error.
 */
MODULE_API auto serialClose(int64_t handle, ErrorCallbackT error_callback = nullptr) -> int;
