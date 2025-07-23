#pragma once

#include "../../error_callback.h"
#include "../../internal/sequential/call.h"
#include "../serial_read_until_sequence.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @copydoc serialReadUntilSequence
     * @note Sequential variant: guarantees execution in the exact order the calls were made across threads.
     */
    inline MODULE_API auto serialReadUntilSequenceSequential(
        int64_t        handle,
        void          *buffer,
        int            buffer_size,
        int            timeout_ms,
        int            multiplier,
        void          *sequence,
        ErrorCallbackT error_callback = nullptr
    ) -> int
    {
        return cpp_core::internal::sequential::call(handle, [=] {
            return serialReadUntilSequence(
                handle, buffer, buffer_size, timeout_ms, multiplier, sequence, error_callback
            );
        });
    }

#ifdef __cplusplus
}
#endif
