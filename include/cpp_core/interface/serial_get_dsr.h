#pragma once
#include "../error_callback.h"
#include "../module_api.h"
#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Read the current state of the Data Set Ready (DSR) input line.
     *
     * DSR is asserted by the remote device to indicate it is powered on and
     * ready to communicate. It is the counterpart to DTR.
     *
     * @param handle Port handle obtained from serialOpen().
     * @param error_callback [optional] Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`.
     * @return 1 if asserted (HIGH), 0 if de-asserted (LOW), or a negative error code from ::cpp_core::StatusCodes.
     */
    MODULE_API auto serialGetDsr(int64_t handle, ErrorCallbackT error_callback = nullptr) -> int;

#ifdef __cplusplus
}
#endif
