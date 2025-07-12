#pragma once

#include "../../detail/sequential_dispatch.h"
#include "../../error_callback.h"
#include "../serial_read.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @copydoc serialRead
     * @note Sequential variant: guarantees execution in the exact order the calls were made across threads.
     */
    inline MODULE_API auto serialReadSequential(
        int64_t        handle,
        void          *buffer,
        int            buffer_size,
        int            timeout_ms,
        int            multiplier,
        ErrorCallbackT error_callback = nullptr
    ) -> int
    {
        return cpp_core::detail::seq::call(handle, [=] {
            return serialRead(handle, buffer, buffer_size, timeout_ms, multiplier, error_callback);
        });
    }

#ifdef __cplusplus
}
#endif
