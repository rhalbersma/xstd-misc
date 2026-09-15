//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_CONCEPTS_NONTYPE_SPECIALIZATION_OF_HPP
#define XSTD_MISC_CONCEPTS_NONTYPE_SPECIALIZATION_OF_HPP

#include <xstd/misc/type_traits/is_nontype_specialization_of.hpp> // is_nontype_specialization_of_v
#include <type_traits>                                            // remove_const_t

namespace xstd {

// The constraint spelling of xstd::is_nontype_specialization_of. As in specialization_of, the const
// of an adaptor over a const owner comes off here and nowhere else.
template<class T, template<auto, auto...> class Primary>
concept nontype_specialization_of = is_nontype_specialization_of_v<std::remove_const_t<T>, Primary>;

} // namespace xstd

#endif // XSTD_MISC_CONCEPTS_NONTYPE_SPECIALIZATION_OF_HPP
