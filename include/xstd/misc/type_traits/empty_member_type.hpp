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

// A stand-in for a data member that is not there, and a MEMBER is what it is for: an enclosing class both
// constructs it and defaults its comparisons over it, and the hidden friend below reaches neither anyone else's
// ADL nor anyone else's overload set, a member's associated classes not being the enclosing class's.
//
// empty_base_type is the other half, and the two are not interchangeable. Derive from this one and its
// operator<=> -- with the operator== [class.compare.default] declares beside it -- is found by ADL for the
// DERIVED type, whose only base is empty, so any two objects of that type compare EQUAL. Use empty_base_type
// where the empty thing is a base.
//
// The tag keeps two empty data members distinct in a layout.
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
