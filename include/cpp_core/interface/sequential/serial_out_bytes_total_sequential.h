#pragma once

#include "../../error_callback.h"
#include "../../internal/sequential_dispatch.h"
#include "../serial_out_bytes_total.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @copydoc serialOutBytesTotal
     * @note Sequential variant: guarantees execution in the exact order the calls were made across threads.
     */
    inline MODULE_API auto serialOutBytesTotalSequential(
        int64_t        handle,
        ErrorCallbackT error_callback = nullptr
    ) -> int64_t
    {
        return cpp_core::internal::seq::call(handle, [=] { return serialOutBytesTotal(handle, error_callback); });
    }

#ifdef __cplusplus
}
#endif
