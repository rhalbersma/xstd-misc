//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_EXT_BOOST_HASH2_HPP
#define XSTD_MISC_EXT_BOOST_HASH2_HPP

#include <boost/hash2/fnv1a.hpp>               // fnv1a_32, fnv1a_64
#include <boost/hash2/get_integral_result.hpp> // get_integral_result
#include <boost/hash2/hash_append.hpp>         // hash_append
#include <boost/hash2/xxhash.hpp>              // xxhash_32, xxhash_64
#include <concepts>                            // constructible_from
#include <cstddef>                             // size_t
#include <cstdint>                             // uint64_t
#include <type_traits>                         // conditional_t

namespace xstd {

// Chosen by the width of std::size_t, never by the key: the result fills it, and the CPU multiplies at it.
using short_hash = std::conditional_t<sizeof(std::size_t) == sizeof(std::uint64_t), boost::hash2::fnv1a_64, boost::hash2::fnv1a_32>;
using long_hash  = std::conditional_t<sizeof(std::size_t) == sizeof(std::uint64_t), boost::hash2::xxhash_64, boost::hash2::xxhash_32>;

// How many bytes T appends is T's to say, so the default is the algorithm whose cost per byte holds at any length.
template<class T, class H = long_hash>
class hash
{
        H m_prototype{};

public:
        [[nodiscard]] hash() = default;

        [[nodiscard]] constexpr explicit hash(std::uint64_t seed)
                requires std::constructible_from<H, std::uint64_t>
                : m_prototype(seed)
        {}

        [[nodiscard]] constexpr hash(unsigned char const* p, std::size_t n)
                requires std::constructible_from<H, unsigned char const*, std::size_t>
                : m_prototype(p, n)
        {}

        // No noexcept: Hash2 declares none, and the hash_append of T is the user's own code.
        [[nodiscard]] constexpr auto operator()(T const& v) const
                -> std::size_t
        {
                auto h = m_prototype;
                boost::hash2::hash_append(h, {}, v);
                return boost::hash2::get_integral_result<std::size_t>(h);
        }
};

} // namespace xstd

#endif // XSTD_MISC_EXT_BOOST_HASH2_HPP
