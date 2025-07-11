#pragma once

#include "../../detail/sequential_dispatch.h"
#include "../../error_callback.h"
#include "../serial_clear_buffer_out.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @copydoc serialClearBufferOut
     * @note Sequential variant: guarantees execution in the exact order the calls were made across threads.
     */
    inline MODULE_API auto serialClearBufferOutSequential(
        int64_t        handle,
        ErrorCallbackT error_callback = nullptr
    ) -> int
    {
        return cpp_core::detail::seq::call([=] { return serialClearBufferOut(handle, error_callback); });
    }

#ifdef __cplusplus
}
#endif
