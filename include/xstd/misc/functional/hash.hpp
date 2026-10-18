//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_FUNCTIONAL_HASH_HPP
#define XSTD_MISC_FUNCTIONAL_HASH_HPP

#include <xstd/misc/concepts/hash_algorithm.hpp>  // hash_algorithm
#include <xstd/misc/functional/detail/hasher.hpp> // hasher
#include <boost/hash2/flavor.hpp>                 // default_flavor
#include <boost/hash2/xxhash.hpp>                 // xxhash_32, xxhash_64
#include <cstddef>                                // size_t
#include <string>                                 // basic_string
#include <string_view>                            // basic_string_view
#include <type_traits>                            // conditional_t, is_same_v

namespace xstd {

// The default H64: H32 itself, so an algorithm named alone runs everywhere; xxhash_64 beside the default xxhash_32.
template<hash_algorithm H32>
using wide_hash_algorithm_t = std::conditional_t<std::is_same_v<H32, boost::hash2::xxhash_32>, boost::hash2::xxhash_64, H32>;

// xxHash passes SMHasher3's Avalanche test at 3 to 128 bytes; FNV-1a's last byte's high bits never reach the low bits.
template<class T, hash_algorithm H32 = boost::hash2::xxhash_32, hash_algorithm H64 = wide_hash_algorithm_t<H32>, class Flavor = boost::hash2::default_flavor>
        // A pair given in the wrong order would run the wider algorithm where std::size_t is the narrower one.
        requires(sizeof(typename H32::result_type) <= sizeof(typename H64::result_type))
class hash
        : detail::hasher<detail::native_hash_algorithm_t<H32, H64>, Flavor>
{
public:
        using hash_type = detail::native_hash_algorithm_t<H32, H64>;

        using detail::hasher<hash_type, Flavor>::hasher;
        // Declared rather than using-declared: MSVC then denies access to its members through the private base.
        // NOLINTNEXTLINE(readability-redundant-typename): clang-cl's -Wc++20-compat rejects the bare dependent name.
        using is_avalanching = typename detail::hasher<hash_type, Flavor>::is_avalanching;

        [[nodiscard]] constexpr auto operator()(T const& v) const
                -> std::size_t
        {
                return detail::hasher<hash_type, Flavor>::operator()(v);
        }
};

// A string, its view and a C string append the same characters, so a lookup by any of them finds the key.
template<class CharT, class Traits, class Allocator, hash_algorithm H32, hash_algorithm H64, class Flavor>
        requires(sizeof(typename H32::result_type) <= sizeof(typename H64::result_type))
class hash<std::basic_string<CharT, Traits, Allocator>, H32, H64, Flavor>
        : detail::hasher<detail::native_hash_algorithm_t<H32, H64>, Flavor>
{
public:
        using hash_type = detail::native_hash_algorithm_t<H32, H64>;

        using detail::hasher<hash_type, Flavor>::hasher;
        // Declared rather than using-declared: MSVC then denies access to its members through the private base.
        // NOLINTNEXTLINE(readability-redundant-typename): clang-cl's -Wc++20-compat rejects the bare dependent name.
        using is_avalanching = typename detail::hasher<hash_type, Flavor>::is_avalanching;
        using is_transparent = void;

        [[nodiscard]] constexpr auto operator()(std::basic_string_view<CharT, Traits> v) const
                -> std::size_t
        {
                return detail::hasher<hash_type, Flavor>::operator()(v);
        }
};

} // namespace xstd

#endif // XSTD_MISC_FUNCTIONAL_HASH_HPP
