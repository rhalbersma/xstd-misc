//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_CONCEPTS_SAME_TEMPLATE_AS_HPP
#define XSTD_MISC_CONCEPTS_SAME_TEMPLATE_AS_HPP

#include <xstd/misc/type_traits/is_same_template_as.hpp> // is_same_template_as_v

namespace xstd {

// The constraint spelling of xstd::is_same_template_as. As a type-constraint it reads
// as the relation does, the constrained type first: same_template_as<std::array<int, 1>> T.
template<class T, class U>
concept same_template_as = is_same_template_as_v<T, U>;

} // namespace xstd

#endif // XSTD_MISC_CONCEPTS_SAME_TEMPLATE_AS_HPP
