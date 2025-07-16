#pragma once

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <queue>

namespace cpp_core::internal::sequential
{
/**
 * @brief Holds the queue and synchronisation primitives used by the sequential dispatch system.
 */
struct DispatchState
{
    // Mutex protecting queue + condition_variable.
    std::mutex mutex;

    // Condition variable to wake the worker thread when new jobs arrive.
    std::condition_variable condition_variable;

    // FIFO containing packaged thunks (jobs).
    std::queue<std::function<void()>> queue;

    // Indicates if the worker thread is already launched.
    std::once_flag worker_started;
};
} // namespace cpp_core::internal::sequential
