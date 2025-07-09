#pragma once

namespace cpp_core
{
enum class StatusCodes
{
    kSuccess             = 0,
    kCloseHandleError    = -1,
    kInvalidHandleError  = -2,
    kReadError           = -3,
    kWriteError          = -4,
    kGetStateError       = -5,
    kSetStateError       = -6,
    kSetTimeoutError     = -7,
    kBufferError         = -8,
    kNotFoundError       = -9,
    kClearBufferInError  = -10,
    kClearBufferOutError = -11,
    kAbortReadError      = -12,
    kAbortWriteError     = -13,
};
} // namespace cpp_core
