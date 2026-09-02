#pragma once
#include "../error_callback.h"
#include "../module_api.h"
#include "../serial_config.hpp"
#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Open and configure a serial port.
     *
     * The function attempts to open the device referenced by @p port and applies
     * the line settings in @p config. @p port is interpreted as a UTF-8 encoded
     * null-terminated string on all platforms.
     *
     * @param port Null-terminated device identifier (e.g. "COM3", "/dev/ttyUSB0"). Passing `nullptr` results in
     * a failure.
     * @param config Serial line configuration. Includes baud rate, data bits,
     * parity, stop bits, and flow-control mode. Passing `nullptr` results in a failure.
     * @param error_callback [optional] Callback to invoke on error. Defined in error_callback.h. Default is `nullptr`.
     * @return A positive opaque handle on success or a negative value from ::cpp_core::StatusCode on failure.
     */
    MODULE_API auto serialOpen(const char *port, const cpp_core::SerialConfig *config,
                               ErrorCallbackT error_callback = nullptr) -> intptr_t;

#ifdef __cplusplus
}
#endif
