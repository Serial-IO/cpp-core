#pragma once
#include "../error_callback.h"
#include "../module_api.h"
#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Read the current state of the Clear To Send (CTS) input line.
     *
     * CTS is asserted by the remote device to indicate it is ready to receive
     * data. Polling this line is useful when manual flow-control logic is
     * required instead of (or in addition to) automatic RTS/CTS flow control.
     *
     * @param handle Port handle obtained from serialOpen().
     * @param error_callback [optional] Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`.
     * @return 1 if asserted (HIGH), 0 if de-asserted (LOW), or a negative error code from ::cpp_core::StatusCodes.
     */
    MODULE_API auto serialGetCts(int64_t handle, ErrorCallbackT error_callback = nullptr) -> int;

#ifdef __cplusplus
}
#endif
