#pragma once

#include "../../error_callback.h"
#include "../../internal/sequential_dispatch.h"
#include "../serial_close.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @copydoc serialClose
     * @note Sequential variant: guarantees execution in the exact order the calls were made across threads.
     */
    inline MODULE_API auto serialCloseSequential(
        int64_t        handle,
        ErrorCallbackT error_callback = nullptr
    ) -> int
    {
        return cpp_core::internal::seq::call(handle, [=] { return serialClose(handle, error_callback); });
    }

#ifdef __cplusplus
}
#endif
