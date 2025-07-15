#pragma once

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <unordered_map>

namespace cpp_core::internal::seq
{

struct DispatchState
{
    /** Mutex protecting queue + condition_variable. */
    std::mutex mutex;

    /** Condition variable to wake the worker thread when new jobs arrive. */
    std::condition_variable condition_variable;

    /** FIFO containing packaged thunks (jobs). */
    std::queue<std::function<void()>> queue;

    /** Indicates if the worker thread is already launched. */
    std::once_flag worker_started;
};

namespace detail
{

/**
 * @brief Global map that stores a dedicated ::DispatchState for each serial handle encountered during runtime. The map
 * is lazily populated on first use of a handle.
 * @return Reference to the global map instance mapping handles to their dispatch states
 */
inline auto handleStates() -> std::unordered_map<
    int64_t,
    DispatchState> &
{
    static std::unordered_map<int64_t, DispatchState> instance;
    return instance;
}

/**
 * @brief Mutex guarding access to ::handleStates() (adds/removes look-ups).
 * @return Reference to the global mutex instance protecting handleStates()
 */
inline auto handleStatesMutex() -> std::mutex &
{
    static std::mutex instance;
    return instance;
}

/**
 * @brief Get (or create) the ::DispatchState associated with @p handle.
 *
 * @param handle Serial handle used as key.
 * @return Reference to the corresponding state object.
 */
inline auto stateForHandle(int64_t handle) -> DispatchState &
{
    std::lock_guard<std::mutex> lock(handleStatesMutex());
    return handleStates()[handle];
}

} // namespace detail

/**
 * @brief Execute @p function sequentially on the queue dedicated to @p handle.
 *
 * Each serial handle owns its own FIFO queue and background worker thread.
 * Calls executed on the same handle are strictly ordered, while calls on
 * different handles may run concurrently.
 *
 * @tparam FunctionT Zero-argument callable type.
 * @param handle Serial handle identifying the queue/worker.
 * @param function Callable object to execute.
 * @return Callable's return value (or `void`).
 */
template <typename FunctionT>
auto call(
    int64_t     handle,
    FunctionT &&function
) -> decltype(function())
{
    return executeInQueue(detail::stateForHandle(handle), std::forward<FunctionT>(function));
}

/**
 * @brief Worker thread loop.
 *
 * Processes jobs from @p state.queue until program termination. The function
 * blocks inside `state.condition_variable.wait()` whenever the queue is empty
 * and wakes up once new jobs become available.  Each invocation of the
 * returned thunk is executed in FIFO order and exceptions propagate to the
 * worker thread (terminating the program if uncaught).
 *
 * @param state Dispatcher state whose queue and synchronisation primitives are serviced by this loop.
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

/**
 * @brief Launch the background worker for @p state exactly once.
 *
 * Internally relies on `std::call_once` to guarantee that a single detached
 * worker thread is created for a given ::DispatchState instance, independent
 * of how many times this function is called or from which thread.  If the
 * worker is already running, the call becomes a no-op.
 *
 * @param state Dispatcher state that owns the queue/condition variable which the spawned worker operates on.
 */
inline void ensureWorkerRunning(DispatchState &state)
{
    std::call_once(state.worker_started, [&state] { std::thread([&state] { workerLoop(state); }).detach(); });
}

/**
 * @brief Execute a callable sequentially via the queue stored in @p state.
 *
 * The function performs the following steps:
 * 1. Wrap the provided callable in `std::packaged_task` to obtain a future.
 * 2. Ensure a worker thread is servicing the queue (lazy initialisation).
 * 3. Enqueue a thunk that invokes the packaged task.
 * 4. Notify the worker and block on the associated `std::future`.
 *
 * @tparam FunctionT Type of the zero-argument callable.
 * @param state Dispatcher state backing the queue.
 * @param function Callable object to execute.
 * @return Callable's return value if the callable returns a non-void type. `void` otherwise.
 *
 * @note Exceptions thrown by @p function are rethrown in the calling thread when `future.get()` is invoked.
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

} // namespace cpp_core::internal::seq
