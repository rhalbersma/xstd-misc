//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_EXT_BOOST_HASH2_HASHER_HPP
#define XSTD_MISC_EXT_BOOST_HASH2_HASHER_HPP

#include <xstd/misc/ext/boost/hash2/hash_algorithm.hpp> // hash_algorithm
#include <boost/hash2/get_integral_result.hpp>          // get_integral_result
#include <boost/hash2/hash_append.hpp>                  // hash_append
#include <boost/hash2/xxhash.hpp>                       // xxhash_64
#include <cstddef>                                      // size_t
#include <cstdint>                                      // uint64_t

namespace xstd {

// xxHash avalanches at any key length; FNV-1a never lets the last byte's high bits reach the low result bits.
template<hash_algorithm H = boost::hash2::xxhash_64>
class hasher
{
        H m_prototype{};

public:
        // No is_transparent: equal values of different types need not append the same bytes, nor hash equal.

        [[nodiscard]] hasher() = default;

        // Not seed, which MSVC's C4459 flags here wherever a consumer declares a seed at namespace scope.
        [[nodiscard]] constexpr explicit hasher(std::uint64_t s)
                : m_prototype(s)
        {}

        [[nodiscard]] constexpr hasher(unsigned char const* p, std::size_t n)
                : m_prototype(p, n)
        {}

        // Neither constrained nor noexcept: Hash2 declares hash_append void for every T, and it runs the user's code.
        template<class T>
        [[nodiscard]] constexpr auto operator()(T const& v) const
                -> std::size_t
        {
                auto h = m_prototype;
                boost::hash2::hash_append(h, {}, v);
                return boost::hash2::get_integral_result<std::size_t>(h);
        }
};

} // namespace xstd

#endif // XSTD_MISC_EXT_BOOST_HASH2_HASHER_HPP
