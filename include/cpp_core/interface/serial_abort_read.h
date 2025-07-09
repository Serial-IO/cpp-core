#pragma once
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
     */
    MODULE_API void serialAbortRead(int64_t handle);

#ifdef __cplusplus
}
#endif
