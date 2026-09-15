//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_TYPE_TRAITS_IS_MIXED_SPECIALIZATION_OF_HPP
#define XSTD_MISC_TYPE_TRAITS_IS_MIXED_SPECIALIZATION_OF_HPP

#include <type_traits> // bool_constant

namespace xstd {

// Whether T is a specialization of a class template taking a type and then values:
// std::array<T, N>, std::span<T, E>, and a storage naming its blocks and then its extent.
//
// A constrained parameter is no obstacle: an unconstrained template template parameter does not
// consider the constraints on its argument, so a template<contiguous_range Blocks, size_t N> class
// binds here as a template<class, size_t> class does.
//
// At least one of each, rather than bare packs: <class, auto...> alone would also take
// std::complex, deducing no values, and every types-only match would be ambiguous between the two.
template<class T, template<class, auto, auto...> class Primary>
inline constexpr auto is_mixed_specialization_of_v = false;

template<template<class, auto, auto...> class Primary, class Arg, auto Val, auto... Vals>
inline constexpr auto is_mixed_specialization_of_v<Primary<Arg, Val, Vals...>, Primary> = true;

template<class T, template<class, auto, auto...> class Primary>
using is_mixed_specialization_of = std::bool_constant<is_mixed_specialization_of_v<T, Primary>>;

} // namespace xstd

#endif // XSTD_MISC_TYPE_TRAITS_IS_MIXED_SPECIALIZATION_OF_HPP
