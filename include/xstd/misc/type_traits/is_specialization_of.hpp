//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_TYPE_TRAITS_IS_SPECIALIZATION_OF_HPP
#define XSTD_MISC_TYPE_TRAITS_IS_SPECIALIZATION_OF_HPP

#include <xstd/misc/type_traits/is_specialization_of_T.hpp> // is_specialization_of_T, is_specialization_of_T_v

namespace xstd {

// The all-types shape under the name it has without a suffix.
template<class T, template<class...> class Primary>
inline constexpr auto is_specialization_of_v = is_specialization_of_T_v<T, Primary>;

template<class T, template<class...> class Primary>
using is_specialization_of = is_specialization_of_T<T, Primary>;

} // namespace xstd

#endif // XSTD_MISC_TYPE_TRAITS_IS_SPECIALIZATION_OF_HPP
