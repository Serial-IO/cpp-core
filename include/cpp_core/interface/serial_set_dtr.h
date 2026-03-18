#pragma once
#include "../error_callback.h"
#include "../module_api.h"
#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Set or clear the Data Terminal Ready (DTR) modem control line.
     *
     * DTR is commonly used for:
     * - Signalling readiness to the remote device.
     * - Triggering a board reset on Arduino-compatible hardware.
     * - Half-duplex direction control on RS-485 adapters.
     *
     * @param handle Port handle obtained from serialOpen().
     * @param state Non-zero to assert (HIGH), zero to de-assert (LOW).
     * @param error_callback [optional] Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`.
     * @return 0 on success or a negative error code from ::cpp_core::StatusCodes on error.
     */
    MODULE_API auto serialSetDtr(int64_t handle, int state, ErrorCallbackT error_callback = nullptr) -> int;

#ifdef __cplusplus
}
#endif
