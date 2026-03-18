#pragma once
#include "../error_callback.h"
#include "../module_api.h"
#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Read the current state of the Data Carrier Detect (DCD) input line.
     *
     * DCD is asserted by a modem when a carrier signal has been detected on the
     * telephone line. For direct serial links it can serve as a general-purpose
     * "connection alive" indicator.
     *
     * @param handle Port handle obtained from serialOpen().
     * @param error_callback [optional] Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`.
     * @return 1 if asserted (HIGH), 0 if de-asserted (LOW), or a negative error code from ::cpp_core::StatusCodes.
     */
    MODULE_API auto serialGetDcd(int64_t handle, ErrorCallbackT error_callback = nullptr) -> int;

#ifdef __cplusplus
}
#endif
