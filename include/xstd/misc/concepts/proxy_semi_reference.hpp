//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_CONCEPTS_PROXY_SEMI_REFERENCE_HPP
#define XSTD_MISC_CONCEPTS_PROXY_SEMI_REFERENCE_HPP

#include <concepts>    // convertible_to, same_as
#include <type_traits> // is_class_v

namespace xstd {

// A class standing in for V, which it converts to implicitly, as vector<bool>'s and bitset's references do for bool.
template<class R, class V>
concept proxy_semi_reference = std::is_class_v<R> and (not std::same_as<R, V>) and std::convertible_to<R, V>;

} // namespace xstd

#endif // XSTD_MISC_CONCEPTS_PROXY_SEMI_REFERENCE_HPP
