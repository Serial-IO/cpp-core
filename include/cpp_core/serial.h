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

    // Inline definition to keep the library header-only
    inline MODULE_API void getVersion(cpp_core::Version *out)
    {
        if (out != nullptr)
        {
            *out = cpp_core::kVersion;
        }
    }

    // Basic serial API
    MODULE_API auto serialOpen(void *port, int baudrate, int data_bits, int parity /*0-none,1-even,2-odd*/ = 0,
                               int stop_bits /*0-1bit,2-2bit*/ = 0) -> intptr_t;

    MODULE_API void serialClose(int64_t handle);

    MODULE_API auto serialRead(int64_t handle, void *buffer, int buffer_size, int timeout_ms, int multiplier) -> int;

    MODULE_API auto serialReadUntil(int64_t handle, void *buffer, int buffer_size, int timeout_ms, int multiplier,
                                    void *until_char) -> int;

    MODULE_API auto serialWrite(int64_t handle, const void *buffer, int buffer_size, int timeout_ms, int multiplier)
        -> int;

    // Enumerate ports; callback gets simple COM name first (e.g. "COM3"),
    // followed by the full device path and further meta-data.
    MODULE_API auto serialGetPortsInfo(void (*function)(const char *port, const char *path, const char *manufacturer,
                                                        const char *serial_number, const char *pnp_id,
                                                        const char *location_id, const char *product_id,
                                                        const char *vendor_id)) -> int;

    MODULE_API void serialClearBufferIn(int64_t handle);
    MODULE_API void serialClearBufferOut(int64_t handle);
    MODULE_API void serialAbortRead(int64_t handle);
    MODULE_API void serialAbortWrite(int64_t handle);

    // Optional callback hooks (can be nullptr)
    extern void (*on_read_callback)(int bytes_read);
    extern void (*on_write_callback)(int bytes_written);
    extern void (*on_error_callback)(int error_code, const char *message);

    MODULE_API void serialOnRead(void (*func)(int bytes_read));
    MODULE_API void serialOnWrite(void (*func)(int bytes_written));
    MODULE_API void serialOnError(void (*func)(int error_code, const char *message));

    MODULE_API auto serialReadLine(int64_t handle, void *buffer, int buffer_size, int timeout_ms) -> int;

    MODULE_API auto serialWriteLine(int64_t handle, const void *buffer, int buffer_size, int timeout_ms) -> int;

    MODULE_API auto serialReadUntilSequence(int64_t handle, void *buffer, int buffer_size, int timeout_ms,
                                            void *sequence) -> int;

    MODULE_API auto serialReadFrame(int64_t handle, void *buffer, int buffer_size, int timeout_ms, char start_byte,
                                    char end_byte) -> int;

    // Byte statistics
    MODULE_API auto serialOutBytesTotal(int64_t handle) -> int64_t;
    MODULE_API auto serialInBytesTotal(int64_t handle) -> int64_t;

    // Drain pending TX bytes (wait until sent)
    MODULE_API auto serialDrain(int64_t handle) -> int;

    // Bytes currently queued in the driver buffers
    MODULE_API auto serialInBytesWaiting(int64_t handle) -> int;
    MODULE_API auto serialOutBytesWaiting(int64_t handle) -> int;

#ifdef __cplusplus
}
#endif
