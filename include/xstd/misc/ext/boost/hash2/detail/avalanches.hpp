//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_EXT_BOOST_HASH2_DETAIL_AVALANCHES_HPP
#define XSTD_MISC_EXT_BOOST_HASH2_DETAIL_AVALANCHES_HPP

#include <boost/hash2/siphash.hpp> // siphash_32, siphash_64
#include <boost/hash2/xxh3.hpp>    // xxh3_128
#include <boost/hash2/xxhash.hpp>  // xxhash_32, xxhash_64

namespace xstd::detail {

// False unless named: a table trusting a biased hash piles keys up, where mixing a good one again costs a multiply.
template<class H>
inline constexpr bool avalanches = false;

// Hash2 marks none of its algorithms, so these are named here: xxHash and XXH3 pass SMHasher, and SipHash is a PRF.
template<>
inline constexpr bool avalanches<boost::hash2::xxhash_32> = true;

template<>
inline constexpr bool avalanches<boost::hash2::xxhash_64> = true;

template<>
inline constexpr bool avalanches<boost::hash2::xxh3_128> = true;

template<>
inline constexpr bool avalanches<boost::hash2::siphash_32> = true;

template<>
inline constexpr bool avalanches<boost::hash2::siphash_64> = true;

} // namespace xstd::detail

#endif // XSTD_MISC_EXT_BOOST_HASH2_DETAIL_AVALANCHES_HPP
