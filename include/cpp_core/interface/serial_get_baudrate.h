#pragma once
#include "../error_callback.h"
#include "../module_api.h"
#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Query the current baud rate of an open serial port.
     *
     * Reads back the baud rate that the OS driver is currently using. Useful for
     * verifying that serialOpen() or serialSetBaudrate() applied the requested
     * value.
     *
     * @param handle Port handle obtained from serialOpen().
     * @param error_callback [optional] Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`.
     * @return Current baud rate in bit/s (>= 300) or a negative error code from ::cpp_core::StatusCodes.
     */
    MODULE_API auto serialGetBaudrate(int64_t handle, ErrorCallbackT error_callback = nullptr) -> int;

#ifdef __cplusplus
}
#endif
