//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef TEST_HASH2_ALGORITHMS_HPP
#define TEST_HASH2_ALGORITHMS_HPP

#include <boost/hash2/blake2.hpp>         // blake2b_512, blake2s_256, hmac_blake2b_512, hmac_blake2s_256
#include <boost/hash2/fnv1a.hpp>          // fnv1a_32, fnv1a_64
#include <boost/hash2/legacy/murmur3.hpp> // murmur3_128, murmur3_32
#include <boost/hash2/legacy/spooky2.hpp> // spooky2_128
#include <boost/hash2/md5.hpp>            // hmac_md5_128, md5_128
#include <boost/hash2/ripemd.hpp>         // hmac_ripemd_128, hmac_ripemd_160, ripemd_128, ripemd_160
#include <boost/hash2/sha1.hpp>           // hmac_sha1_160, sha1_160
#include <boost/hash2/sha2.hpp>           // hmac_sha2_224, hmac_sha2_256, hmac_sha2_384, hmac_sha2_512, hmac_sha2_512_224, hmac_sha2_512_256, sha2_224, sha2_256, sha2_384, sha2_512, sha2_512_224, sha2_512_256
#include <boost/hash2/sha3.hpp>           // hmac_sha3_224, hmac_sha3_256, hmac_sha3_384, hmac_sha3_512, sha3_224, sha3_256, sha3_384, sha3_512, shake_128, shake_256
#include <boost/hash2/siphash.hpp>        // siphash_32, siphash_64
#include <boost/hash2/xxh3.hpp>           // xxh3_128
#include <boost/hash2/xxhash.hpp>         // xxhash_32, xxhash_64
#include <tuple>                          // tuple, tuple_cat
#include <utility>                        // declval

namespace test {

// Not under clang-tidy: its analyzer reports a read past the buffer of xxh3_128 on a path no byte count reaches.
#ifdef __clang_analyzer__
using xxh3_unless_analyzed = std::tuple<>;
#else
using xxh3_unless_analyzed = std::tuple<boost::hash2::xxh3_128>;
#endif

// Hash2's algorithms outside legacy/, each constructible in a constant expression, less xxh3_128.
using constexpr_algorithms_but_xxh3 = std::tuple<
        boost::hash2::fnv1a_32, boost::hash2::fnv1a_64,
        boost::hash2::xxhash_32, boost::hash2::xxhash_64,
        boost::hash2::siphash_32, boost::hash2::siphash_64,
        boost::hash2::md5_128, boost::hash2::sha1_160,
        boost::hash2::sha2_224, boost::hash2::sha2_256, boost::hash2::sha2_384,
        boost::hash2::sha2_512, boost::hash2::sha2_512_224, boost::hash2::sha2_512_256,
        boost::hash2::sha3_224, boost::hash2::sha3_256, boost::hash2::sha3_384, boost::hash2::sha3_512,
        boost::hash2::shake_128, boost::hash2::shake_256,
        boost::hash2::ripemd_128, boost::hash2::ripemd_160,
        boost::hash2::blake2b_512, boost::hash2::blake2s_256,
        boost::hash2::hmac_md5_128, boost::hash2::hmac_sha1_160,
        boost::hash2::hmac_sha2_224, boost::hash2::hmac_sha2_256, boost::hash2::hmac_sha2_384,
        boost::hash2::hmac_sha2_512, boost::hash2::hmac_sha2_512_224, boost::hash2::hmac_sha2_512_256,
        boost::hash2::hmac_sha3_224, boost::hash2::hmac_sha3_256, boost::hash2::hmac_sha3_384, boost::hash2::hmac_sha3_512,
        boost::hash2::hmac_ripemd_128, boost::hash2::hmac_ripemd_160,
        boost::hash2::hmac_blake2b_512, boost::hash2::hmac_blake2s_256>;

using constexpr_algorithms = decltype(std::tuple_cat(std::declval<constexpr_algorithms_but_xxh3>(), std::declval<xxh3_unless_analyzed>()));

// The legacy algorithms seed through defaulted arguments rather than one constructor per form.
using legacy_algorithms = std::tuple<boost::hash2::murmur3_32, boost::hash2::murmur3_128, boost::hash2::spooky2_128>;

using algorithms = decltype(std::tuple_cat(std::declval<constexpr_algorithms>(), std::declval<legacy_algorithms>()));

// The algorithms whose hasher reports itself avalanching, so that Boost.Unordered uses its hash without a post-mix.
using avalanching_algorithms_but_xxh3 = std::tuple<boost::hash2::xxhash_32, boost::hash2::xxhash_64, boost::hash2::siphash_32, boost::hash2::siphash_64>;

using avalanching_algorithms = decltype(std::tuple_cat(std::declval<avalanching_algorithms_but_xxh3>(), std::declval<xxh3_unless_analyzed>()));

} // namespace test

#endif // TEST_HASH2_ALGORITHMS_HPP
