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

namespace cpp_core::detail::seq
{

struct DispatchState
{
    /** Mutex protecting queue + condition_variable. */
    std::mutex mutex;

    /** Condition variable to wake the worker thread when new jobs arrive. */
    std::condition_variable condition_variable;

    /** FIFO containing packaged thunks (jobs). */
    std::queue<std::function<void()>> queue;

    /** Ensures the worker thread is launched exactly once. */
    std::once_flag worker_started;
};

enum class QueueMode
{
    kGlobal,
    kPerHandle
};

/**
 * @brief Determines how sequential dispatching is applied.
 *
 * @li kGlobal     — All API calls are executed through a single process-wide
 *                   queue. Guarantees strict ordering across every handle
 *                   (legacy behaviour).
 * @li kPerHandle — Each serial handle owns its own queue/worker thread. Calls
 *                   on different handles may run in parallel while calls on
 *                   the same handle remain strictly ordered.
 */
// Holds the current mode – defaults to the former behaviour (global queue)
inline auto queueMode() -> std::atomic<QueueMode> &
{
    static std::atomic<QueueMode> mode{QueueMode::kGlobal};
    return mode;
}

/**
 * @brief Thread-safe accessor to the currently selected ::QueueMode.
 *
 * The `std::atomic` wrapper allows lock-free loads/stores from any thread.
 * The variable is a Meyers-singleton to sidestep static initialisation order
 * issues.
 *
 * @return Reference to the internal atomic holding the active mode.
 */
inline void setQueueMode(QueueMode mode)
{
    queueMode().store(mode, std::memory_order_relaxed);
}

/**
 * @brief Retrieve the currently active sequential dispatch mode.
 */
inline auto getQueueMode() -> QueueMode
{
    return queueMode().load(std::memory_order_relaxed);
}

namespace detail
{

/**
 * @brief Global map that stores a dedicated ::DispatchState for each serial
 *        handle encountered during runtime. The map is lazily populated on
 *        first use of a handle.
 */
inline auto handleStates() -> std::unordered_map<
    int64_t,
    DispatchState> &
{
    static std::unordered_map<int64_t, DispatchState> states;
    return states;
}

/**
 * @brief Mutex guarding access to ::handleStates() (adds/removes look-ups).
 */
inline auto handleStatesMutex() -> std::mutex &
{
    static std::mutex mutex;
    return mutex;
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
 * @param function Function object to be executed sequentially.
 * @return The callable's return value (or `void`).
 */
template <typename FunctionT> auto call(FunctionT &&function) -> decltype(function())
{
    return executeInQueue(state(), std::forward<FunctionT>(function));
}

/**
 * @brief Execute @p function on the *handle-local* worker thread.
 *
 * Helper used by the public `call(handle, fn)` overload when
 * ::QueueMode::kPerHandle is active. All semantics are identical to the global
 * variant but the queue/worker are scoped to the given handle.
 *
 * @tparam FunctionT Callable type with no parameters.
 * @param handle Serial handle used to select the queue.
 * @param function Callable object to execute.
 * @return Callable’s return value (or `void`).
 */
template <typename FunctionT>
auto callPerHandle(
    int64_t     handle,
    FunctionT &&function
) -> decltype(function())
{
    return executeInQueue(detail::stateForHandle(handle), std::forward<FunctionT>(function));
}

/**
 * @brief Dispatch @p function via the global queue or per-handle queue
 *        depending on ::getQueueMode().
 *
 * This is the entry point used by all *Sequential* API wrappers that accept
 * a serial handle.
 *
 * @tparam FunctionT Callable type with no parameters.
 * @param handle Serial handle identifying the per-handle queue.
 * @param function Callable to execute.
 * @return Callable’s return value (or `void`).
 */
template <typename FunctionT>
auto call(
    int64_t     handle,
    FunctionT &&function
) -> decltype(function())
{
    if (getQueueMode() == QueueMode::kPerHandle)
    {
        return callPerHandle(handle, std::forward<FunctionT>(function));
    }
    return call(std::forward<FunctionT>(function));
}

/**
 * @brief Worker thread loop – processes @p state until program termination.
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
 * @brief Ensure a dedicated worker thread exists for the given @p state.
 */
inline void ensureWorkerRunning(DispatchState &state)
{
    std::call_once(state.worker_started, [&state] { std::thread([&state] { workerLoop(state); }).detach(); });
}

/**
 * @brief Enqueue @p function in @p state and wait for completion.
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

} // namespace cpp_core::detail::seq
