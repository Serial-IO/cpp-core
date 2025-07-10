#pragma once
#include "../error_callback.h"
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
     * @param handle Port handle.
     * @param buffer Destination buffer.
     * @param buffer_size Capacity of @p buffer in bytes.
     * @param timeout_ms Base timeout per byte in milliseconds (first byte uses this value; each additional byte uses
     * `timeout_ms * multiplier`).
     * @param multiplier Factor applied to the timeout for subsequent bytes.
     * @param sequence Pointer to the terminating byte sequence (must not be `nullptr`).
     * @param error_callback [optional] Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`.
     * @return Bytes read (including the terminator) or a negative error code from ::cpp_core::StatusCodes on error.
     */
    MODULE_API auto serialReadUntilSequence(
        int64_t        handle,
        void          *buffer,
        int            buffer_size,
        int            timeout_ms,
        int            multiplier,
        void          *sequence,
        ErrorCallbackT error_callback = nullptr
    ) -> int;

#ifdef __cplusplus
}
#endif
