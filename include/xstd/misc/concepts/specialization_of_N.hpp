//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_CONCEPTS_SPECIALIZATION_OF_N_HPP
#define XSTD_MISC_CONCEPTS_SPECIALIZATION_OF_N_HPP

#include <xstd/misc/type_traits/is_specialization_of_N.hpp> // is_specialization_of_N_v
#include <type_traits>                                      // remove_const_t

namespace xstd {

// The constraint spelling of xstd::is_specialization_of_N, taking a const owner as specialization_of does.
template<class T, template<auto, auto...> class Primary>
concept specialization_of_N = is_specialization_of_N_v<std::remove_const_t<T>, Primary>;

} // namespace xstd

#endif // XSTD_MISC_CONCEPTS_SPECIALIZATION_OF_N_HPP
