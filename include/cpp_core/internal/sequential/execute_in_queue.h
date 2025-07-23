#pragma once

#include "dispatch_state.h"
#include "ensure_worker_running.h"
#include <future>
#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>

namespace cpp_core::internal::sequential
{
/**
 * @brief Queues the given callable in @p state and returns its result.
 *
 * The function first makes sure a worker thread is active via
 * ensureWorkerRunning(). It then enqueues the callable and blocks until the
 * callable has completed, forwarding the return value (if any).
 *
 * @tparam FunctionT Type of the callable object.
 * @param state DispatchState that owns the queue and synchronisation primitives.
 * @param function Callable object to execute. Can return any type.
 * @return Result produced by @p function or void if it returns void.
 */
template <typename FunctionT>
auto executeInQueue(
    DispatchState &state,
    FunctionT    &&function
) -> decltype(function())
{
    using FunctionReturnT = decltype(function());

    auto task_ptr = std::make_shared<std::packaged_task<FunctionReturnT()>>(std::forward<FunctionT>(function));
    auto future   = task_ptr->get_future();

    ensureWorkerRunning(state);

    {
        std::lock_guard<std::mutex> lock(state.mutex);
        state.queue.emplace([task_ptr]() { (*task_ptr)(); });
    }
    state.condition_variable.notify_one();

    if constexpr (std::is_void_v<FunctionReturnT>)
    {
        future.get();
    }
    else
    {
        return future.get();
    }
}
} // namespace cpp_core::internal::sequential
