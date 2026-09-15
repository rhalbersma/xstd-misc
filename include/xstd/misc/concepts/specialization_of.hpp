//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_CONCEPTS_SPECIALIZATION_OF_HPP
#define XSTD_MISC_CONCEPTS_SPECIALIZATION_OF_HPP

#include <xstd/misc/type_traits/is_specialization_of.hpp> // is_specialization_of_N_v, is_specialization_of_TN_v, is_specialization_of_T_v
#include <type_traits>                                    // remove_const_t

namespace xstd {

// The constraint spellings of the traits, each taking the const an adaptor over a const owner names.
template<class T, template<class...> class Primary>
concept specialization_of_T = is_specialization_of_T_v<std::remove_const_t<T>, Primary>;

template<class T, template<auto, auto...> class Primary>
concept specialization_of_N = is_specialization_of_N_v<std::remove_const_t<T>, Primary>;

template<class T, template<class, auto, auto...> class Primary>
concept specialization_of_TN = is_specialization_of_TN_v<std::remove_const_t<T>, Primary>;

// The all-types shape under the name it has without a suffix, normalizing to the same constraint.
template<class T, template<class...> class Primary>
concept specialization_of = specialization_of_T<T, Primary>;

} // namespace xstd

#endif // XSTD_MISC_CONCEPTS_SPECIALIZATION_OF_HPP
