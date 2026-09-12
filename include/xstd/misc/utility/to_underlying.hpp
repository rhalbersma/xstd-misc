//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_UTILITY_TO_UNDERLYING_HPP
#define XSTD_MISC_UTILITY_TO_UNDERLYING_HPP

#include <type_traits> // integral_constant, is_enum_v, underlying_type_t

namespace xstd {

// The cast P1682R1 specifies std::to_underlying as, and nothing more.
template<class Enum>
        requires std::is_enum_v<Enum>
[[nodiscard]] constexpr auto to_underlying(Enum value) noexcept
        -> std::underlying_type_t<Enum>
{
        // NOLINTNEXTLINE(bugprone-non-zero-enum-to-bool-conversion): a bool-underlying enum value-initializes to false, so the cast is not always true.
        return static_cast<std::underlying_type_t<Enum>>(value);
}

// Preserves constantness for an integral_constant-wrapped enum; qualified, so no enum namespace supplies its own by ADL.
template<class Enum, Enum N>
        requires std::is_enum_v<Enum>
[[nodiscard]] constexpr auto to_underlying(std::integral_constant<Enum, N>) noexcept
{
        return std::integral_constant<std::underlying_type_t<Enum>, xstd::to_underlying(N)>();
}

} // namespace xstd

#endif // XSTD_MISC_UTILITY_TO_UNDERLYING_HPP
