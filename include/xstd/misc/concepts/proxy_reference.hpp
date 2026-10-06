//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_CONCEPTS_PROXY_REFERENCE_HPP
#define XSTD_MISC_CONCEPTS_PROXY_REFERENCE_HPP

#include <xstd/misc/concepts/proxy_iterator.hpp> // proxy_iterator
#include <concepts>                              // same_as
#include <iterator>                              // iter_reference_t
#include <type_traits>                           // is_class_v
#include <utility>                               // declval

namespace xstd {

// A proxy closed under & and *: its address is a proxy iterator that dereferences back to it, naming its value type.
template<class R>
concept proxy_reference = std::is_class_v<R> and requires (R& ref) {
        { &ref } -> proxy_iterator;
} and std::same_as<std::iter_reference_t<decltype(&std::declval<R&>())>, R>;

} // namespace xstd

#endif // XSTD_MISC_CONCEPTS_PROXY_REFERENCE_HPP
