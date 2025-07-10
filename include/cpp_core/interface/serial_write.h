#pragma once
#include "../error_callback.h"
#include "../module_api.h"
#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Write raw bytes to the serial port.
     *
     * Timeout handling mirrors serialRead(): @p timeout_ms applies to the first
     * byte, `timeout_ms * multiplier` to every subsequent one.
     *
     * @param handle Port handle.
     * @param buffer Data to transmit (must not be `nullptr`).
     * @param buffer_size Number of bytes in @p buffer (> 0).
     * @param timeout_ms Base timeout per byte in milliseconds (applied to the first byte as-is; subsequent bytes use
     * `timeout_ms * multiplier`).
     * @param multiplier Factor applied to the timeout for subsequent bytes.
     * @return Bytes written (may be 0 on timeout) or a negative error code.
     */
    MODULE_API auto serialWrite(
        int64_t        handle,
        const void    *buffer,
        int            buffer_size,
        int            timeout_ms,
        int            multiplier,
        ErrorCallbackT error_callback = nullptr
    ) -> int;

#ifdef __cplusplus
}
#endif
