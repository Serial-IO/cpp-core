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
 * @brief Thread-safe singleton storing the process-wide ::QueueMode flag.
 *
 * The underlying variable is declared as a local static `std::atomic` to avoid
 * the static-initialisation-order fiasco. Accessing the flag via this
 * function guarantees that the object is initialised on first use in a
 * thread-safe manner (C++11 and later).
 *
 * The returned reference enables lock-free loads and stores from any thread.
 * All public helpers such as ::setQueueMode() and ::getQueueMode() act on the
 * object obtained here.
 *
 * @return Reference to the `std::atomic<QueueMode>` that tracks the active sequential dispatch policy.
 */
inline auto queueMode() -> std::atomic<QueueMode> &
{
    static std::atomic<QueueMode> mode{QueueMode::kGlobal};
    return mode;
}

/**
 * @brief Select the sequential dispatch policy at runtime.
 *
 * Performs a relaxed atomic store on the flag returned by ::queueMode(). The
 * operation is lock-free and therefore safe to call from any thread at any
 * time during program execution.
 *
 * @param mode The ::QueueMode to activate for subsequent sequential API calls.
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
 * @brief Global map that stores a dedicated ::DispatchState for each serial handle encountered during runtime. The map
 * is lazily populated on first use of a handle.
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
 * @brief Execute a callable on the *process-global* sequential queue.
 *
 * The callable is enqueued in FIFO order on the dispatcher returned by
 * ::state(). The calling thread blocks until the job finishes and forwards
 * the return value or rethrows any exception.
 *
 * This overload is primarily used by API wrappers that do not take an explicit
 * serial handle.  When ::QueueMode::kPerHandle is enabled the global queue
 * continues to exist; calls routed through this function therefore still run
 * sequentially with respect to one another but *not* with respect to jobs
 * dispatched via the per-handle overload.
 *
 * Steps:
 * 1. Wrap the callable in `std::packaged_task` to obtain an associated
 *    `std::future`.
 * 2. Ensure the background worker servicing the global queue is running.
 * 3. Enqueue the packaged task and notify the worker.
 * 4. Wait on the future and return (or propagate) the result.
 *
 * @tparam FunctionT Zero-argument callable type.
 * @param function Callable object to execute.
 * @return Callable’s return value, or `void` if the callable is `void`-returning.
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
 * @copydoc call(FunctionT &&)
 *
 * Dispatches the callable either through the global queue or, when
 * ::QueueMode::kPerHandle is active, through the queue dedicated to
 * @p handle.
 *
 * @param handle Serial handle identifying the per-handle queue.
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
 * @return Callable’s return value if the callable returns a non-void type. `void` otherwise.
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

} // namespace cpp_core::detail::seq
