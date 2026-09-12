//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_TYPE_TRAITS_CONDITIONAL_DATA_MEMBER_HPP
#define XSTD_MISC_TYPE_TRAITS_CONDITIONAL_DATA_MEMBER_HPP

#include <xstd/misc/type_traits/empty_member_type.hpp> // IWYU pragma: export; empty_member_type
#include <xstd/misc/type_traits/no_unique_address.hpp> // IWYU pragma: export; XSTD_NO_UNIQUE_ADDRESS
#include <type_traits>                                 // conditional, conditional_t

namespace xstd {

// Tag keeps multiple absent [[XSTD_NO_UNIQUE_ADDRESS]] data members distinct.
template<bool Condition, class Type, class Tag>
using conditional_data_member = std::conditional<Condition, Type, empty_member_type<Tag>>;

// conditional_t, not the trait's dependent ::type, which would need the typename P0634R3 made optional in Clang 16.
template<bool Condition, class Type, class Tag>
using conditional_data_member_t = std::conditional_t<Condition, Type, empty_member_type<Tag>>;

} // namespace xstd

#endif // XSTD_MISC_TYPE_TRAITS_CONDITIONAL_DATA_MEMBER_HPP
