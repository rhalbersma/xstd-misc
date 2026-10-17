//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_FUNCTIONAL_DIGEST_HPP
#define XSTD_MISC_FUNCTIONAL_DIGEST_HPP

#include <xstd/misc/concepts/hash_algorithm.hpp>  // hash_algorithm
#include <xstd/misc/functional/detail/hasher.hpp> // hasher
#include <boost/hash2/flavor.hpp>                 // little_endian_flavor
#include <boost/hash2/xxh3.hpp>                   // xxh3_128
#include <string>                                 // basic_string
#include <string_view>                            // basic_string_view

namespace xstd {

// A digest stands in for its key on every target: XXH3-128 keeps two keys apart, little-endian bytes keep it portable.
template<class T, hash_algorithm H = boost::hash2::xxh3_128, class Flavor = boost::hash2::little_endian_flavor>
class digest
        : detail::hasher<H, Flavor>
{
public:
        using detail::hasher<H, Flavor>::hasher;
        // NOLINTNEXTLINE(readability-redundant-typename): clang-cl's -Wc++20-compat rejects the bare dependent name.
        using result_type = typename detail::hasher<H, Flavor>::digest_type;

        [[nodiscard]] constexpr auto operator()(T const& v) const
                -> result_type
        {
                return detail::hasher<H, Flavor>::digest(v);
        }
};

// A string, its view and a C string append the same characters, so each digests as the key without building one.
template<class CharT, class Traits, class Allocator, hash_algorithm H, class Flavor>
class digest<std::basic_string<CharT, Traits, Allocator>, H, Flavor>
        : detail::hasher<H, Flavor>
{
public:
        using detail::hasher<H, Flavor>::hasher;
        // NOLINTNEXTLINE(readability-redundant-typename): clang-cl's -Wc++20-compat rejects the bare dependent name.
        using result_type = typename detail::hasher<H, Flavor>::digest_type;

        [[nodiscard]] constexpr auto operator()(std::basic_string_view<CharT, Traits> v) const
                -> result_type
        {
                return detail::hasher<H, Flavor>::digest(v);
        }
};

} // namespace xstd

#endif // XSTD_MISC_FUNCTIONAL_DIGEST_HPP
