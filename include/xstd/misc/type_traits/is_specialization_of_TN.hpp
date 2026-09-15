//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_TYPE_TRAITS_IS_SPECIALIZATION_OF_TN_HPP
#define XSTD_MISC_TYPE_TRAITS_IS_SPECIALIZATION_OF_TN_HPP

#include <type_traits> // bool_constant

namespace xstd {

// Whether T is a specialization of a class template taking a type and then values.
template<class T, template<class, auto, auto...> class Primary>
inline constexpr auto is_specialization_of_TN_v = false;

// At least one of each, so that an empty value pack cannot make this match a types-only template.
template<template<class, auto, auto...> class Primary, class Arg, auto Val, auto... Vals>
inline constexpr auto is_specialization_of_TN_v<Primary<Arg, Val, Vals...>, Primary> = true;

template<class T, template<class, auto, auto...> class Primary>
using is_specialization_of_TN = std::bool_constant<is_specialization_of_TN_v<T, Primary>>;

} // namespace xstd

#endif // XSTD_MISC_TYPE_TRAITS_IS_SPECIALIZATION_OF_TN_HPP
