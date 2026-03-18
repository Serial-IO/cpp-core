#pragma once
#include "../error_callback.h"
#include "../module_api.h"
#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Change the stop-bit setting on an already-open serial port.
     *
     * Takes effect immediately. All other line settings remain unchanged.
     *
     * @param handle Port handle obtained from serialOpen().
     * @param stop_bits 0 = 1 stop bit, 2 = 2 stop bits.
     * @param error_callback [optional] Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`.
     * @return 0 on success or a negative error code from ::cpp_core::StatusCodes on error.
     */
    MODULE_API auto serialSetStopBits(int64_t handle, int stop_bits,
                                      ErrorCallbackT error_callback = nullptr) -> int;

#ifdef __cplusplus
}
#endif
