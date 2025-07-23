#pragma once

#include <mutex>

namespace cpp_core::internal::sequential::internal
{
/**
 * @brief Returns the mutex guarding access to handleStates().
 *
 * @return Reference to the singleton std::mutex.
 */
inline auto handleStatesMutex() -> std::mutex &
{
    static std::mutex instance;
    return instance;
}
} // namespace cpp_core::internal::sequential::internal
