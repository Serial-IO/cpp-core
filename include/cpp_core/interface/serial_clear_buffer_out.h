#pragma once
#include "../module_api.h"
#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Clear (flush) the device's output buffer.
     *
     * Blocks until all queued bytes have been transmitted and then discards any
     * remaining data.
     *
     * @param handle Port handle.
     */
    MODULE_API void serialClearBufferOut(int64_t handle);

#ifdef __cplusplus
}
#endif
