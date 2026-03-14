#pragma once
#include "../error_callback.h"
#include "../module_api.h"
#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Change the baud rate of an already-open serial port.
     *
     * The new rate takes effect immediately without closing and re-opening the
     * port. All other line settings (data bits, parity, stop bits, flow control)
     * remain unchanged.
     *
     * Typical use-case: a bootloader handshake starts at a safe 9600 baud and
     * then both sides switch to a higher speed for the actual data transfer.
     *
     * @code{.c}
     * intptr_t h = serialOpen("/dev/ttyUSB0", 9600, 8);
     * // ... bootloader handshake at 9600 ...
     * serialUpdateBaudrate(h, 115200);
     * // ... fast data transfer ...
     * @endcode
     *
     * @param handle Port handle obtained from serialOpen().
     * @param baudrate New baud rate in bit/s (>= 300).
     * @param error_callback [optional] Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`.
     * @return 0 on success or a negative error code from ::cpp_core::StatusCodes on error.
     */
    MODULE_API auto serialUpdateBaudrate(int64_t handle, int baudrate,
                                         ErrorCallbackT error_callback = nullptr) -> int;

#ifdef __cplusplus
}
#endif
