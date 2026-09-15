//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_TYPE_TRAITS_IS_SAME_TEMPLATE_AS_HPP
#define XSTD_MISC_TYPE_TRAITS_IS_SAME_TEMPLATE_AS_HPP

#include <type_traits> // bool_constant

namespace xstd {

// Whether T and U are specializations of one class template, whatever the kinds of
// that template's parameters.
//
// The kinds are part of a class template's own type, and no single template template
// parameter binds them all: <class...> takes std::complex and std::vector but not
// std::array, <auto...> takes std::bitset but neither of those. A parameter mixing the
// two, <class..., auto...>, is not a parameter list any template may have -- a pack must
// come last -- so the primary template cannot be a parameter here at all. It is named by
// an example specialization of it instead, and the kinds move into the partial
// specializations below, one per shape.
//
// Each shape demands the parameter that tells it from the others, because a trailing pack
// deduces to empty: <class, auto...> also takes std::complex, with no values, and every
// types-only match would then be ambiguous between two shapes.
//
// A template of a shape not listed, say <class, auto, class>, is false against even
// itself; the shapes are the ones the standard library gives reason to spell.
template<class T, class U>
inline constexpr auto is_same_template_as_v = false;

// types only: std::complex<T>, std::vector<T, A>, std::tuple<Ts...>
template<template<class...> class Primary, class... Ts, class... Us>
inline constexpr auto is_same_template_as_v<Primary<Ts...>, Primary<Us...>> = true;

// values only: std::bitset<N>, std::ratio<N, D>
template<template<auto, auto...> class Primary, auto V, auto... Vs, auto W, auto... Ws>
inline constexpr auto is_same_template_as_v<Primary<V, Vs...>, Primary<W, Ws...>> = true;

// a type, then values: std::array<T, N>, std::span<T, E>
template<template<class, auto, auto...> class Primary, class T, auto V, auto... Vs, class U, auto W, auto... Ws>
inline constexpr auto is_same_template_as_v<Primary<T, V, Vs...>, Primary<U, W, Ws...>> = true;

// a value, then types: no standard template, but the shape a user's may have
template<template<auto, class, class...> class Primary, auto V, class T, class... Ts, auto W, class U, class... Us>
inline constexpr auto is_same_template_as_v<Primary<V, T, Ts...>, Primary<W, U, Us...>> = true;

template<class T, class U>
using is_same_template_as = std::bool_constant<is_same_template_as_v<T, U>>;

} // namespace xstd

#endif // XSTD_MISC_TYPE_TRAITS_IS_SAME_TEMPLATE_AS_HPP
