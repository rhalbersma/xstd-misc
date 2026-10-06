//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_CONCEPTS_PROXY_ITERATOR_HPP
#define XSTD_MISC_CONCEPTS_PROXY_ITERATOR_HPP

#include <xstd/misc/concepts/proxy_reference.hpp>     // proxy_reference
#include <xstd/misc/concepts/proxy_semi_iterator.hpp> // proxy_semi_iterator
#include <iterator>                                   // iter_reference_t

namespace xstd {

// A semi-iterator whose reference is a proxy reference, so the address of what it yields is an iterator like it.
template<class I>
concept proxy_iterator = proxy_semi_iterator<I> and proxy_reference<std::iter_reference_t<I>>;

} // namespace xstd

#endif // XSTD_MISC_CONCEPTS_PROXY_ITERATOR_HPP
