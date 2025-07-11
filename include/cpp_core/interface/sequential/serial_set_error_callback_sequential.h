#pragma once

#include "../../detail/sequential_dispatch.h"
#include "../../error_callback.h"
#include "../serial_set_error_callback.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @copydoc serialSetErrorCallback
     * @note Sequential variant: guarantees execution in the exact order the calls were made across threads.
     */
    inline MODULE_API void serialSetErrorCallbackSequential(ErrorCallbackT error_callback)
    {
        cpp_core::detail::seq::call([=] { serialSetErrorCallback(error_callback); });
    }

#ifdef __cplusplus
}
#endif
