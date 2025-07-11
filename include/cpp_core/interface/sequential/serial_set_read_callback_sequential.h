#pragma once

#include "../../detail/sequential_dispatch.h"
#include "../serial_set_read_callback.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @copydoc serialSetReadCallback
     * @note Sequential variant: guarantees execution in the exact order the calls were made across threads.
     */
    inline MODULE_API void serialSetReadCallbackSequential(void (*callback_fn)(int bytes_read))
    {
        cpp_core::detail::seq::call([=] { serialSetReadCallback(callback_fn); });
    }

#ifdef __cplusplus
}
#endif
