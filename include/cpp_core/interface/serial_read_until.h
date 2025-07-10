#pragma once
#include "../error_callback.h"
#include "../module_api.h"
#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Read bytes until a terminator character appears.
     *
     * Semantics are identical to serialRead() but reading stops as soon as the
     * byte pointed to by @p until_char has been received. The terminator is part
     * of the returned data.
     *
     * @param handle Port handle.
     * @param buffer Destination buffer.
     * @param buffer_size Capacity of @p buffer in bytes.
     * @param timeout_ms Base timeout per byte in milliseconds (applied to the first byte as-is; each additional byte
     * uses `timeout_ms * multiplier`).
     * @param multiplier Factor applied to the timeout for every additional byte.
     * @param until_char Pointer to the terminator character (must not be `nullptr`).
     * @return Bytes read (including the terminator), 0 on timeout or a negative error code.
     */
    MODULE_API auto serialReadUntil(
        int64_t        handle,
        void          *buffer,
        int            buffer_size,
        int            timeout_ms,
        int            multiplier,
        void          *until_char,
        ErrorCallbackT error_callback = nullptr
    ) -> int;

#ifdef __cplusplus
}
#endif
