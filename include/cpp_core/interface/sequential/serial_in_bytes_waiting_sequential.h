#pragma once

#include "../../detail/sequential_dispatch.h"
#include "../../error_callback.h"
#include "../serial_in_bytes_waiting.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @copydoc serialInBytesWaiting
     * @note Sequential variant: guarantees execution in the exact order the calls were made across threads.
     */
    inline MODULE_API auto serialInBytesWaitingSequential(
        int64_t        handle,
        ErrorCallbackT error_callback = nullptr
    ) -> int
    {
        return cpp_core::detail::seq::call(handle, [=] { return serialInBytesWaiting(handle, error_callback); });
    }

#ifdef __cplusplus
}
#endif
