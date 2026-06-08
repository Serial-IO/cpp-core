#pragma once
#include "../error_callback.h"
#include "../module_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Start or stop port attach/detach notifications.
     *
     * Passing a non-null callback starts monitoring and invokes it with `event = 1`
     * for attach and `event = 0` for detach notifications. Passing `nullptr`
     * stops a previously running monitor.
     *
     * @param callback_fn Notification callback or `nullptr` to stop monitoring.
     * @param error_callback [optional] Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`.
     * @return 0 on success or a negative error code from ::cpp_core::StatusCode on error.
     */
    MODULE_API auto serialMonitorPorts(void (*callback_fn)(int event, const char *port),
                                       ErrorCallbackT error_callback = nullptr) -> int;

#ifdef __cplusplus
}
#endif
