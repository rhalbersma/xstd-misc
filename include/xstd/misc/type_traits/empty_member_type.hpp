//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_TYPE_TRAITS_EMPTY_MEMBER_TYPE_HPP
#define XSTD_MISC_TYPE_TRAITS_EMPTY_MEMBER_TYPE_HPP

#include <compare>     // strong_ordering
#include <concepts>    // same_as
#include <type_traits> // remove_cvref_t

namespace xstd {

// An absent data member, safely comparable where empty_base_type is not: a member's hidden friends do not reach the enclosing class. Tagged, to keep two distinct in a layout.
template<class Tag = void>
struct empty_member_type
{
        [[nodiscard]] empty_member_type() = default;

        // Lets an enclosing class construct this member, without hijacking copy or move construction.
        template<class... Args>
                requires ((not std::same_as<std::remove_cvref_t<Args>, empty_member_type>) and ...)
        [[nodiscard]] constexpr explicit empty_member_type(Args&&...) noexcept
        {}

        // Lets an enclosing class default its comparisons over this member.
        [[nodiscard]] friend auto operator<=>(empty_member_type, empty_member_type) -> std::strong_ordering = default;
};

} // namespace xstd

#endif // XSTD_MISC_TYPE_TRAITS_EMPTY_MEMBER_TYPE_HPP
