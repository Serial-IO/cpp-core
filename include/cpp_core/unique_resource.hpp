#pragma once

#include <concepts>
#include <type_traits>
#include <utility>

namespace cpp_core
{

/**
 * Trait contract for UniqueResource. Specialize this for your platform handle.
 * Required members:
 *   using handle_type = ...;
 *   static constexpr handle_type invalid() noexcept;
 *   static void close(handle_type h) noexcept;
 *
 * Example (POSIX):
 *   template<> struct ResourceTraits<int> {
 *       using handle_type = int;
 *       static constexpr handle_type invalid() noexcept { return -1; }
 *       static void close(handle_type h) noexcept { ::close(h); }
 *   };
 */
template <typename T> struct ResourceTraits;

// clang-format off
template <typename T>
concept ResourceTraitSpec = requires {
    typename T::handle_type;
    { T::invalid() } noexcept -> std::same_as<typename T::handle_type>;
    { T::close(std::declval<typename T::handle_type>()) } noexcept;
} && std::is_nothrow_move_constructible_v<typename T::handle_type>
  && std::is_nothrow_copy_constructible_v<typename T::handle_type>
  && std::equality_comparable<typename T::handle_type>;
// clang-format on

/**
 * Generic RAII wrapper parameterized by a traits type.
 * Drop-in replacement for UniqueFd / UniqueHandle with zero overhead.
 *   using UniqueFd     = UniqueResource<PosixFdTraits>;
 *   using UniqueHandle = UniqueResource<Win32HandleTraits>;
 */
template <ResourceTraitSpec Traits> class UniqueResource
{
  public:
    using HandleType = typename Traits::handle_type;
    using TraitsType = Traits;

    constexpr UniqueResource() noexcept = default;

    constexpr explicit UniqueResource(HandleType handle) noexcept : handle_(handle)
    {
    }

    UniqueResource(const UniqueResource &) = delete;
    auto operator=(const UniqueResource &) -> UniqueResource & = delete;

    constexpr UniqueResource(UniqueResource &&other) noexcept : handle_(other.handle_)
    {
        other.handle_ = Traits::invalid();
    }

    constexpr auto operator=(UniqueResource &&other) noexcept -> UniqueResource &
    {
        if (this != &other)
        {
            reset(other.release());
        }
        return *this;
    }

    ~UniqueResource()
    {
        reset(Traits::invalid());
    }

    [[nodiscard]] constexpr auto get() const noexcept -> HandleType
    {
        return handle_;
    }

    [[nodiscard]] constexpr auto valid() const noexcept -> bool
    {
        return handle_ != Traits::invalid();
    }

    [[nodiscard]] constexpr explicit operator bool() const noexcept
    {
        return valid();
    }

    auto reset(HandleType new_handle = Traits::invalid()) noexcept -> void
    {
        if (valid())
        {
            Traits::close(handle_);
        }
        handle_ = new_handle;
    }

    [[nodiscard]] constexpr auto release() noexcept -> HandleType
    {
        HandleType out = handle_;
        handle_ = Traits::invalid();
        return out;
    }

    // Swap two resources without closing either.
    constexpr friend auto swap(UniqueResource &lhs, UniqueResource &rhs) noexcept -> void
    {
        using std::swap;
        swap(lhs.handle_, rhs.handle_);
    }

    [[nodiscard]] constexpr auto operator<=>(const UniqueResource &) const noexcept = default;

  private:
    HandleType handle_ = Traits::invalid();
};

} // namespace cpp_core
