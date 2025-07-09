#pragma once
#include "../module_api.h"
#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Return the number of bytes that have been accepted by the driver but not yet sent.
     *
     * Useful for gauging transmission progress in the background or for pacing
     * further writes to avoid unbounded buffering.
     *
     * @code{.c}
     * while (serialOutBytesWaiting(h) > 0) {
     *     usleep(1000); // wait 1 ms and poll again
     * }
     * @endcode
     *
     * @param handle Port handle.
     * @return Bytes still waiting in the TX FIFO or a negative error code.
     */
    MODULE_API auto serialOutBytesWaiting(int64_t handle) -> int;

#ifdef __cplusplus
}
#endif
