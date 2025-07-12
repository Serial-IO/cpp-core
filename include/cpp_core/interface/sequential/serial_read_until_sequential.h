#pragma once

#include "../../detail/sequential_dispatch.h"
#include "../../error_callback.h"
#include "../serial_read_until.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @copydoc serialReadUntil
     * @note Sequential variant: guarantees execution in the exact order the calls were made across threads.
     */
    inline MODULE_API auto serialReadUntilSequential(
        int64_t        handle,
        void          *buffer,
        int            buffer_size,
        int            timeout_ms,
        int            multiplier,
        void          *until_char,
        ErrorCallbackT error_callback = nullptr
    ) -> int
    {
        return cpp_core::detail::seq::call(handle, [=] {
            return serialReadUntil(handle, buffer, buffer_size, timeout_ms, multiplier, until_char, error_callback);
        });
    }

#ifdef __cplusplus
}
#endif
