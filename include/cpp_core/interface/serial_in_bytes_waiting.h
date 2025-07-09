#pragma once
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
     * @return       Bytes available for instant reading or a negative error code.
     */
    MODULE_API auto serialInBytesWaiting(int64_t handle) -> int;

#ifdef __cplusplus
}
#endif
