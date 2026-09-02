#pragma once
#include "../error_callback.h"
#include "../module_api.h"
#include "../strong_types.hpp"
#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Configure the flow-control mode for an open serial port.
     *
     * Flow control prevents buffer overruns when one side is slower than the
     * other. Three modes are supported:
     *
     * | @p flow_mode                 | Meaning                                      |
     * |------------------------------|----------------------------------------------|
     * | `FlowControl::kNone`         | No flow control.                             |
     * | `FlowControl::kRtsCts`       | Hardware RTS/CTS flow control.               |
     * | `FlowControl::kXonXoff`      | Software XON/XOFF flow control.              |
     *
     * Changing the mode on an already-open port takes effect immediately.
     *
     * @param handle Port handle obtained from serialOpen().
     * @param flow_mode Flow-control mode.
     * @param error_callback [optional] Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`.
     * @return 0 on success or a negative error code from ::cpp_core::StatusCode on error.
     */
    MODULE_API auto serialSetFlowControl(int64_t handle, cpp_core::FlowControl flow_mode,
                                         ErrorCallbackT error_callback = nullptr) -> int;

#ifdef __cplusplus
}
#endif
