#pragma once
#include "../error_callback.h"
#include "../module_api.h"
#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Configure the flow-control mode for an open serial port.
     *
     * Flow control prevents buffer overruns when one side is slower than the
     * other. Three modes are supported:
     *
     * | @p mode | Meaning                                       |
     * |---------|-----------------------------------------------|
     * | 0       | None - no flow control (default after open).  |
     * | 1       | Hardware (RTS/CTS) - the UART automatically   |
     * |         | de-asserts RTS when the RX buffer is full and |
     * |         | pauses TX when CTS is de-asserted.            |
     * | 2       | Software (XON/XOFF) - in-band control chars   |
     * |         | `0x11` (XON) and `0x13` (XOFF) are sent to    |
     * |         | pause/resume the remote transmitter.          |
     *
     * Changing the mode on an already-open port takes effect immediately.
     *
     * @param handle Port handle obtained from serialOpen().
     * @param mode Flow-control mode: 0 = none, 1 = RTS/CTS, 2 = XON/XOFF.
     * @param error_callback [optional] Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`.
     * @return 0 on success or a negative error code from ::cpp_core::StatusCodes on error.
     */
    MODULE_API auto serialSetFlowControl(int64_t handle, int mode, ErrorCallbackT error_callback = nullptr) -> int;

#ifdef __cplusplus
}
#endif
