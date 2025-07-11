#pragma once

#include "../../detail/sequential_dispatch.h"
#include "../../error_callback.h"
#include "../serial_get_ports_info.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @copydoc serialGetPortsInfo
     * @note Sequential variant: guarantees execution in the exact order the calls were made across threads.
     */
    inline MODULE_API auto serialGetPortsInfoSequential(
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
    ) -> int
    {
        return cpp_core::detail::seq::call([=] { return serialGetPortsInfo(callback_fn, error_callback); });
    }

#ifdef __cplusplus
}
#endif
