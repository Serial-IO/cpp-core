#pragma once

#include "../../error_callback.h"
#include "../../internal/sequential/call.h"
#include "../serial_abort_read.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @copydoc serialAbortRead
     * @note Sequential variant: guarantees execution in the exact order the calls were made across threads.
     */
    inline MODULE_API auto serialAbortReadSequential(
        int64_t        handle,
        ErrorCallbackT error_callback = nullptr
    ) -> int
    {
        return cpp_core::internal::sequential::call(handle, [=] { return serialAbortRead(handle, error_callback); });
    }

#ifdef __cplusplus
}
#endif
