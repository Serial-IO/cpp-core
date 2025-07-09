#pragma once
#include "../module_api.h"
#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Total number of bytes received since the port was opened.
     *
     * @param handle Port handle.
     * @return Total number of bytes read.
     */
    MODULE_API auto serialInBytesTotal(int64_t handle) -> int64_t;

#ifdef __cplusplus
}
#endif
