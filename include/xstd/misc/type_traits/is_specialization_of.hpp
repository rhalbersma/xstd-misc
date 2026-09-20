//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_TYPE_TRAITS_IS_SPECIALIZATION_OF_HPP
#define XSTD_MISC_TYPE_TRAITS_IS_SPECIALIZATION_OF_HPP

#include <type_traits> // bool_constant

namespace xstd {

// One per parameter shape, the suffix spelling the kinds in the order a template declares them.

// Whether T is a specialization of a class template whose parameters are all types.
template<class T, template<class...> class Primary>
inline constexpr auto is_specialization_of_T_v = false;

template<template<class...> class Primary, class... Args>
inline constexpr auto is_specialization_of_T_v<Primary<Args...>, Primary> = true;

template<class T, template<class...> class Primary>
using is_specialization_of_T = std::bool_constant<is_specialization_of_T_v<T, Primary>>;

// Whether T is a specialization of a class template whose parameters are all values.
template<class T, template<auto, auto...> class Primary>
inline constexpr auto is_specialization_of_N_v = false;

// At least one value, so that an empty pack cannot make this match where another shape does.
template<template<auto, auto...> class Primary, auto Val, auto... Vals>
inline constexpr auto is_specialization_of_N_v<Primary<Val, Vals...>, Primary> = true;

template<class T, template<auto, auto...> class Primary>
using is_specialization_of_N = std::bool_constant<is_specialization_of_N_v<T, Primary>>;

// A class template taking a type then values, spelt <class U, U...>: <class, auto...> admits std::integer_sequence too.
template<class T, template<class U, U...> class Primary>
inline constexpr auto is_specialization_of_TN_v = false;

// At least one of each, so that an empty value pack cannot make this match a types-only template.
template<template<class U, U...> class Primary, class Arg, auto Val, auto... Vals>
inline constexpr auto is_specialization_of_TN_v<Primary<Arg, Val, Vals...>, Primary> = true;

template<class T, template<class U, U...> class Primary>
using is_specialization_of_TN = std::bool_constant<is_specialization_of_TN_v<T, Primary>>;

// Whether T is a specialization of a class template taking a value and then types.
template<class T, template<auto, class, class...> class Primary>
inline constexpr auto is_specialization_of_NT_v = false;

// At least one of each, so that an empty type pack cannot make this match a values-only template.
template<template<auto, class, class...> class Primary, auto Val, class Arg, class... Args>
inline constexpr auto is_specialization_of_NT_v<Primary<Val, Arg, Args...>, Primary> = true;

template<class T, template<auto, class, class...> class Primary>
using is_specialization_of_NT = std::bool_constant<is_specialization_of_NT_v<T, Primary>>;

// The all-types shape under the name it has without a suffix.
template<class T, template<class...> class Primary>
inline constexpr auto is_specialization_of_v = is_specialization_of_T_v<T, Primary>;

template<class T, template<class...> class Primary>
using is_specialization_of = is_specialization_of_T<T, Primary>;

} // namespace xstd

#endif // XSTD_MISC_TYPE_TRAITS_IS_SPECIALIZATION_OF_HPP
