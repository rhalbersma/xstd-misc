//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_TYPE_TRAITS_EMPTY_BASE_TYPE_HPP
#define XSTD_MISC_TYPE_TRAITS_EMPTY_BASE_TYPE_HPP

namespace xstd {

// An absent base with no comparison: its hidden friends reach the derived class by ADL. Tagged, to derive from it once.
template<class Tag = void>
struct empty_base_type
{};

} // namespace xstd

#endif // XSTD_MISC_TYPE_TRAITS_EMPTY_BASE_TYPE_HPP
