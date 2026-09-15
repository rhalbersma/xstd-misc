//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_TYPE_TRAITS_IS_NONTYPE_SPECIALIZATION_OF_HPP
#define XSTD_MISC_TYPE_TRAITS_IS_NONTYPE_SPECIALIZATION_OF_HPP

#include <type_traits> // bool_constant

namespace xstd {

// Whether T is a specialization of a class template whose parameters are all values:
// std::bitset<N>, std::ratio<N, D>. The sibling of is_specialization_of for the other kind.
//
// At least one value, rather than a bare pack: a trailing pack deduces to empty, and a shape that
// did not demand its own first parameter would also match where another already does.
template<class T, template<auto, auto...> class Primary>
inline constexpr auto is_nontype_specialization_of_v = false;

template<template<auto, auto...> class Primary, auto Val, auto... Vals>
inline constexpr auto is_nontype_specialization_of_v<Primary<Val, Vals...>, Primary> = true;

template<class T, template<auto, auto...> class Primary>
using is_nontype_specialization_of = std::bool_constant<is_nontype_specialization_of_v<T, Primary>>;

} // namespace xstd

#endif // XSTD_MISC_TYPE_TRAITS_IS_NONTYPE_SPECIALIZATION_OF_HPP
