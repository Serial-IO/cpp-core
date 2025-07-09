#pragma once
#include "../module_api.h"
#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Open and configure a serial port.
     *
     * The function attempts to open the device referenced by @p port and applies
     * the given line settings. The pointer is interpreted as
     *   - `const char*` on POSIX and
     *   - `const wchar_t*` on Windows.
     *
     * @param port       Null-terminated device identifier (e.g. "COM3",
     *                   "/dev/ttyUSB0"). Passing `nullptr` results in
     *                   ::cpp_core::StatusCodes::kNotFoundError.
     * @param baudrate   Desired baud rate in bit/s (≥ 300).
     * @param data_bits  Number of data bits (5–8).
     * @param parity     0 = none, 1 = even, 2 = odd.
     * @param stop_bits  0 = 1 stop bit, 2 = 2 stop bits.
     * @return           A positive opaque handle on success or a negative value
     *                   from ::cpp_core::StatusCodes on failure.
     */
    MODULE_API auto serialOpen(
        void *port,
        int   baudrate,
        int   data_bits,
        int   parity    = 0,
        int   stop_bits = 0
    ) -> intptr_t;

#ifdef __cplusplus
}
#endif
