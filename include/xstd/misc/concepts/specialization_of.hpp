//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_CONCEPTS_SPECIALIZATION_OF_HPP
#define XSTD_MISC_CONCEPTS_SPECIALIZATION_OF_HPP

#include <xstd/misc/type_traits/is_specialization_of.hpp> // is_specialization_of_v
#include <type_traits>                                    // remove_const_t

namespace xstd {

// The constraint spelling of xstd::is_specialization_of.
//
// A view over a const owner names Container const, which no specialization pattern matches, so the
// const comes off here and nowhere else: a constrained parameter is written for what a caller may
// name, and the trait stays the exact question. A reference is not a specialization either way.
template<class T, template<class...> class Primary>
concept specialization_of = is_specialization_of_v<std::remove_const_t<T>, Primary>;

} // namespace xstd

#endif // XSTD_MISC_CONCEPTS_SPECIALIZATION_OF_HPP
