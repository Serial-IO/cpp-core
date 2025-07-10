#pragma once
#include "../error_callback.h"
#include "../module_api.h"
#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Abort a blocking read operation running in a different thread.
     *
     * The target read function returns immediately with
     * ::cpp_core::StatusCodes::kAbortReadError.
     *
     * @param handle Port handle.
     * @return 0 on success or a negative error code on failure.
     */
    MODULE_API auto serialAbortRead(
        int64_t        handle,
        ErrorCallbackT error_callback = nullptr
    ) -> int;

#ifdef __cplusplus
}
#endif
