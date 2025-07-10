#pragma once
#include "../error_callback.h"
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
     * @param error_callback [optional] Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`.
     * @return Total number of bytes read or a negative error code.
     */
    MODULE_API auto serialInBytesTotal(
        int64_t        handle,
        ErrorCallbackT error_callback = nullptr
    ) -> int64_t;

#ifdef __cplusplus
}
#endif
