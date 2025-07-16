#pragma once

#include "../../error_callback.h"
#include "../../internal/sequential/call.h"
#include "../serial_abort_write.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @copydoc serialAbortWrite
     * @note Sequential variant: guarantees execution in the exact order the calls were made across threads.
     */
    inline MODULE_API auto serialAbortWriteSequential(
        int64_t        handle,
        ErrorCallbackT error_callback = nullptr
    ) -> int
    {
        return cpp_core::internal::sequential::call(handle, [=] { return serialAbortWrite(handle, error_callback); });
    }

#ifdef __cplusplus
}
#endif
