#pragma once
#include "../module_api.h"
#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Total number of bytes transmitted since the port was opened.
     *
     * @param handle Port handle.
     * @return Total number of bytes written.
     */
    MODULE_API auto serialOutBytesTotal(int64_t handle) -> int64_t;

#ifdef __cplusplus
}
#endif
