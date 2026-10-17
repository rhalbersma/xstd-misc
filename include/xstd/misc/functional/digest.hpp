//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_FUNCTIONAL_DIGEST_HPP
#define XSTD_MISC_FUNCTIONAL_DIGEST_HPP

#include <xstd/misc/concepts/hash_algorithm.hpp>  // hash_algorithm
#include <xstd/misc/functional/detail/hasher.hpp> // hasher
#include <boost/hash2/xxh3.hpp>                   // xxh3_128
#include <string>                                 // basic_string
#include <string_view>                            // basic_string_view

namespace xstd {

// A digest stands in for its key, so XXH3-128 makes two keys colliding never the likeliest bug, at any table size.
template<class T, hash_algorithm H = boost::hash2::xxh3_128>
class digest
        : detail::hasher<H>
{
public:
        using detail::hasher<H>::hasher;
        // NOLINTNEXTLINE(readability-redundant-typename): clang-cl's -Wc++20-compat rejects the bare dependent name.
        using result_type = typename detail::hasher<H>::digest_type;

        [[nodiscard]] constexpr auto operator()(T const& v) const
                -> result_type
        {
                return detail::hasher<H>::digest(v);
        }
};

// A string, its view and a C string append the same characters, so each digests as the key without building one.
template<class CharT, class Traits, class Allocator, hash_algorithm H>
class digest<std::basic_string<CharT, Traits, Allocator>, H>
        : detail::hasher<H>
{
public:
        using detail::hasher<H>::hasher;
        // NOLINTNEXTLINE(readability-redundant-typename): clang-cl's -Wc++20-compat rejects the bare dependent name.
        using result_type = typename detail::hasher<H>::digest_type;

        [[nodiscard]] constexpr auto operator()(std::basic_string_view<CharT, Traits> v) const
                -> result_type
        {
                return detail::hasher<H>::digest(v);
        }
};

} // namespace xstd

#endif // XSTD_MISC_FUNCTIONAL_DIGEST_HPP
