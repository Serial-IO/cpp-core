#pragma once
#include "version.h"

#include <cstdint>

#if defined(_WIN32) || defined(__CYGWIN__)
#ifdef cpp_windows_bindings_EXPORTS
#define MODULE_API __declspec(dllexport)
#else
#define MODULE_API __declspec(dllimport)
#endif
#else
#define MODULE_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C"
{
#endif
    /**
     * @brief Copy the compile-time version of the cpp_core library.
     *
     * Writes the version baked into the library at build-time to the structure
     * pointed to by @p out. If @p out is `nullptr` the call is a no-op.
     *
     * @param[out] out Pointer to a `cpp_core::Version` structure that receives
     *                 the version information. May be `nullptr`.
     */
    inline MODULE_API void getVersion(cpp_core::Version *out)
    {
        if (out != nullptr)
        {
            *out = cpp_core::kVersion;
        }
    }

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

    /**
     * @brief Close a previously opened serial port.
     *
     * The handle becomes invalid after the call. Passing an already invalid
     * (≤ 0) handle is a no-op.
     *
     * @param handle Handle obtained from serialOpen().
     * @return       0 on success or a negative error code on failure.
     */
    MODULE_API auto serialClose(int64_t handle) -> int;

    /**
     * @brief Read raw bytes from the serial port.
     *
     * The call blocks for at most @p timeout_ms milliseconds while waiting for
     * the FIRST byte. For every subsequent byte the individual timeout is
     * calculated as `timeout_ms * multiplier`.
     *
     * @param handle       Port handle.
     * @param buffer       Destination buffer (must not be `nullptr`).
     * @param buffer_size  Size of @p buffer in bytes (> 0).
     * @param timeout_ms   Base timeout per byte in milliseconds (applied to the
     *                     first byte as-is; subsequent bytes use
     *                     `timeout_ms * multiplier`).
     * @param multiplier   Factor applied to @p timeout_ms for every byte after
     *                     the first. 0 -> return immediately after the first byte.
     * @return             Bytes read (0 on timeout) or a negative error code.
     */
    MODULE_API auto serialRead(
        int64_t handle,
        void   *buffer,
        int     buffer_size,
        int     timeout_ms,
        int     multiplier
    ) -> int;

    /**
     * @brief Read bytes until a terminator character appears.
     *
     * Semantics are identical to serialRead() but reading stops as soon as the
     * byte pointed to by @p until_char has been received. The terminator is part
     * of the returned data.
     *
     * @param handle       Port handle.
     * @param buffer       Destination buffer.
     * @param buffer_size  Capacity of @p buffer in bytes.
     * @param timeout_ms   Base timeout per byte in milliseconds (applied to the
     *                     first byte as-is; each additional byte uses
     *                     `timeout_ms * multiplier`).
     * @param multiplier   Factor applied to the timeout for every additional byte.
     * @param until_char   Pointer to the terminator character (must not be `nullptr`).
     * @return             Bytes read (including the terminator), 0 on timeout
     *                     or a negative error code.
     */
    MODULE_API auto serialReadUntil(
        int64_t handle,
        void   *buffer,
        int     buffer_size,
        int     timeout_ms,
        int     multiplier,
        void   *until_char
    ) -> int;

    /**
     * @brief Write raw bytes to the serial port.
     *
     * Timeout handling mirrors serialRead(): @p timeout_ms applies to the first
     * byte, `timeout_ms * multiplier` to every subsequent one.
     *
     * @param handle       Port handle.
     * @param buffer       Data to transmit (must not be `nullptr`).
     * @param buffer_size  Number of bytes in @p buffer (> 0).
     * @param timeout_ms   Base timeout per byte in milliseconds (applied to the
     *                     first byte as-is; subsequent bytes use
     *                     `timeout_ms * multiplier`).
     * @param multiplier   Factor applied to the timeout for subsequent bytes.
     * @return             Bytes written (may be 0 on timeout) or a negative error code.
     */
    MODULE_API auto serialWrite(
        int64_t     handle,
        const void *buffer,
        int         buffer_size,
        int         timeout_ms,
        int         multiplier
    ) -> int;

    /**
     * @brief Enumerate all available serial ports on the system.
     *
     * The supplied callback is invoked once for every discovered port. All string
     * parameters may be `nullptr` if the information is unknown.
     *
     * @param function Callback receiving port information.
     * @return         Number of ports found or a negative error code.
     */
    MODULE_API auto serialGetPortsInfo(
        void (*function)(
            const char *port,
            const char *path,
            const char *manufacturer,
            const char *serial_number,
            const char *pnp_id,
            const char *location_id,
            const char *product_id,
            const char *vendor_id
        )
    ) -> int;

    /**
     * @brief Clear (flush) the device's input buffer.
     *
     * Discards every byte the driver has already received but the application
     * has not yet read.
     *
     * @param handle Port handle.
     */
    MODULE_API void serialClearBufferIn(int64_t handle);

    /**
     * @brief Clear (flush) the device's output buffer.
     *
     * Blocks until all queued bytes have been transmitted and then discards any
     * remaining data.
     *
     * @param handle Port handle.
     */
    MODULE_API void serialClearBufferOut(int64_t handle);

    /**
     * @brief Abort a blocking read operation running in a different thread.
     *
     * The target read function returns immediately with
     * ::cpp_core::StatusCodes::kAbortReadError.
     *
     * @param handle Port handle.
     */
    MODULE_API void serialAbortRead(int64_t handle);

    /**
     * @brief Abort a blocking write operation running in a different thread.
     *
     * The target write function returns immediately with
     * ::cpp_core::StatusCodes::kAbortWriteError.
     *
     * @param handle Port handle.
     */
    MODULE_API void serialAbortWrite(int64_t handle);

    /**
     * @brief Register a callback that is invoked whenever bytes are read.
     *
     * Pass `nullptr` to disable the callback.
     *
     * @param callback Function receiving the number of bytes that have just been read.
     */
    MODULE_API void serialSetReadCallback(void (*callback)(int bytes_read));

    /**
     * @brief Register a callback that is invoked whenever bytes are written.
     *
     * Pass `nullptr` to disable the callback.
     *
     * @param callback Function receiving the number of bytes that have just been written.
     */
    MODULE_API void serialSetWriteCallback(void (*callback)(int bytes_written));

    /**
     * @brief Register a callback that is invoked whenever an error occurs.
     *
     * Pass `nullptr` to disable the callback.
     *
     * @param callback Function receiving the error code and a textual description.
     */
    MODULE_API void serialSetErrorCallback(
        void (*callback)(
            int         error_code,
            const char *message
        )
    );

    /**
     * @brief Read a single line terminated by '\n'.
     *
     * Timeout handling is identical to serialRead(); the newline character is
     * included in the returned data.
     *
     * @param handle       Port handle.
     * @param buffer       Destination buffer.
     * @param buffer_size  Capacity of @p buffer in bytes.
     * @param timeout_ms   Base timeout per byte in milliseconds (applied to the
     *                     first byte as-is; subsequent bytes use
     *                     `timeout_ms * multiplier`).
     * @param multiplier   Factor applied to the timeout for subsequent bytes.
     * @return             Bytes read (0 on timeout) or a negative error code.
     */
    MODULE_API auto serialReadLine(
        int64_t handle,
        void   *buffer,
        int     buffer_size,
        int     timeout_ms,
        int     multiplier
    ) -> int;

    /**
     * @brief Write a buffer followed by a '\n' byte.
     *
     * Convenience wrapper that adds the line-feed for you and then calls
     * `serialWrite()`.
     *
     * @param handle       Port handle.
     * @param buffer       Data to transmit.
     * @param buffer_size  Number of bytes in @p buffer.
     * @param timeout_ms   Base timeout per byte in milliseconds.
     * @param multiplier   Factor applied to the timeout for bytes after the
     *                     first.
     * @return             Bytes written (including the newline) or a negative
     *                     error code.
     */
    MODULE_API auto serialWriteLine(
        int64_t     handle,
        const void *buffer,
        int         buffer_size,
        int         timeout_ms,
        int         multiplier
    ) -> int;

    /**
     * @brief Read until a specific byte sequence appears.
     *
     * Works like serialReadUntil() but supports an arbitrary terminator string.
     * The terminator is included in the returned data.
     *
     * @param handle       Port handle.
     * @param buffer       Destination buffer.
     * @param buffer_size  Capacity of @p buffer in bytes.
     * @param timeout_ms   Base timeout per byte in milliseconds (first byte
     *                     uses this value; each additional byte uses
     *                     `timeout_ms * multiplier`).
     * @param multiplier   Factor applied to the timeout for subsequent bytes.
     * @param sequence     Pointer to the terminating byte sequence (must not be `nullptr`).
     * @return             Bytes read (including the terminator) or a negative error code.
     */
    MODULE_API auto serialReadUntilSequence(
        int64_t handle,
        void   *buffer,
        int     buffer_size,
        int     timeout_ms,
        int     multiplier,
        void   *sequence
    ) -> int;

    /**
     * @brief Total number of bytes transmitted since the port was opened.
     *
     * @param handle Port handle.
     * @return       Total number of bytes written.
     */
    MODULE_API auto serialOutBytesTotal(int64_t handle) -> int64_t;

    /**
     * @brief Total number of bytes received since the port was opened.
     *
     * @param handle Port handle.
     * @return       Total number of bytes read.
     */
    MODULE_API auto serialInBytesTotal(int64_t handle) -> int64_t;

    /**
     * @brief Wait until the operating-system driver has physically sent every queued byte.
     *
     * The function blocks the *calling thread* until the device driver reports
     * that the transmit FIFO is **empty** – i.e. all bytes handed to previous
     * `serialWrite*` calls have been shifted out on the wire.  It does *not*
     * flush higher-level protocol buffers you may have implemented yourself.
     *
     * Typical use-case: ensure a complete command frame has left the UART
     * before toggling RTS/DTR or powering down the device.
     *
     * @code{.c}
     * // Send a frame and make sure it actually hits the line
     * serialWrite(h, frame, frame_len, 50, 1);
     * if (serialDrain(h) < 0) {
     *     fprintf(stderr, "drain failed\n");
     * }
     * @endcode
     *
     * @param handle Port handle.
     * @return       0 on success; negative ::cpp_core::StatusCodes value on error.
     */
    MODULE_API auto serialDrain(int64_t handle) -> int;

    /**
     * @brief Query how many bytes can be read *immediately* without blocking.
     *
     * The number reflects the size of the driver's RX FIFO **after** accounting
     * for data already consumed by the application.  A value of `0` therefore
     * means a read call would have to wait for the next byte to arrive.
     *
     * @code{.c}
     * int pending = serialInBytesWaiting(h);
     * if (pending > 0) {
     *     serialRead(h, buf, pending, 0, 1); // non-blocking read
     * }
     * @endcode
     *
     * @param handle Port handle.
     * @return       Bytes available for instant reading or a negative error code.
     */
    MODULE_API auto serialInBytesWaiting(int64_t handle) -> int;

    /**
     * @brief Return the number of bytes that have been accepted by the driver but not yet sent.
     *
     * Useful for gauging transmission progress in the background or for pacing
     * further writes to avoid unbounded buffering.
     *
     * @code{.c}
     * while (serialOutBytesWaiting(h) > 0) {
     *     usleep(1000); // wait 1 ms and poll again
     * }
     * @endcode
     *
     * @param handle Port handle.
     * @return       Bytes still waiting in the TX FIFO or a negative error code.
     */
    MODULE_API auto serialOutBytesWaiting(int64_t handle) -> int;

#ifdef __cplusplus
}
#endif
