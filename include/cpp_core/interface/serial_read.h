#pragma once
#include "../error_callback.h"
#include "../module_api.h"
#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Read raw bytes from the serial port.
     *
     * The call blocks for at most @p timeout_ms milliseconds while waiting for
     * the FIRST byte. For every subsequent byte the individual timeout is
     * calculated as `timeout_ms * multiplier`.
     *
     * @param handle Port handle.
     * @param buffer Destination buffer (must not be `nullptr`).
     * @param buffer_size Size of @p buffer in bytes (> 0).
     * @param timeout_ms Base timeout per byte in milliseconds (applied to the first byte as-is; subsequent bytes use
     * `timeout_ms * multiplier`).
     * @param multiplier Factor applied to @p timeout_ms for every byte after the first. 0 -> return immediately after
     * the first byte.
     * @param error_callback [optional] Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`.
     * @return Bytes read (0 on timeout) or a negative error code from ::cpp_core::StatusCodes on error.
     */
    MODULE_API auto serialRead(
        int64_t        handle,
        void          *buffer,
        int            buffer_size,
        int            timeout_ms,
        int            multiplier,
        ErrorCallbackT error_callback = nullptr
    ) -> int;

#ifdef __cplusplus
}
#endif
