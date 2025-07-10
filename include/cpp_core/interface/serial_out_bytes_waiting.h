#pragma once
#include "../error_callback.h"
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
     * @param error_callback [optional] Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`.
     * @return Bytes still waiting in the TX FIFO or a negative error code from ::cpp_core::StatusCodes on error.
     */
    MODULE_API auto serialOutBytesWaiting(
        int64_t        handle,
        ErrorCallbackT error_callback = nullptr
    ) -> int;

#ifdef __cplusplus
}
#endif
