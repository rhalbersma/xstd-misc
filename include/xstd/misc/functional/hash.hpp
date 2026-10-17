//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_FUNCTIONAL_HASH_HPP
#define XSTD_MISC_FUNCTIONAL_HASH_HPP

#include <xstd/misc/concepts/hash_algorithm.hpp> // hash_algorithm
#include <xstd/misc/functional/hasher.hpp>       // hasher
#include <boost/hash2/xxhash.hpp>                // xxhash_64
#include <cstddef>                               // size_t
#include <string>                                // basic_string
#include <string_view>                           // basic_string_view

namespace xstd {

// The hasher at one key type: each argument converts to T before it is appended, so it hashes as the key it finds.
template<class T, hash_algorithm H = boost::hash2::xxhash_64>
class hash
        : hasher<H>
{
public:
        using hasher<H>::hasher;
        using typename hasher<H>::digest_type;
        using typename hasher<H>::is_avalanching;

        [[nodiscard]] constexpr auto operator()(T const& v) const
                -> std::size_t
        {
                return hasher<H>::operator()(v);
        }

        [[nodiscard]] constexpr auto digest(T const& v) const
                -> digest_type
        {
                return hasher<H>::digest(v);
        }
};

// A string, its view and a C string append the same characters, so a lookup by any of them finds the key.
template<class CharT, class Traits, class Allocator, hash_algorithm H>
class hash<std::basic_string<CharT, Traits, Allocator>, H>
        : hasher<H>
{
public:
        using hasher<H>::hasher;
        using typename hasher<H>::digest_type;
        using typename hasher<H>::is_avalanching;
        using is_transparent = void;

        [[nodiscard]] constexpr auto operator()(std::basic_string_view<CharT, Traits> v) const
                -> std::size_t
        {
                return hasher<H>::operator()(v);
        }

        [[nodiscard]] constexpr auto digest(std::basic_string_view<CharT, Traits> v) const
                -> digest_type
        {
                return hasher<H>::digest(v);
        }
};

} // namespace xstd

#endif // XSTD_MISC_FUNCTIONAL_HASH_HPP
