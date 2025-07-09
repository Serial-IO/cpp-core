#pragma once
#include "../module_api.h"
#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Write a buffer followed by a '\n' byte.
     *
     * Convenience wrapper that adds the line-feed for you and then calls
     * `serialWrite()`.
     *
     * @param handle Port handle.
     * @param buffer Data to transmit.
     * @param buffer_size Number of bytes in @p buffer.
     * @param timeout_ms Base timeout per byte in milliseconds.
     * @param multiplier Factor applied to the timeout for bytes after the first.
     * @return Bytes written (including the newline) or a negative error code.
     */
    MODULE_API auto serialWriteLine(
        int64_t     handle,
        const void *buffer,
        int         buffer_size,
        int         timeout_ms,
        int         multiplier
    ) -> int;

#ifdef __cplusplus
}
#endif
