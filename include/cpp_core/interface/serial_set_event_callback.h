#pragma once
#include "../error_callback.h"
#include "../module_api.h"
#include "../strong_types.hpp"

/**
 * @brief Register or clear the serial-port lifecycle event callback.
 *
 * Passing a non-null callback starts monitoring and invokes it with
 * ::cpp_core::PortEvent::kAttached or ::cpp_core::PortEvent::kDetached.
 * Passing `nullptr` clears the callback and stops monitoring.
 *
 * @param callback_fn Notification callback or `nullptr` to stop monitoring.
 * @param error_callback [optional] Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`.
 * @return 0 on success or a negative error code from ::cpp_core::StatusCode on error.
 */
MODULE_API auto serialSetEventCallback(void (*callback_fn)(cpp_core::PortEvent event, const char *port),
                                       ErrorCallbackT error_callback = nullptr) -> int;
