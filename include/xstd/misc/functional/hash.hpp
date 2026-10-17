//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_FUNCTIONAL_HASH_HPP
#define XSTD_MISC_FUNCTIONAL_HASH_HPP

#include <xstd/misc/concepts/hash_algorithm.hpp>  // hash_algorithm
#include <xstd/misc/functional/detail/hasher.hpp> // hasher
#include <boost/hash2/flavor.hpp>                 // default_flavor
#include <boost/hash2/xxhash.hpp>                 // xxhash_64
#include <cstddef>                                // size_t
#include <string>                                 // basic_string
#include <string_view>                            // basic_string_view

namespace xstd {

// xxHash avalanches at any key length; FNV-1a never lets the last byte's high bits reach the low result bits.
template<class T, hash_algorithm H = boost::hash2::xxhash_64, class Flavor = boost::hash2::default_flavor>
class hash
        : detail::hasher<H, Flavor>
{
public:
        using detail::hasher<H, Flavor>::hasher;
        // Declared rather than using-declared: MSVC then denies access to its members through the private base.
        // NOLINTNEXTLINE(readability-redundant-typename): clang-cl's -Wc++20-compat rejects the bare dependent name.
        using is_avalanching = typename detail::hasher<H, Flavor>::is_avalanching;

        [[nodiscard]] constexpr auto operator()(T const& v) const
                -> std::size_t
        {
                return detail::hasher<H, Flavor>::operator()(v);
        }
};

// A string, its view and a C string append the same characters, so a lookup by any of them finds the key.
template<class CharT, class Traits, class Allocator, hash_algorithm H, class Flavor>
class hash<std::basic_string<CharT, Traits, Allocator>, H, Flavor>
        : detail::hasher<H, Flavor>
{
public:
        using detail::hasher<H, Flavor>::hasher;
        // Declared rather than using-declared: MSVC then denies access to its members through the private base.
        // NOLINTNEXTLINE(readability-redundant-typename): clang-cl's -Wc++20-compat rejects the bare dependent name.
        using is_avalanching = typename detail::hasher<H, Flavor>::is_avalanching;
        using is_transparent = void;

        [[nodiscard]] constexpr auto operator()(std::basic_string_view<CharT, Traits> v) const
                -> std::size_t
        {
                return detail::hasher<H, Flavor>::operator()(v);
        }
};

} // namespace xstd

#endif // XSTD_MISC_FUNCTIONAL_HASH_HPP
