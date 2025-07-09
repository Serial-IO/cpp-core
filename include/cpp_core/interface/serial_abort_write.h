#pragma once
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
     */
    MODULE_API void serialAbortWrite(int64_t handle);

#ifdef __cplusplus
}
#endif
