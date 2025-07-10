#pragma once
#include "../error_callback.h"
#include "../module_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Enumerate all available serial ports on the system.
     *
     * The supplied callback is invoked once for every discovered port. All string
     * parameters may be `nullptr` if the information is unknown.
     *
     * @param callback_fn Callback receiving port information.
     * @param error_callback [optional] Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`.
     * @return Number of ports found or a negative error code.
     */
    MODULE_API auto serialGetPortsInfo(
        void (*callback_fn)(
            const char *port,
            const char *path,
            const char *manufacturer,
            const char *serial_number,
            const char *pnp_id,
            const char *location_id,
            const char *product_id,
            const char *vendor_id
        ),
        ErrorCallbackT error_callback = nullptr
    ) -> int;

#ifdef __cplusplus
}
#endif
