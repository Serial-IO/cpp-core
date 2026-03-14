#pragma once
#include "../error_callback.h"
#include "../module_api.h"
#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Query the current line settings of an open serial port.
     *
     * The OS-level configuration is read back and reported via the supplied
     * callback. This is useful for verifying that serialOpen() or
     * serialUpdateBaudrate() applied the requested values, or for inspecting a
     * port that was configured externally.
     *
     * @code{.c}
     * void onConfig(int baud, int data_bits, int parity, int stop_bits, int flow_control) {
     *     printf("baud=%d data=%d par=%d stop=%d flow=%d\n",
     *            baud, data_bits, parity, stop_bits, flow_control);
     * }
     * serialGetConfig(handle, onConfig);
     * @endcode
     *
     * @param handle Port handle obtained from serialOpen().
     * @param callback_fn Callback receiving the current configuration values:
     *        - @p baudrate   Current baud rate in bit/s.
     *        - @p data_bits  Number of data bits (5-8).
     *        - @p parity     0 = none, 1 = even, 2 = odd.
     *        - @p stop_bits  0 = 1 stop bit, 2 = 2 stop bits.
     *        - @p flow_control 0 = none, 1 = RTS/CTS, 2 = XON/XOFF.
     * @param error_callback [optional] Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`.
     * @return 0 on success or a negative error code from ::cpp_core::StatusCodes on error.
     */
    MODULE_API auto serialGetConfig(int64_t handle,
                                    void (*callback_fn)(int baudrate, int data_bits, int parity, int stop_bits,
                                                        int flow_control),
                                    ErrorCallbackT error_callback = nullptr) -> int;

#ifdef __cplusplus
}
#endif
