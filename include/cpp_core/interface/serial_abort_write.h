#pragma once
#include "../error_callback.h"
#include "../module_api.h"
#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Abort a blocking write operation running in a different thread.
     *
     * The target write function returns immediately with
     * ::cpp_core::StatusCodes::kAbortWriteError.
     *
     * @param handle Port handle.
     * @param error_callback [optional] Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`.
     * @return 0 on success or a negative error code from ::cpp_core::StatusCodes on error.
     */
    MODULE_API auto serialAbortWrite(int64_t handle, ErrorCallbackT error_callback = nullptr) -> int;

#ifdef __cplusplus
}
#endif
