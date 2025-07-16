#pragma once

#include "dispatch_state.h"
#include "worker_loop.h"
#include <mutex>
#include <thread>

namespace cpp_core::internal::sequential
{
/**
 * @brief Starts the worker thread for @p state if it is not already running.
 *
 * The worker thread executes workerLoop() on the given DispatchState. A
 * call_once flag inside the state guarantees that the thread is only created
 * the first time this function is invoked.
 *
 * @param state DispatchState whose queue should be processed by the worker.
 */
inline void ensureWorkerRunning(DispatchState &state)
{
    std::call_once(state.worker_started, [&state] { std::thread([&state] { workerLoop(state); }).detach(); });
}
} // namespace cpp_core::internal::sequential
