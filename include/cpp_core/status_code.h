#pragma once

#include "status_code_registry.hpp"

#include <cstdint>
#include <limits>
#include <string_view>

namespace cpp_core::status_codes
{

namespace detail
{

using ValueType = std::int64_t;

inline constexpr ValueType kCategoryMultiplier{100};

template <typename Category, ValueType NumericValue> struct Code
{
    static constexpr ValueType kValue = NumericValue;
    std::string_view kName; // NOLINT(readability-identifier-naming)

    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    constexpr operator ValueType() const noexcept
    {
        return kValue;
    }
    [[nodiscard]] constexpr auto value() const noexcept -> ValueType
    {
        return kValue;
    }
    [[nodiscard]] constexpr auto name() const noexcept -> std::string_view
    {
        return kName;
    }
    [[nodiscard]] constexpr auto category() const noexcept -> std::string_view
    {
        return Category::kCategoryName;
    }
};

template <typename Derived> struct CategoryBase
{
  private:
    constexpr CategoryBase() = default;

  protected:
    template <ValueType LocalCode> static consteval auto computeValue() -> ValueType
    {
        static_assert(Derived::kCategoryCode >= 0, "Category code must not be negative");
        static_assert(LocalCode >= 0, "Code index must not be negative");
        static_assert(LocalCode < kCategoryMultiplier, "Category overflow (max 99 codes)");
        static_assert(Derived::kCategoryCode <=
                          (std::numeric_limits<ValueType>::max() - kCategoryMultiplier + 1) / kCategoryMultiplier,
                      "Category code too large, multiplication would overflow");
        return -((Derived::kCategoryCode * kCategoryMultiplier) + LocalCode);
    }

    template <ValueType LocalCode> using Code = detail::Code<Derived, computeValue<LocalCode>()>;

    friend Derived;
};

} // namespace detail

struct StatusCode
{
    using ValueType = detail::ValueType;
    static constexpr ValueType kSuccess = 0;

#define CPP_CORE_DECLARE_STATUS_CODE_MEMBER(CategoryName, LocalCode, CodeName)                                        \
    static constexpr Code<LocalCode> k##CodeName{#CodeName};

#define CPP_CORE_DECLARE_STATUS_CODE_CATEGORY(CategoryName, CategoryCode, CodeListMacro)                              \
    struct CategoryName : detail::CategoryBase<CategoryName>                                                           \
    {                                                                                                                  \
        static constexpr ValueType kCategoryCode = CategoryCode;                                                       \
        static constexpr std::string_view kCategoryName{#CategoryName};                                                \
        CodeListMacro(CPP_CORE_DECLARE_STATUS_CODE_MEMBER, CategoryName)                                               \
    };

    CPP_CORE_STATUS_CODE_CATEGORY_LIST(CPP_CORE_DECLARE_STATUS_CODE_CATEGORY)

#undef CPP_CORE_DECLARE_STATUS_CODE_CATEGORY
#undef CPP_CORE_DECLARE_STATUS_CODE_MEMBER

    [[nodiscard]] static constexpr auto isError(ValueType code) noexcept -> bool
    {
        return code < 0;
    }
    [[nodiscard]] static constexpr auto isSuccess(ValueType code) noexcept -> bool
    {
        return code >= 0;
    }
    template <typename Category> [[nodiscard]] static constexpr auto belongsTo(ValueType code) noexcept -> bool
    {
        return code < 0 && (-code) / detail::kCategoryMultiplier == Category::kCategoryCode;
    }
};

} // namespace cpp_core::status_codes

namespace cpp_core
{
using StatusCodeValue = ::cpp_core::status_codes::detail::ValueType;
using ::cpp_core::status_codes::StatusCode;

struct StatusCodes
{
    static constexpr StatusCodeValue kSuccess = StatusCode::kSuccess;

#define CPP_CORE_DECLARE_STATUS_CODE_ALIAS(CategoryName, LocalCode, CodeName)                                         \
    static constexpr StatusCodeValue k##CodeName = StatusCode::CategoryName::k##CodeName;

#define CPP_CORE_DECLARE_STATUS_CODE_ALIAS_CATEGORY(CategoryName, CategoryCode, CodeListMacro)                        \
    CodeListMacro(CPP_CORE_DECLARE_STATUS_CODE_ALIAS, CategoryName)

    CPP_CORE_STATUS_CODE_CATEGORY_LIST(CPP_CORE_DECLARE_STATUS_CODE_ALIAS_CATEGORY)

#undef CPP_CORE_DECLARE_STATUS_CODE_ALIAS_CATEGORY
#undef CPP_CORE_DECLARE_STATUS_CODE_ALIAS
};
} // namespace cpp_core
