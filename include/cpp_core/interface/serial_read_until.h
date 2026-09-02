#pragma once
#include "../error_callback.h"
#include "../module_api.h"
#include "../serial_config.hpp"
#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Read bytes until a terminator byte appears.
     *
     * Semantics are identical to serialRead() but reading stops as soon as the
     * byte supplied as @p until_byte has been received. The terminator is part
     * of the returned data.
     *
     * @param handle Port handle.
     * @param buffer Destination buffer.
     * @param buffer_size Capacity of @p buffer in bytes.
     * @param timeout_config Timeout settings for this operation. Passing `nullptr` results in a failure.
     * @param until_byte Terminator byte.
     * @param error_callback [optional] Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`.
     * @return Bytes read (including the terminator), 0 on timeout or a negative error code from ::cpp_core::StatusCode
     * on error.
     */
    MODULE_API auto serialReadUntil(int64_t handle, std::uint8_t *buffer, int buffer_size,
                                    const cpp_core::SerialTimeoutConfig *timeout_config, std::uint8_t until_byte,
                                    ErrorCallbackT error_callback = nullptr) -> int;

#ifdef __cplusplus
}
#endif
