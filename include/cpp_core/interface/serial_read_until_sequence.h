#pragma once
#include "../module_api.h"
#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Read until a specific byte sequence appears.
     *
     * Works like serialReadUntil() but supports an arbitrary terminator string.
     * The terminator is included in the returned data.
     *
     * @param handle       Port handle.
     * @param buffer       Destination buffer.
     * @param buffer_size  Capacity of @p buffer in bytes.
     * @param timeout_ms   Base timeout per byte in milliseconds (first byte
     *                     uses this value; each additional byte uses
     *                     `timeout_ms * multiplier`).
     * @param multiplier   Factor applied to the timeout for subsequent bytes.
     * @param sequence     Pointer to the terminating byte sequence (must not be `nullptr`).
     * @return             Bytes read (including the terminator) or a negative error code.
     */
    MODULE_API auto serialReadUntilSequence(
        int64_t handle,
        void   *buffer,
        int     buffer_size,
        int     timeout_ms,
        int     multiplier,
        void   *sequence
    ) -> int;

#ifdef __cplusplus
}
#endif
