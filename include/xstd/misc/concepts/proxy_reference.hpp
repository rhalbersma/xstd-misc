//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_CONCEPTS_PROXY_REFERENCE_HPP
#define XSTD_MISC_CONCEPTS_PROXY_REFERENCE_HPP

#include <xstd/misc/concepts/proxy_semi_iterator.hpp> // proxy_semi_iterator
#include <concepts>                                   // same_as
#include <iterator>                                   // indirectly_writable, iter_reference_t, iter_value_t
#include <utility>                                    // declval

namespace xstd {

// Its address is a semi-iterator back to it that writes as P2321 has vector<bool>'s write: a read-only proxy is none.
template<class R>
concept proxy_reference = requires (R& ref) {
        { &ref } -> proxy_semi_iterator;
} and std::same_as<std::iter_reference_t<decltype(&std::declval<R&>())>, R> and std::indirectly_writable<decltype(&std::declval<R&>()), std::iter_value_t<decltype(&std::declval<R&>())>>;

} // namespace xstd

#endif // XSTD_MISC_CONCEPTS_PROXY_REFERENCE_HPP
