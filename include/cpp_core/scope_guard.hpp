#pragma once

#include <concepts>
#include <exception>
#include <type_traits>
#include <utility>

namespace cpp_core
{

// Core ScopeGuard

// RAII scope guard that invokes a callable on destruction.
// Supports dismiss() to cancel the action.
//
//   auto guard = ScopeGuard([&] { cleanup(); });
//   // ... if everything succeeds ...
//   guard.dismiss();
template <std::invocable Fn> class [[nodiscard]] ScopeGuard
{
  public:
    constexpr explicit ScopeGuard(Fn func) noexcept(std::is_nothrow_move_constructible_v<Fn>) : fn_(std::move(func))
    {
    }

    ScopeGuard(const ScopeGuard &) = delete;
    auto operator=(const ScopeGuard &) -> ScopeGuard & = delete;

    constexpr ScopeGuard(ScopeGuard &&other) noexcept(std::is_nothrow_move_constructible_v<Fn>)
        : fn_(std::move(other.fn_)), active_(other.active_)
    {
        other.active_ = false;
    }

    auto operator=(ScopeGuard &&) -> ScopeGuard & = delete;

    ~ScopeGuard()
    {
        if (active_)
        {
            fn_();
        }
    }

    constexpr auto dismiss() noexcept -> void
    {
        active_ = false;
    }

    [[nodiscard]] constexpr auto isActive() const noexcept -> bool
    {
        return active_;
    }

  private:
    Fn fn_;
    bool active_{true};
};

template <typename Fn> ScopeGuard(Fn) -> ScopeGuard<Fn>;

// ScopeExit / ScopeFail / ScopeSuccess

template <std::invocable Fn> [[nodiscard]] constexpr auto onScopeExit(Fn &&func)
{
    return ScopeGuard<std::remove_cvref_t<Fn>>(std::forward<Fn>(func));
}

// Runs only when the scope exits via an exception.
template <std::invocable Fn> class [[nodiscard]] ScopeFail
{
  public:
    constexpr explicit ScopeFail(Fn func) noexcept(std::is_nothrow_move_constructible_v<Fn>)
        : fn_(std::move(func)), exceptions_on_entry_(std::uncaught_exceptions())
    {
    }

    ScopeFail(const ScopeFail &) = delete;
    auto operator=(const ScopeFail &) -> ScopeFail & = delete;
    ScopeFail(ScopeFail &&) = delete;
    auto operator=(ScopeFail &&) -> ScopeFail & = delete;

    ~ScopeFail()
    {
        if (std::uncaught_exceptions() > exceptions_on_entry_)
        {
            fn_();
        }
    }

  private:
    Fn fn_;
    int exceptions_on_entry_;
};

template <typename Fn> ScopeFail(Fn) -> ScopeFail<Fn>;

template <std::invocable Fn> [[nodiscard]] constexpr auto onScopeFail(Fn &&func)
{
    return ScopeFail<std::remove_cvref_t<Fn>>(std::forward<Fn>(func));
}

// Runs only when the scope exits normally (no exception).
template <std::invocable Fn> class [[nodiscard]] ScopeSuccess
{
  public:
    constexpr explicit ScopeSuccess(Fn func) noexcept(std::is_nothrow_move_constructible_v<Fn>)
        : fn_(std::move(func)), exceptions_on_entry_(std::uncaught_exceptions())
    {
    }

    ScopeSuccess(const ScopeSuccess &) = delete;
    auto operator=(const ScopeSuccess &) -> ScopeSuccess & = delete;
    ScopeSuccess(ScopeSuccess &&) = delete;
    auto operator=(ScopeSuccess &&) -> ScopeSuccess & = delete;

    ~ScopeSuccess()
    {
        if (std::uncaught_exceptions() <= exceptions_on_entry_)
        {
            fn_();
        }
    }

  private:
    Fn fn_;
    int exceptions_on_entry_;
};

template <typename Fn> ScopeSuccess(Fn) -> ScopeSuccess<Fn>;

template <std::invocable Fn> [[nodiscard]] constexpr auto onScopeSuccess(Fn &&func)
{
    return ScopeSuccess<std::remove_cvref_t<Fn>>(std::forward<Fn>(func));
}

// DEFER macro (Go-style)
// Usage:  DEFER { cleanup(); };

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
namespace detail
{
struct DeferHelper
{
    template <std::invocable Fn> constexpr auto operator<<(Fn &&func) const
    {
        return ScopeGuard<std::remove_cvref_t<Fn>>(std::forward<Fn>(func));
    }
};
} // namespace detail

#define CPP_CORE_DEFER_CONCAT_(a, b) a##b
#define CPP_CORE_DEFER_CONCAT(a, b) CPP_CORE_DEFER_CONCAT_(a, b)
#define DEFER auto CPP_CORE_DEFER_CONCAT(_cpp_core_defer_, __COUNTER__) = ::cpp_core::detail::DeferHelper{} << [&]()
// NOLINTEND(cppcoreguidelines-macro-usage)

} // namespace cpp_core
