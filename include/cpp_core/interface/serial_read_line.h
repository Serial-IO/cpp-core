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
     * @brief Read a single line terminated by '\n'.
     *
     * Timeout handling is identical to serialRead(); the newline character is
     * included in the returned data.
     *
     * @param handle Port handle.
     * @param buffer Destination buffer.
     * @param buffer_size Capacity of @p buffer in bytes.
     * @param timeout_config Timeout settings for this operation. Passing `nullptr` results in a failure.
     * @param error_callback [optional] Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`.
     * @return Bytes read (0 on timeout) or a negative error code from ::cpp_core::StatusCode on error.
     */
    MODULE_API auto serialReadLine(int64_t handle, std::uint8_t *buffer, int buffer_size,
                                   const cpp_core::SerialTimeoutConfig *timeout_config,
                                   ErrorCallbackT error_callback = nullptr) -> int;

#ifdef __cplusplus
}
#endif
