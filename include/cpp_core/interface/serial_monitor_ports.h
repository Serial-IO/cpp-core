#pragma once
#include "../error_callback.h"
#include "../module_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Start monitoring for serial-port hot-plug events.
     *
     * Registers a callback that is invoked whenever a serial port appears or
     * disappears on the system (e.g. a USB-to-serial adapter is plugged in or
     * removed). The monitoring runs on a background thread managed by the
     * library.
     *
     * Only **one** monitor can be active at a time. Calling this function again
     * replaces the previous callback. Pass `nullptr` as @p callback_fn to stop
     * monitoring and release the background thread.
     *
     * @code{.c}
     * void onChange(int event, const char *port) {
     *     if (event == 1) printf("connected: %s\n", port);
     *     else            printf("disconnected: %s\n", port);
     * }
     * serialMonitorPorts(onChange);
     * // ... later ...
     * serialMonitorPorts(nullptr);  // stop
     * @endcode
     *
     * @param callback_fn Callback receiving:
     *        - @p event 1 = port appeared (connected), 0 = port disappeared (disconnected).
     *        - @p port  Null-terminated device identifier (e.g. "COM3", "/dev/ttyUSB0").
     *                   Valid only for the duration of the callback.
     * @param error_callback [optional] Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`.
     * @return 0 on success or a negative error code from ::cpp_core::StatusCodes on error.
     */
    MODULE_API auto serialMonitorPorts(void (*callback_fn)(int event, const char *port),
                                       ErrorCallbackT error_callback = nullptr) -> int;

#ifdef __cplusplus
}
#endif
