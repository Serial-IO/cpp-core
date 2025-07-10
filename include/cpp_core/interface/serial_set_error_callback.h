#pragma once
#include "../error_callback.h"
#include "../module_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Register a callback that is invoked whenever an error occurs.
     *
     * Pass `nullptr` to disable the callback.
     *
     * @param callback Function receiving the error code and a textual description.
     */
    MODULE_API void serialSetErrorCallback(ErrorCallbackT error_callback);

#ifdef __cplusplus
}
#endif
