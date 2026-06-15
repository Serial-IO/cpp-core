#pragma once

#include <meta>

#include <concepts>
#include <cstddef>
#include <string_view>
#include <type_traits>

namespace cpp_core::reflection
{

template <typename T>
concept ReflectableRecord = std::is_class_v<T> || std::is_union_v<T>;

namespace detail
{

template <typename T> consteval auto publicAccessContext() -> std::meta::access_context
{
    return std::meta::access_context::unprivileged().via(^^T);
}

} // namespace detail

template <ReflectableRecord T> [[nodiscard]] consteval auto hasPubliclyReflectableFields() -> bool
{
    constexpr auto access_context = detail::publicAccessContext<T>();
    return !std::meta::has_inaccessible_nonstatic_data_members(^^T, access_context);
}

template <ReflectableRecord T> [[nodiscard]] consteval auto publicFieldCount() -> std::size_t
{
    constexpr auto access_context = detail::publicAccessContext<T>();
    static_assert(!std::meta::has_inaccessible_nonstatic_data_members(^^T, access_context),
                  "cpp_core::reflection::publicFieldCount requires publicly reflectable non-static data members");
    return std::meta::nonstatic_data_members_of(^^T, access_context).size();
}

template <typename E>
requires std::is_enum_v<E>
[[nodiscard]] consteval auto enumeratorCount() -> std::size_t
{
    return std::meta::enumerators_of(^^E).size();
}

template <typename E, std::size_t Index>
requires std::is_enum_v<E>
[[nodiscard]] consteval auto enumeratorName() -> std::string_view
{
    static_assert(Index < enumeratorCount<E>(), "Enumerator index out of range");
    return std::meta::identifier_of(std::meta::enumerators_of(^^E)[Index]);
}

template <ReflectableRecord T, std::size_t Index>
[[nodiscard]] consteval auto publicFieldName() -> std::string_view
{
    constexpr auto access_context = detail::publicAccessContext<T>();
    static_assert(!std::meta::has_inaccessible_nonstatic_data_members(^^T, access_context),
                  "cpp_core::reflection::publicFieldName requires publicly reflectable non-static data members");
    static_assert(Index < publicFieldCount<T>(), "Field index out of range");
    return std::meta::identifier_of(std::meta::nonstatic_data_members_of(^^T, access_context)[Index]);
}

template <ReflectableRecord T> inline constexpr std::size_t public_field_count_v = publicFieldCount<T>();
template <typename E>
requires std::is_enum_v<E>
inline constexpr std::size_t enumerator_count_v = enumeratorCount<E>();
template <typename E, std::size_t Index>
requires std::is_enum_v<E>
inline constexpr std::string_view enumerator_name_v = enumeratorName<E, Index>();
template <ReflectableRecord T, std::size_t Index>
inline constexpr std::string_view public_field_name_v = publicFieldName<T, Index>();

} // namespace cpp_core::reflection
