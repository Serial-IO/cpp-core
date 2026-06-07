#pragma once

#define CPP_CORE_STATUS_CODE_CONFIGURATION_LIST(X, CategoryName)                                                      \
    X(CategoryName, 0, SetBaudrateError)                                                                              \
    X(CategoryName, 1, SetDataBitsError)                                                                              \
    X(CategoryName, 2, SetParityError)                                                                                \
    X(CategoryName, 3, SetStopBitsError)                                                                              \
    X(CategoryName, 4, SetFlowControlError)                                                                           \
    X(CategoryName, 5, SetTimeoutError)

#define CPP_CORE_STATUS_CODE_CONNECTION_LIST(X, CategoryName)                                                         \
    X(CategoryName, 0, NotFoundError)                                                                                 \
    X(CategoryName, 1, InvalidHandleError)                                                                            \
    X(CategoryName, 2, CloseHandleError)

#define CPP_CORE_STATUS_CODE_IO_LIST(X, CategoryName)                                                                 \
    X(CategoryName, 0, ReadError)                                                                                     \
    X(CategoryName, 1, WriteError)                                                                                    \
    X(CategoryName, 2, AbortReadError)                                                                                \
    X(CategoryName, 3, AbortWriteError)                                                                               \
    X(CategoryName, 4, BufferError)                                                                                   \
    X(CategoryName, 5, ClearBufferInError)                                                                            \
    X(CategoryName, 6, ClearBufferOutError)

#define CPP_CORE_STATUS_CODE_CONTROL_LIST(X, CategoryName)                                                            \
    X(CategoryName, 0, SetDtrError)                                                                                   \
    X(CategoryName, 1, SetRtsError)                                                                                   \
    X(CategoryName, 2, GetModemStatusError)                                                                           \
    X(CategoryName, 3, SendBreakError)                                                                                \
    X(CategoryName, 4, GetStateError)                                                                                 \
    X(CategoryName, 5, SetStateError)

#define CPP_CORE_STATUS_CODE_MONITOR_LIST(X, CategoryName)                                                            \
    X(CategoryName, 0, MonitorError)

#define CPP_CORE_STATUS_CODE_CATEGORY_LIST(X)                                                                         \
    X(Configuration, 1, CPP_CORE_STATUS_CODE_CONFIGURATION_LIST)                                                      \
    X(Connection, 2, CPP_CORE_STATUS_CODE_CONNECTION_LIST)                                                            \
    X(Io, 3, CPP_CORE_STATUS_CODE_IO_LIST)                                                                            \
    X(Control, 4, CPP_CORE_STATUS_CODE_CONTROL_LIST)                                                                  \
    X(Monitor, 5, CPP_CORE_STATUS_CODE_MONITOR_LIST)
