#pragma once
#include "../error_callback.h"
#include "../module_api.h"
#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Send a break condition on the serial line.
     *
     * A break is a sustained logic-LOW that lasts longer than a normal
     * character frame. Many protocols rely on it:
     *
     * - **DMX512**: A break of >= 88 us marks the start of a new frame.
     * - **LIN bus**: The master starts each frame with a 13-bit break.
     * - **MODBUS RTU**: Some implementations use break for frame sync.
     *
     * The @p duration_ms parameter is a *minimum* - the actual break may be
     * slightly longer due to OS scheduling.
     *
     * @param handle Port handle obtained from serialOpen().
     * @param duration_ms Break duration in milliseconds (> 0).
     * @param error_callback [optional] Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`.
     * @return 0 on success or a negative error code from ::cpp_core::StatusCodes on error.
     */
    MODULE_API auto serialSendBreak(int64_t handle, int duration_ms, ErrorCallbackT error_callback = nullptr) -> int;

#ifdef __cplusplus
}
#endif
