//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_CONCEPTS_PROXY_ITERATOR_HPP
#define XSTD_MISC_CONCEPTS_PROXY_ITERATOR_HPP

#include <concepts>    // convertible_to, same_as
#include <iterator>    // indirectly_readable, iter_reference_t, iter_value_t
#include <type_traits> // is_class_v

namespace xstd {

// An iterator whose reference is a class prvalue standing in for the value it converts to, as vector<bool>'s does.
template<class I>
concept proxy_iterator = std::indirectly_readable<I> and std::is_class_v<std::iter_reference_t<I>> and (not std::same_as<std::iter_reference_t<I>, std::iter_value_t<I>>) and std::convertible_to<std::iter_reference_t<I>, std::iter_value_t<I>>;

} // namespace xstd

#endif // XSTD_MISC_CONCEPTS_PROXY_ITERATOR_HPP
