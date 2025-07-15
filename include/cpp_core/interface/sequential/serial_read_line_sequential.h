#pragma once

#include "../../error_callback.h"
#include "../../internal/sequential_dispatch.h"
#include "../serial_read_line.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @copydoc serialReadLine
     * @note Sequential variant: guarantees execution in the exact order the calls were made across threads.
     */
    inline MODULE_API auto serialReadLineSequential(
        int64_t        handle,
        void          *buffer,
        int            buffer_size,
        int            timeout_ms,
        int            multiplier,
        ErrorCallbackT error_callback = nullptr
    ) -> int
    {
        return cpp_core::internal::seq::call(handle, [=] {
            return serialReadLine(handle, buffer, buffer_size, timeout_ms, multiplier, error_callback);
        });
    }

#ifdef __cplusplus
}
#endif
