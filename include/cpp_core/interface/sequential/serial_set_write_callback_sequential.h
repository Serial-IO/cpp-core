#pragma once

#include "../../detail/sequential_dispatch.h"
#include "../serial_set_write_callback.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @copydoc serialSetWriteCallback
     * @note Sequential variant: guarantees execution in the exact order the calls were made across threads.
     */
    inline MODULE_API void serialSetWriteCallbackSequential(void (*callback_fn)(int bytes_written))
    {
        cpp_core::detail::seq::call([=] { serialSetWriteCallback(callback_fn); });
    }

#ifdef __cplusplus
}
#endif
