#pragma once

#include "execute_in_queue.h"
#include "state_for_handle.h"
#include <cstdint>
#include <utility>

namespace cpp_core::internal::sequential
{
/**
 * @brief Executes the given callable in the sequential dispatch queue associated with the specified handle.
 *
 * This helper forwards the callable to executeInQueue() together with the
 * DispatchState that belongs to @p handle, thereby ensuring that all calls
 * targeting the same handle are processed strictly sequentially.
 *
 * @tparam FunctionT Type of the invocable object.
 * @param handle Handle identifying the sequential queue.
 * @param function Callable object to execute. Can return any type (including void).
 * @return Whatever @p function returns.
 */
template <typename FunctionT>
auto call(
    int64_t     handle,
    FunctionT &&function
) -> decltype(function())
{
    return executeInQueue(
        ::cpp_core::internal::sequential::internal::stateForHandle(handle), std::forward<FunctionT>(function)
    );
}
} // namespace cpp_core::internal::sequential
