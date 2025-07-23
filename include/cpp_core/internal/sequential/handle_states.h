#pragma once

#include "dispatch_state.h"
#include <cstdint>
#include <mutex>
#include <unordered_map>

namespace cpp_core::internal::sequential::internal
{
/**
 * @brief Gives access to the global handle -> DispatchState map.
 *
 * The map is lazily initialised on first access and is intended to be
 * protected by handleStatesMutex().
 *
 * @return Reference to the singleton unordered_map storing all DispatchStates.
 */
inline auto handleStates() -> std::unordered_map<
    int64_t,
    cpp_core::internal::sequential::DispatchState> &
{
    static std::unordered_map<int64_t, cpp_core::internal::sequential::DispatchState> instance;
    return instance;
}
} // namespace cpp_core::internal::sequential::internal
