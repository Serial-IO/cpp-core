#pragma once
#include "../module_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Register a callback that is invoked whenever bytes are written.
     *
     * Pass `nullptr` to disable the callback.
     *
     * @param callback Function receiving the number of bytes that have just been written.
     */
    MODULE_API void serialSetWriteCallback(void (*callback_fn)(int bytes_written));

#ifdef __cplusplus
}
#endif
