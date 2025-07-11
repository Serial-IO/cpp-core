#pragma once

#include "../../detail/sequential_dispatch.h"
#include "../../error_callback.h"
#include "../serial_open.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @copydoc serialOpen
     * @note Sequential variant: guarantees execution in the exact order the calls were made across threads.
     */
    inline MODULE_API auto serialOpenSequential(
        void          *port,
        int            baudrate,
        int            data_bits,
        int            parity         = 0,
        int            stop_bits      = 0,
        ErrorCallbackT error_callback = nullptr
    ) -> intptr_t
    {
        return cpp_core::detail::seq::call([=] {
            return serialOpen(port, baudrate, data_bits, parity, stop_bits, error_callback);
        });
    }

#ifdef __cplusplus
}
#endif
