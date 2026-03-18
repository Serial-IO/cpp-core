#pragma once
#include "../error_callback.h"
#include "../module_api.h"
#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Set or clear the Request To Send (RTS) modem control line.
     *
     * RTS is typically used for hardware flow control (RTS/CTS) or
     * as a transmit-enable signal in half-duplex RS-485 setups. When
     * hardware flow control is **not** enabled via serialSetFlowControl(),
     * this function gives manual control over the line.
     *
     * @param handle Port handle obtained from serialOpen().
     * @param state Non-zero to assert (HIGH), zero to de-assert (LOW).
     * @param error_callback [optional] Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`.
     * @return 0 on success or a negative error code from ::cpp_core::StatusCodes on error.
     */
    MODULE_API auto serialSetRts(int64_t handle, int state, ErrorCallbackT error_callback = nullptr) -> int;

#ifdef __cplusplus
}
#endif
