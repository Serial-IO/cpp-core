#pragma once

#include <concepts>
#include <cstdint>
#include <string_view>
#include <type_traits>

namespace cpp_core::status_codes
{

namespace detail
{

using ValueType = std::int64_t;

inline constexpr ValueType kCategoryMultiplier{100};

template <ValueType NumericValue> struct Code
{
    static constexpr ValueType kValue = NumericValue;

    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    constexpr operator ValueType() const noexcept
    {
        return kValue;
    }
    [[nodiscard]] constexpr auto value() const noexcept -> ValueType
    {
        return kValue;
    }
};

template <typename Derived> struct CategoryBase
{
  private:
    constexpr CategoryBase() = default;

  protected:
    template <ValueType LocalCode> static consteval auto computeValue() -> ValueType
    {
        static_assert(LocalCode >= 0, "Code index must not be negative");
        static_assert(LocalCode < kCategoryMultiplier, "Category overflow (max 99 codes)");
        return -((Derived::kCategoryCode * kCategoryMultiplier) + LocalCode);
    }

    template <ValueType LocalCode> using GenCode = Code<computeValue<LocalCode>()>;

    friend Derived;
};

} // namespace detail

// Helper Macro
//
//  CPP_CORE_STATUS_CATEGORY(Name, Id)   opens a category struct
//  CPP_CORE_CODE(name)                  declares an auto-numbered code
//  CPP_CORE_STATUS_CATEGORY_END()       closes the category
//
//  Each code carries:
//    .value()     / compile-time numeric value
//    .name()      / "kSomethingError"
//    .category()  / "CategoryName"
//  and converts implicitly to ValueType (no static_cast needed).

// NOLINTBEGIN(cppcoreguidelines-macro-usage)

#define CPP_CORE_STATUS_CATEGORY(Name, CategoryId)                                                                     \
    struct Name : ::cpp_core::status_codes::detail::CategoryBase<Name>                                                 \
    {                                                                                                                  \
        static constexpr ::cpp_core::status_codes::detail::ValueType kCategoryCode = CategoryId;                       \
        static constexpr std::string_view kCategoryName{#Name};                                                        \
                                                                                                                       \
      private:                                                                                                         \
        static constexpr int kCounterBase_ = __COUNTER__;                                                              \
                                                                                                                       \
      public:

// NOLINTNEXTLINE(bugprone-macro-parentheses)
#define CPP_CORE_CODE(code_name)                                                                                       \
    static constexpr struct code_name##_t                                                                              \
        : GenCode<__COUNTER__ - kCounterBase_ - 1>{[[nodiscard]] constexpr auto name() const noexcept                  \
                                                   -> std::string_view{return #code_name;                              \
    }                                                                                                                  \
    [[nodiscard]] constexpr auto category() const noexcept -> std::string_view                                         \
    {                                                                                                                  \
        return kCategoryName;                                                                                          \
    }                                                                                                                  \
    }                                                                                                                  \
    (code_name)                                                                                                        \
    {                                                                                                                  \
    }

#define CPP_CORE_STATUS_CATEGORY_END() }

// NOLINTEND(cppcoreguidelines-macro-usage)

struct StatusCode
{
    static constexpr detail::ValueType kSuccess = 0;

    CPP_CORE_STATUS_CATEGORY(Configuration, 1);
    CPP_CORE_CODE(kSetBaudrateError);
    CPP_CORE_CODE(kSetDataBitsError);
    CPP_CORE_CODE(kSetParityError);
    CPP_CORE_CODE(kSetStopBitsError);
    CPP_CORE_CODE(kSetFlowControlError);
    CPP_CORE_CODE(kSetTimeoutError);
    CPP_CORE_STATUS_CATEGORY_END();

    CPP_CORE_STATUS_CATEGORY(Connection, 2);
    CPP_CORE_CODE(kNotFoundError);
    CPP_CORE_CODE(kInvalidHandleError);
    CPP_CORE_CODE(kCloseHandleError);
    CPP_CORE_STATUS_CATEGORY_END();

    CPP_CORE_STATUS_CATEGORY(Io, 3);
    CPP_CORE_CODE(kReadError);
    CPP_CORE_CODE(kWriteError);
    CPP_CORE_CODE(kAbortReadError);
    CPP_CORE_CODE(kAbortWriteError);
    CPP_CORE_CODE(kBufferError);
    CPP_CORE_CODE(kClearBufferInError);
    CPP_CORE_CODE(kClearBufferOutError);
    CPP_CORE_STATUS_CATEGORY_END();

    CPP_CORE_STATUS_CATEGORY(Control, 4);
    CPP_CORE_CODE(kSetDtrError);
    CPP_CORE_CODE(kSetRtsError);
    CPP_CORE_CODE(kGetModemStatusError);
    CPP_CORE_CODE(kSendBreakError);
    CPP_CORE_CODE(kGetStateError);
    CPP_CORE_CODE(kSetStateError);
    CPP_CORE_STATUS_CATEGORY_END();

    [[nodiscard]] static constexpr auto isError(detail::ValueType code) noexcept -> bool
    {
        return code < 0;
    }
    [[nodiscard]] static constexpr auto isSuccess(detail::ValueType code) noexcept -> bool
    {
        return code >= 0;
    }
    template <typename Category> [[nodiscard]] static constexpr auto belongsTo(detail::ValueType code) noexcept -> bool
    {
        return code < 0 && (-code) / detail::kCategoryMultiplier == Category::kCategoryCode;
    }
};

} // namespace cpp_core::status_codes

namespace cpp_core
{
using ::cpp_core::status_codes::StatusCode;
} // namespace cpp_core
