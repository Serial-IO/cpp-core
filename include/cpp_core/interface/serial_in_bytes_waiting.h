#pragma once
#include "../error_callback.h"
#include "../module_api.h"
#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Query how many bytes can be read *immediately* without blocking.
     *
     * The number reflects the size of the driver's RX FIFO **after** accounting
     * for data already consumed by the application.  A value of `0` therefore
     * means a read call would have to wait for the next byte to arrive.
     *
     * @code{.c}
     * int pending = serialInBytesWaiting(h);
     * if (pending > 0) {
     *     serialRead(h, buf, pending, 0, 1); // non-blocking read
     * }
     * @endcode
     *
     * @param handle Port handle.
     * @param error_callback [optional] Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`.
     * @return Bytes available for instant reading or a negative error code from ::cpp_core::StatusCodes on error.
     */
    MODULE_API auto serialInBytesWaiting(int64_t handle, ErrorCallbackT error_callback = nullptr) -> int;

#ifdef __cplusplus
}
#endif
