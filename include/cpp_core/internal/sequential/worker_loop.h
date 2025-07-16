#pragma once

#include "dispatch_state.h"
#include <condition_variable>
#include <functional>
#include <mutex>
#include <utility>

namespace cpp_core::internal::sequential
{
/**
 * @brief Blocking loop that continuously processes jobs from @p state.
 *
 * Intended to run on a background thread created by ensureWorkerRunning().
 * The loop waits on a condition variable until a job is available, executes
 * it and then repeats indefinitely.
 *
 * @param state DispatchState providing the job queue and synchronisation primitives.
 */
inline void workerLoop(DispatchState &state)
{
    for (;;)
    {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(state.mutex);
            state.condition_variable.wait(lock, [&state] { return !state.queue.empty(); });
            job = std::move(state.queue.front());
            state.queue.pop();
        }
        job();
    }
}
} // namespace cpp_core::internal::sequential
