//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_UTILITY_TO_UNDERLYING_HPP
#define XSTD_MISC_UTILITY_TO_UNDERLYING_HPP

#include <type_traits> // integral_constant, is_enum_v, underlying_type_t

namespace xstd {

// The cast P1682R1 specifies std::to_underlying as: its Returns clause is
// static_cast<underlying_type_t<T>>(value) and nothing more.
template<class Enum>
        requires std::is_enum_v<Enum>
[[nodiscard]] constexpr auto to_underlying(Enum value) noexcept
        -> std::underlying_type_t<Enum>
{
        // An enum with a bool underlying type and no enumerators makes this a cast to
        // bool, which bugprone-non-zero-enum-to-bool-conversion reads as always true.
        // A value-initialized one converts to false, so it is not. Suppressed here
        // rather than in .clang-tidy, so a consumer linting their own code gets it too.
        // NOLINTNEXTLINE(bugprone-non-zero-enum-to-bool-conversion)
        return static_cast<std::underlying_type_t<Enum>>(value);
}

// Preserves compile-time constantness for an integral_constant-wrapped enum.
// Qualified, so an enum's own namespace cannot supply a to_underlying by ADL.
template<class Enum, Enum N>
        requires std::is_enum_v<Enum>
[[nodiscard]] constexpr auto to_underlying(std::integral_constant<Enum, N>) noexcept
{
        return std::integral_constant<std::underlying_type_t<Enum>, xstd::to_underlying(N)>();
}

} // namespace xstd

#endif // XSTD_MISC_UTILITY_TO_UNDERLYING_HPP
