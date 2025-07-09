#pragma once
#include "../module_api.h"
#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Close a previously opened serial port.
     *
     * The handle becomes invalid after the call. Passing an already invalid
     * (≤ 0) handle is a no-op.
     *
     * @param handle Handle obtained from serialOpen().
     * @return 0 on success or a negative error code on failure.
     */
    MODULE_API auto serialClose(int64_t handle) -> int;

#ifdef __cplusplus
}
#endif
