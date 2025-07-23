#pragma once

#include "handle_states.h"
#include "handle_states_mutex.h"
#include <cstdint>
#include <mutex>

namespace cpp_core::internal::sequential::internal
{
/**
 * @brief Retrieves the DispatchState associated with the specified handle.
 *
 * The global map is protected by a mutex to make this operation thread-safe.
 *
 * @param handle Identifier of the sequential dispatch queue.
 * @return Reference to the corresponding DispatchState object.
 */
inline auto stateForHandle(int64_t handle) -> cpp_core::internal::sequential::DispatchState &
{
    std::lock_guard<std::mutex> lock(handleStatesMutex());
    return handleStates()[handle];
}
} // namespace cpp_core::internal::sequential::internal
