#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>

namespace cpp_core::detail::seq
{

struct DispatchState
{
    std::mutex                        mtx;
    std::condition_variable           cv;
    std::queue<std::function<void()>> queue;
    std::once_flag                    worker_started;
};

/**
 * @brief Access the global dispatcher state (singleton).
 *
 * Implements the C++11 *Meyers-Singleton* idiom: the static local variable is
 * initialised on first use in a thread-safe manner and subsequently reused.
 * This ensures exactly one queue / mutex / condition_variable exists per
 * process while avoiding the static-initialisation-order problem.
 *
 * @return Reference to the sole ::DispatchState instance.
 */
inline auto state() -> DispatchState &
{
    static DispatchState instance;
    return instance;
}

/**
 * @brief Worker thread main loop.
 *
 * Waits for new jobs in `state().queue` and executes them sequentially.
 * The loop never terminates, the thread lives for the entire lifetime of
 * the process.
 */
inline void worker()
{
    for (;;)
    {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(state().mtx);
            state().cv.wait(lock, [] { return !state().queue.empty(); });
            job = std::move(state().queue.front());
            state().queue.pop();
        }
        job();
    }
}

/**
 * @brief Starts the background worker thread (called once).
 *
 * Invoked internally by ::call via `std::call_once`. On the first invocation a
 * detached thread is spawned; subsequent calls are no-ops.
 */
inline void startWorker()
{
    std::thread(worker).detach();
}

/**
 * @brief Execute a callable sequentially on the background thread.
 *
 * The function/lambda is enqueued in FIFO order. The calling thread then
 * blocks until completion and returns the result (or propagates an exception).
 *
 * Steps:
 * 1. Wrap the callable in `std::packaged_task` to obtain an associated
 *    `std::future`.
 * 2. Ensure the worker thread is running (`std::call_once`).
 * 3. Push a copyable thunk into the queue and wake the worker via
 *    `notify_one()`.
 * 4. Wait on `future.get()` for completion and forward the return value.
 *
 * @tparam FunctionT Callable type with no parameters.
 * @param  function  Function object to be executed sequentially.
 * @return           The callable's return value (or `void`).
 */
template <typename FunctionT> auto call(FunctionT &&function) -> decltype(function())
{
    using FunctionReturnT = decltype(function());

    auto task_ptr = std::make_shared<std::packaged_task<FunctionReturnT()>>(std::forward<FunctionT>(function));
    auto future   = task_ptr->get_future();

    std::call_once(state().worker_started, startWorker);

    {
        std::lock_guard<std::mutex> lock(state().mtx);
        state().queue.emplace([task_ptr]() { (*task_ptr)(); });
    }
    state().cv.notify_one();

    if constexpr (std::is_void_v<FunctionReturnT>)
    {
        future.get();
    }
    else
    {
        return future.get();
    }
}

} // namespace cpp_core::detail::seq
