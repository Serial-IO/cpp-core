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
     * @brief Read until a specific byte sequence appears.
     *
     * Works like serialReadUntil() but supports an arbitrary terminator string.
     * The terminator is included in the returned data.
     *
     * @param handle Port handle.
     * @param buffer Destination buffer.
     * @param buffer_size Capacity of @p buffer in bytes.
     * @param timeout_config Timeout settings for this operation. Passing `nullptr` results in a failure.
     * @param sequence Pointer to the terminating byte sequence (must not be `nullptr`).
     * @param error_callback [optional] Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`.
     * @return Bytes read (including the terminator) or a negative error code from ::cpp_core::StatusCode on error.
     */
    MODULE_API auto serialReadUntilSequence(int64_t handle, void *buffer, int buffer_size,
                                            const cpp_core::SerialTimeoutConfig *timeout_config, void *sequence,
                                            ErrorCallbackT error_callback = nullptr) -> int;

#ifdef __cplusplus
}
#endif
