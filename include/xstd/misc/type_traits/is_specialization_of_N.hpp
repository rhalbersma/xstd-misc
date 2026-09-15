//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_TYPE_TRAITS_IS_SPECIALIZATION_OF_N_HPP
#define XSTD_MISC_TYPE_TRAITS_IS_SPECIALIZATION_OF_N_HPP

#include <type_traits> // bool_constant

namespace xstd {

// Whether T is a specialization of a class template whose parameters are all values.
template<class T, template<auto, auto...> class Primary>
inline constexpr auto is_specialization_of_N_v = false;

// At least one value, so that an empty pack cannot make this match where another shape does.
template<template<auto, auto...> class Primary, auto Val, auto... Vals>
inline constexpr auto is_specialization_of_N_v<Primary<Val, Vals...>, Primary> = true;

template<class T, template<auto, auto...> class Primary>
using is_specialization_of_N = std::bool_constant<is_specialization_of_N_v<T, Primary>>;

} // namespace xstd

#endif // XSTD_MISC_TYPE_TRAITS_IS_SPECIALIZATION_OF_N_HPP
