#pragma once
#include "../module_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Register a callback that is invoked whenever bytes are read.
     *
     * Pass `nullptr` to disable the callback.
     *
     * @param callback Function receiving the number of bytes that have just been read.
     */
    MODULE_API void serialSetReadCallback(void (*callback_fn)(int bytes_read));

#ifdef __cplusplus
}
#endif
