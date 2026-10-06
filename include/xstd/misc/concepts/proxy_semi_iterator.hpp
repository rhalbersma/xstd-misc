//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_CONCEPTS_PROXY_SEMI_ITERATOR_HPP
#define XSTD_MISC_CONCEPTS_PROXY_SEMI_ITERATOR_HPP

#include <xstd/misc/concepts/proxy_semi_reference.hpp> // proxy_semi_reference
#include <iterator>                                    // indirectly_readable, iter_reference_t, iter_value_t

namespace xstd {

// An iterator whose reference is a semi-reference for its value type, as vector<bool>'s and zip_view's are.
template<class I>
concept proxy_semi_iterator = std::indirectly_readable<I> and proxy_semi_reference<std::iter_reference_t<I>, std::iter_value_t<I>>;

} // namespace xstd

#endif // XSTD_MISC_CONCEPTS_PROXY_SEMI_ITERATOR_HPP
