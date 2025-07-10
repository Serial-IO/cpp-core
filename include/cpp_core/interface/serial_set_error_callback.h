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
     * @param error_callback Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`. Gets
     * invoked on any error.
     */
    MODULE_API void serialSetErrorCallback(ErrorCallbackT error_callback);

#ifdef __cplusplus
}
#endif
