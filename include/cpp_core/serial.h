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
     * @brief Retrieve the compile-time version number of the cpp_core library.
     *
     * Fills the provided Version structure with the current compile-time version
     * of the library. If the pointer is nullptr the function returns immediately
     * without performing any action.
     *
     * @param[out] out Pointer to a cpp_core::Version object that will receive the
     *                 version information.
     */
    inline MODULE_API void getVersion(cpp_core::Version *out)
    {
        if (out != nullptr)
        {
            *out = cpp_core::kVersion;
        }
    }

    /**
     * @brief Open a serial port with the given parameters.
     *
     * @param port        Platform-specific identifier of the serial port (e.g.
     *                    "COM3" on Windows or "/dev/ttyUSB0" on POSIX systems).
     * @param baudrate    Desired baud rate in bits per second.
     * @param data_bits   Number of data bits (typically 5–8).
     * @param parity      Parity mode: 0 = none, 1 = even, 2 = odd.
     * @param stop_bits   Stop-bit configuration: 0 = 1 stop bit, 2 = 2 stop bits.
     * @return            Opaque handle to the opened port on success; a negative
     *                    value indicates failure.
     */
    MODULE_API auto serialOpen(void *port, int baudrate, int data_bits, int parity = 0, int stop_bits = 0) -> intptr_t;

    /**
     * @brief Close a previously opened serial port.
     *
     * @param handle Handle returned by serialOpen().
     */
    MODULE_API void serialClose(int64_t handle);

    /**
     * @brief Read bytes from the serial port.
     *
     * Attempts to read up to @p buffer_size bytes into @p buffer. The call blocks
     * for at most @p timeout_ms for the first byte and for @p timeout_ms *
     * @p multiplier for subsequent bytes.
     *
     * @param handle       Port handle returned by serialOpen().
     * @param buffer       Destination buffer that will receive the data.
     * @param buffer_size  Size of @p buffer in bytes.
     * @param timeout_ms   Timeout for the first byte in milliseconds.
     * @param multiplier   Factor applied to the timeout for each additional byte.
     * @return             Number of bytes actually read, or a negative value on
     *                     error.
     */
    MODULE_API auto serialRead(int64_t handle, void *buffer, int buffer_size, int timeout_ms, int multiplier) -> int;

    /**
     * @brief Read bytes until a specific terminator character is encountered.
     *
     * Behaves like serialRead() but stops reading when the character pointed to by
     * @p until_char is read (the terminator is included in the output).
     *
     * @param handle       Port handle returned by serialOpen().
     * @param buffer       Destination buffer.
     * @param buffer_size  Size of the destination buffer in bytes.
     * @param timeout_ms   Timeout for the first byte in milliseconds.
     * @param multiplier   Factor applied to the timeout for subsequent bytes.
     * @param until_char   Pointer to the terminator character.
     * @return             Number of bytes read (including the terminator), or a
     *                     negative value on error.
     */
    MODULE_API auto serialReadUntil(int64_t handle, void *buffer, int buffer_size, int timeout_ms, int multiplier,
                                    void *until_char) -> int;

    /**
     * @brief Write raw bytes to the serial port.
     *
     * @param handle       Port handle returned by serialOpen().
     * @param buffer       Pointer to the data to transmit.
     * @param buffer_size  Number of bytes to send.
     * @param timeout_ms   Timeout for the first byte in milliseconds.
     * @param multiplier   Factor applied to the timeout for each additional byte.
     * @return             Number of bytes actually written, or a negative value on
     *                     error.
     */
    MODULE_API auto serialWrite(int64_t handle, const void *buffer, int buffer_size, int timeout_ms, int multiplier)
        -> int;

    /**
     * @brief Enumerate all available serial ports on the system.
     *
     * The supplied callback is invoked once for every discovered port and receives
     * detailed information about the device.
     *
     * @param function Callback that is called for each port. The parameters are:
     *                 - port:           Short port name (e.g. "COM3").
     *                 - path:           Full device path.
     *                 - manufacturer:   Manufacturer string (may be nullptr).
     *                 - serial_number:  Device serial number (may be nullptr).
     *                 - pnp_id:         Plug-and-play identifier (may be nullptr).
     *                 - location_id:    Device location (may be nullptr).
     *                 - product_id:     USB product ID string (may be nullptr).
     *                 - vendor_id:      USB vendor ID string (may be nullptr).
     * @return         Number of ports found; a negative value indicates error.
     */
    MODULE_API auto serialGetPortsInfo(void (*function)(const char *port, const char *path, const char *manufacturer,
                                                        const char *serial_number, const char *pnp_id,
                                                        const char *location_id, const char *product_id,
                                                        const char *vendor_id)) -> int;

    /**
     * @brief Clear (flush) the device's input buffer.
     *
     * @param handle Port handle.
     */
    MODULE_API void serialClearBufferIn(int64_t handle);

    /**
     * @brief Clear (flush) the device's output buffer.
     *
     * @param handle Port handle.
     */
    MODULE_API void serialClearBufferOut(int64_t handle);

    /**
     * @brief Abort a blocking read operation running on another thread.
     *
     * @param handle Port handle.
     */
    MODULE_API void serialAbortRead(int64_t handle);

    /**
     * @brief Abort a blocking write operation running on another thread.
     *
     * @param handle Port handle.
     */
    MODULE_API void serialAbortWrite(int64_t handle);

    /**
     * @brief Register a callback that is invoked whenever bytes are read.
     *
     * Passing nullptr disables the callback.
     *
     * @param callback Pointer to a function receiving the number of bytes that
     *                 have been read.
     */
    MODULE_API void serialSetReadCallback(void (*callback)(int bytes_read));

    /**
     * @brief Register a callback that is invoked whenever bytes are written.
     *
     * Passing nullptr disables the callback.
     *
     * @param callback Pointer to a function receiving the number of bytes that
     *                 have been written.
     */
    MODULE_API void serialSetWriteCallback(void (*callback)(int bytes_written));

    /**
     * @brief Register a callback that is invoked whenever an error occurs.
     *
     * Passing nullptr disables the callback.
     *
     * @param callback Pointer to a function receiving an error code and a
     *                 textual description.
     */
    MODULE_API void serialSetErrorCallback(void (*callback)(int error_code, const char *message));

    /**
     * @brief Read a line terminated by a newline character ("\n"). This function keeps reading until the newline character is found or the timeout is reached.
     *
     * @param handle       Port handle.
     * @param buffer       Destination buffer.
     * @param buffer_size  Size of the destination buffer in bytes.
     * @param timeout_ms   Maximum time to wait for the line in milliseconds.
     * @return             Number of bytes read (including the newline), or a
     *                     negative value on error.
     */
    MODULE_API auto serialReadLine(int64_t handle, void *buffer, int buffer_size, int timeout_ms) -> int;

    /**
     * @brief Write a buffer followed by a newline character ("\n").
     *
     * @param handle       Port handle.
     * @param buffer       Data to transmit.
     * @param buffer_size  Number of bytes in @p buffer.
     * @param timeout_ms   Maximum time to wait for the transmission in
     *                     milliseconds.
     * @return             Number of bytes actually written, or a negative value on
     *                     error.
     */
    MODULE_API auto serialWriteLine(int64_t handle, const void *buffer, int buffer_size, int timeout_ms) -> int;

    /**
     * @brief Read until a specific byte sequence is encountered.
     *
     * @param handle       Port handle.
     * @param buffer       Destination buffer.
     * @param buffer_size  Size of the destination buffer in bytes.
     * @param timeout_ms   Timeout for the first byte in milliseconds.
     * @param sequence     Pointer to the byte sequence that terminates the read.
     * @return             Number of bytes read (including the terminating
     *                     sequence), or a negative value on error.
     */
    MODULE_API auto serialReadUntilSequence(int64_t handle, void *buffer, int buffer_size, int timeout_ms,
                                            void *sequence) -> int;

    /**
     * @brief Read a frame delimited by start and end bytes.
     *
     * @param handle       Port handle.
     * @param buffer       Destination buffer.
     * @param buffer_size  Size of the destination buffer in bytes.
     * @param timeout_ms   Maximum time to wait for the frame in milliseconds.
     * @param start_byte   Byte that marks the beginning of the frame.
     * @param end_byte     Byte that marks the end of the frame.
     * @return             Number of bytes in the frame (including delimiters), or
     *                     a negative value on error.
     */
    MODULE_API auto serialReadFrame(int64_t handle, void *buffer, int buffer_size, int timeout_ms, char start_byte,
                                    char end_byte) -> int;

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
     * @brief Block until all queued bytes have been transmitted.
     *
     * @param handle Port handle.
     * @return       0 on success, negative value on error.
     */
    MODULE_API auto serialDrain(int64_t handle) -> int;

    /**
     * @brief Number of bytes currently waiting in the driver's input buffer.
     *
     * @param handle Port handle.
     * @return       Number of bytes available for immediate reading.
     */
    MODULE_API auto serialInBytesWaiting(int64_t handle) -> int;

    /**
     * @brief Number of bytes currently waiting in the driver's output buffer.
     *
     * @param handle Port handle.
     * @return       Number of bytes pending transmission.
     */
    MODULE_API auto serialOutBytesWaiting(int64_t handle) -> int;

#ifdef __cplusplus
}
#endif
