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
     * Reads bytes using the same timeout semantics as serialRead() and stops as
     * soon as @p sequence has been received. Sequences may contain zero bytes,
     * and the complete terminator is included in the returned data.
     *
     * @param handle Port handle.
     * @param buffer Destination buffer.
     * @param buffer_size Capacity of @p buffer in bytes.
     * @param timeout_config Timeout settings for this operation. Passing `nullptr` results in a failure.
     * @param sequence Terminating byte sequence (must not be `nullptr`).
     * @param sequence_size Size of @p sequence in bytes (> 0).
     * @param error_callback [optional] Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`.
     * @return Bytes read (including the terminator) or a negative error code from ::cpp_core::StatusCode on error.
     */
    MODULE_API auto serialReadUntilSequence(int64_t handle, std::uint8_t *buffer, int buffer_size,
                                            const cpp_core::SerialTimeoutConfig *timeout_config,
                                            const std::uint8_t *sequence, int sequence_size,
                                            ErrorCallbackT error_callback = nullptr) -> int;

#ifdef __cplusplus
}
#endif
