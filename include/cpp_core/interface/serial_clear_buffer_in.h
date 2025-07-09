#pragma once
#include "../module_api.h"
#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Clear (flush) the device's input buffer.
     *
     * Discards every byte the driver has already received but the application
     * has not yet read.
     *
     * @param handle Port handle.
     */
    MODULE_API void serialClearBufferIn(int64_t handle);

#ifdef __cplusplus
}
#endif
