#pragma once
#include "../module_api.h"
#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Read a single line terminated by '\n'.
     *
     * Timeout handling is identical to serialRead(); the newline character is
     * included in the returned data.
     *
     * @param handle       Port handle.
     * @param buffer       Destination buffer.
     * @param buffer_size  Capacity of @p buffer in bytes.
     * @param timeout_ms   Base timeout per byte in milliseconds (applied to the
     *                     first byte as-is; subsequent bytes use
     *                     `timeout_ms * multiplier`).
     * @param multiplier   Factor applied to the timeout for subsequent bytes.
     * @return             Bytes read (0 on timeout) or a negative error code.
     */
    MODULE_API auto serialReadLine(
        int64_t handle,
        void   *buffer,
        int     buffer_size,
        int     timeout_ms,
        int     multiplier
    ) -> int;

#ifdef __cplusplus
}
#endif
