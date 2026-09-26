//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_CONCEPTS_SPECIALIZATION_OF_HPP
#define XSTD_MISC_CONCEPTS_SPECIALIZATION_OF_HPP

#include <xstd/misc/type_traits/is_specialization_of.hpp> // is_specialization_of_v
#include <type_traits>                                    // remove_const_t

namespace xstd {

// The constraint spelling of the trait, taking the const an adaptor over a const owner names.
template<class T, template<class...> class Primary>
concept specialization_of = is_specialization_of_v<std::remove_const_t<T>, Primary>;

} // namespace xstd

#endif // XSTD_MISC_CONCEPTS_SPECIALIZATION_OF_HPP
