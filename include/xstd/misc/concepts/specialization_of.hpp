//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_CONCEPTS_SPECIALIZATION_OF_HPP
#define XSTD_MISC_CONCEPTS_SPECIALIZATION_OF_HPP

#include <xstd/misc/concepts/specialization_of_T.hpp> // specialization_of_T

namespace xstd {

// The all-types shape under the name it has without a suffix, normalizing to the same constraint.
template<class T, template<class...> class Primary>
concept specialization_of = specialization_of_T<T, Primary>;

} // namespace xstd

#endif // XSTD_MISC_CONCEPTS_SPECIALIZATION_OF_HPP
