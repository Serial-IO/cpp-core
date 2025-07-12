#pragma once

#include "../../detail/sequential_dispatch.h"
#include "../../module_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Select the sequential execution mode.
     *
     * Passing `1` enables a dedicated queue per serial handle (concurrent
     * execution across ports). Passing `0` restores the legacy behavior of a
     * single global queue (strict sequential execution across all ports).
     *
     * @param per_handle Non-zero -> per-handle mode, 0 -> global queue.
     */
    inline MODULE_API void serialSetSequentialQueueMode(int per_handle)
    {
        cpp_core::detail::seq::setQueueMode(
            (per_handle != 0) ? cpp_core::detail::seq::QueueMode::kPerHandle : cpp_core::detail::seq::QueueMode::kGlobal
        );
    }

#ifdef __cplusplus
} // extern "C"
#endif
