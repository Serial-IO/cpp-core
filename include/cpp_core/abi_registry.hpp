#pragma once

#include "serial.h"

#include <meta>

namespace cpp_core::detail
{

struct AbiFunctionRegistry
{
    std::meta::info getVersion = ^^::getVersion;
    std::meta::info serialAbortRead = ^^::serialAbortRead;
    std::meta::info serialAbortWrite = ^^::serialAbortWrite;
    std::meta::info serialClearBufferIn = ^^::serialClearBufferIn;
    std::meta::info serialClearBufferOut = ^^::serialClearBufferOut;
    std::meta::info serialClose = ^^::serialClose;
    std::meta::info serialDrain = ^^::serialDrain;
    std::meta::info serialGetBaudrate = ^^::serialGetBaudrate;
    std::meta::info serialGetCts = ^^::serialGetCts;
    std::meta::info serialGetDataBits = ^^::serialGetDataBits;
    std::meta::info serialGetDcd = ^^::serialGetDcd;
    std::meta::info serialGetDsr = ^^::serialGetDsr;
    std::meta::info serialGetFlowControl = ^^::serialGetFlowControl;
    std::meta::info serialGetParity = ^^::serialGetParity;
    std::meta::info serialGetRi = ^^::serialGetRi;
    std::meta::info serialGetStopBits = ^^::serialGetStopBits;
    std::meta::info serialInBytesTotal = ^^::serialInBytesTotal;
    std::meta::info serialInBytesWaiting = ^^::serialInBytesWaiting;
    std::meta::info serialListPorts = ^^::serialListPorts;
    std::meta::info serialMonitorPorts = ^^::serialMonitorPorts;
    std::meta::info serialOpen = ^^::serialOpen;
    std::meta::info serialOutBytesTotal = ^^::serialOutBytesTotal;
    std::meta::info serialOutBytesWaiting = ^^::serialOutBytesWaiting;
    std::meta::info serialRead = ^^::serialRead;
    std::meta::info serialReadLine = ^^::serialReadLine;
    std::meta::info serialReadUntil = ^^::serialReadUntil;
    std::meta::info serialReadUntilSequence = ^^::serialReadUntilSequence;
    std::meta::info serialSendBreak = ^^::serialSendBreak;
    std::meta::info serialSetBaudrate = ^^::serialSetBaudrate;
    std::meta::info serialSetDataBits = ^^::serialSetDataBits;
    std::meta::info serialSetDtr = ^^::serialSetDtr;
    std::meta::info serialSetErrorCallback = ^^::serialSetErrorCallback;
    std::meta::info serialSetFlowControl = ^^::serialSetFlowControl;
    std::meta::info serialSetParity = ^^::serialSetParity;
    std::meta::info serialSetReadCallback = ^^::serialSetReadCallback;
    std::meta::info serialSetRts = ^^::serialSetRts;
    std::meta::info serialSetStopBits = ^^::serialSetStopBits;
    std::meta::info serialSetWriteCallback = ^^::serialSetWriteCallback;
    std::meta::info serialWrite = ^^::serialWrite;
};

} // namespace cpp_core::detail
