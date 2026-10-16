//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_EXT_BOOST_HASH2_HASH_ALGORITHM_IS_AVALANCHING_HPP
#define XSTD_MISC_EXT_BOOST_HASH2_HASH_ALGORITHM_IS_AVALANCHING_HPP

#include <xstd/misc/ext/boost/hash2/detail/algorithms_fwd.hpp> // blake2b_512, blake2s_256, hmac, md5_128, murmur3_128, murmur3_32, ripemd_128, ripemd_160, sha1_160, sha2_224, sha2_256, sha2_384, sha2_512, sha2_512_224, sha2_512_256, sha3_224, sha3_256, sha3_384, sha3_512, shake_128, shake_256, siphash_32, siphash_64, spooky2_128, xxh3_128, xxhash_32, xxhash_64
#include <type_traits>                                         // false_type, true_type

namespace xstd {

// Whether flipping any bit of H's input flips each bit of its result half the time; specialize it for an algorithm.
template<class H>
struct hash_algorithm_is_avalanching : std::false_type
{};

// Every Hash2 algorithm but FNV-1a, whose multiply only carries upward, passes the Strict Avalanche Criterion.
template<>
struct hash_algorithm_is_avalanching<boost::hash2::blake2b_512> : std::true_type
{};

template<>
struct hash_algorithm_is_avalanching<boost::hash2::blake2s_256> : std::true_type
{};

template<>
struct hash_algorithm_is_avalanching<boost::hash2::md5_128> : std::true_type
{};

template<>
struct hash_algorithm_is_avalanching<boost::hash2::murmur3_128> : std::true_type
{};

template<>
struct hash_algorithm_is_avalanching<boost::hash2::murmur3_32> : std::true_type
{};

template<>
struct hash_algorithm_is_avalanching<boost::hash2::ripemd_128> : std::true_type
{};

template<>
struct hash_algorithm_is_avalanching<boost::hash2::ripemd_160> : std::true_type
{};

template<>
struct hash_algorithm_is_avalanching<boost::hash2::sha1_160> : std::true_type
{};

template<>
struct hash_algorithm_is_avalanching<boost::hash2::sha2_224> : std::true_type
{};

template<>
struct hash_algorithm_is_avalanching<boost::hash2::sha2_256> : std::true_type
{};

template<>
struct hash_algorithm_is_avalanching<boost::hash2::sha2_384> : std::true_type
{};

template<>
struct hash_algorithm_is_avalanching<boost::hash2::sha2_512> : std::true_type
{};

template<>
struct hash_algorithm_is_avalanching<boost::hash2::sha2_512_224> : std::true_type
{};

template<>
struct hash_algorithm_is_avalanching<boost::hash2::sha2_512_256> : std::true_type
{};

template<>
struct hash_algorithm_is_avalanching<boost::hash2::sha3_224> : std::true_type
{};

template<>
struct hash_algorithm_is_avalanching<boost::hash2::sha3_256> : std::true_type
{};

template<>
struct hash_algorithm_is_avalanching<boost::hash2::sha3_384> : std::true_type
{};

template<>
struct hash_algorithm_is_avalanching<boost::hash2::sha3_512> : std::true_type
{};

template<>
struct hash_algorithm_is_avalanching<boost::hash2::shake_128> : std::true_type
{};

template<>
struct hash_algorithm_is_avalanching<boost::hash2::shake_256> : std::true_type
{};

template<>
struct hash_algorithm_is_avalanching<boost::hash2::siphash_32> : std::true_type
{};

template<>
struct hash_algorithm_is_avalanching<boost::hash2::siphash_64> : std::true_type
{};

template<>
struct hash_algorithm_is_avalanching<boost::hash2::spooky2_128> : std::true_type
{};

template<>
struct hash_algorithm_is_avalanching<boost::hash2::xxh3_128> : std::true_type
{};

template<>
struct hash_algorithm_is_avalanching<boost::hash2::xxhash_32> : std::true_type
{};

template<>
struct hash_algorithm_is_avalanching<boost::hash2::xxhash_64> : std::true_type
{};

// HMAC avalanches where the algorithm it keys does.
template<class H>
struct hash_algorithm_is_avalanching<boost::hash2::hmac<H>> : hash_algorithm_is_avalanching<H>
{};

template<class H>
inline constexpr bool hash_algorithm_is_avalanching_v = hash_algorithm_is_avalanching<H>::value;

} // namespace xstd

#endif // XSTD_MISC_EXT_BOOST_HASH2_HASH_ALGORITHM_IS_AVALANCHING_HPP
