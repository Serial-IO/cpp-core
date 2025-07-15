#pragma once

#include "../../error_callback.h"
#include "../../internal/sequential_dispatch.h"
#include "../serial_in_bytes_total.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @copydoc serialInBytesTotal
     * @note Sequential variant: guarantees execution in the exact order the calls were made across threads.
     */
    inline MODULE_API auto serialInBytesTotalSequential(
        int64_t        handle,
        ErrorCallbackT error_callback = nullptr
    ) -> int64_t
    {
        return cpp_core::internal::seq::call(handle, [=] { return serialInBytesTotal(handle, error_callback); });
    }

#ifdef __cplusplus
}
#endif
