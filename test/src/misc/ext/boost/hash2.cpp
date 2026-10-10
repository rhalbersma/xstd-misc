//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <test/constexpr_check.hpp> // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_CHECK_NE
#include <array>                    // array
#include <concepts>                 // same_as
#include <cstddef>                  // size_t
#include <cstdint>                  // int64_t, uint64_t, uint8_t
#include <functional>               // equal_to, hash
#include <initializer_list>         // initializer_list
#include <string>                   // string
#include <tuple>                    // tuple, tuple_cat
#include <type_traits>              // is_constructible_v, is_convertible_v
#include <unordered_set>            // unordered_set
#include <utility>                  // declval, pair
#include <vector>                   // vector

// Reached the way a consumer reaches it: the probe here, the adapter behind it.
#if __has_include(<boost/hash2/hash_append.hpp>)
#define TEST_HAS_BOOST_HASH2
#include <xstd/misc/ext/boost/hash2.hpp>       // hash_algorithm, hasher
#include <boost/hash2/blake2.hpp>              // blake2b_512, blake2s_256, hmac_blake2b_512, hmac_blake2s_256
#include <boost/hash2/digest.hpp>              // digest
#include <boost/hash2/fnv1a.hpp>               // fnv1a_32, fnv1a_64
#include <boost/hash2/get_integral_result.hpp> // get_integral_result
#include <boost/hash2/hash_append.hpp>         // hash_append
#include <boost/hash2/legacy/murmur3.hpp>      // murmur3_128, murmur3_32
#include <boost/hash2/legacy/spooky2.hpp>      // spooky2_128
#include <boost/hash2/md5.hpp>                 // hmac_md5_128, md5_128
#include <boost/hash2/ripemd.hpp>              // hmac_ripemd_128, hmac_ripemd_160, ripemd_128, ripemd_160
#include <boost/hash2/sha1.hpp>                // hmac_sha1_160, sha1_160
#include <boost/hash2/sha2.hpp>                // hmac_sha2_224, hmac_sha2_256, hmac_sha2_384, hmac_sha2_512, hmac_sha2_512_224, hmac_sha2_512_256, sha2_224, sha2_256, sha2_384, sha2_512, sha2_512_224, sha2_512_256
#include <boost/hash2/sha3.hpp>                // hmac_sha3_224, hmac_sha3_256, hmac_sha3_384, hmac_sha3_512, sha3_224, sha3_256, sha3_384, sha3_512, shake_128, shake_256
#include <boost/hash2/siphash.hpp>             // siphash_32, siphash_64
#include <boost/hash2/xxh3.hpp>                // xxh3_128
#include <boost/hash2/xxhash.hpp>              // xxhash_32, xxhash_64
#endif

#if __has_include(<boost/unordered/unordered_flat_set.hpp>)
#define TEST_HAS_BOOST_UNORDERED
#include <boost/unordered/unordered_flat_set.hpp> // unordered_flat_set
#endif

#ifdef TEST_HAS_BOOST_HASH2

namespace {

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

constexpr auto seed       = std::uint64_t{0x9e37'79b9'7f4a'7c15};
constexpr auto other_seed = std::uint64_t{0xc2b2'ae3d'27d4'eb4f};
constexpr auto key        = std::array<unsigned char, 16>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

template<class H>
concept has_block_size = requires { H::block_size; };

// Whether a hasher over H can be named at all, rather than failing to compile.
template<class H>
concept hasher_argument = requires { typename xstd::hasher<H>; };

template<class Hash>
concept transparent = requires { typename Hash::is_transparent; };

// What Hash2 makes of a key, with no hasher in between.
template<class H, class T>
auto hash2_of(H algorithm, T const& v)
        -> std::size_t
{
        boost::hash2::hash_append(algorithm, {}, v);
        return boost::hash2::get_integral_result<std::size_t>(algorithm);
}

} // namespace

#endif

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(Ext)
BOOST_AUTO_TEST_SUITE(Boost)
BOOST_AUTO_TEST_SUITE(Hash2)

#ifdef TEST_HAS_BOOST_HASH2

BOOST_AUTO_TEST_CASE_TEMPLATE(EveryHash2AlgorithmIsAHashAlgorithm, H, algorithms)
{
        XSTD_CONSTEXPR_CHECK(xstd::hash_algorithm<H>);
}

// An unsigned integer or a fixed-size byte array for a result, a block size or none: Hash2 has each form.
BOOST_AUTO_TEST_CASE(EachFormHash2TakesIsAHashAlgorithm)
{
        XSTD_CONSTEXPR_CHECK((std::same_as<boost::hash2::fnv1a_32::result_type, std::uint32_t>));
        XSTD_CONSTEXPR_CHECK((std::same_as<boost::hash2::md5_128::result_type, boost::hash2::digest<16>>));
        XSTD_CONSTEXPR_CHECK((std::same_as<boost::hash2::murmur3_128::result_type, std::array<unsigned char, 16>>));
        XSTD_CONSTEXPR_CHECK(not has_block_size<boost::hash2::fnv1a_32>);
        XSTD_CONSTEXPR_CHECK(has_block_size<boost::hash2::md5_128>);

        XSTD_CONSTEXPR_CHECK(xstd::hash_algorithm<boost::hash2::fnv1a_32>);
        XSTD_CONSTEXPR_CHECK(xstd::hash_algorithm<boost::hash2::md5_128>);
        XSTD_CONSTEXPR_CHECK(xstd::hash_algorithm<boost::hash2::murmur3_128>);
}

// A hasher is the mistake to expect: each hashes a key, and xstd::hasher even takes both seeds, yet neither updates.
BOOST_AUTO_TEST_CASE(AHasherIsNotAHashAlgorithm)
{
        XSTD_CONSTEXPR_CHECK(not xstd::hash_algorithm<std::hash<int>>);
        XSTD_CONSTEXPR_CHECK(not xstd::hash_algorithm<xstd::hasher<>>);
}

BOOST_AUTO_TEST_CASE(TheDefaultAlgorithmIsXxhash64)
{
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::hasher<>, xstd::hasher<boost::hash2::xxhash_64>>));
}

// A table that masks the low bits sees FNV-1a ignore a key's top bit; the default sees it.
BOOST_AUTO_TEST_CASE(TheDefaultLetsTheTopBitReachTheLowBits)
{
        constexpr auto low_bits = std::size_t{0x7F};
        auto const fnv1a        = xstd::hasher<boost::hash2::fnv1a_64>();
        auto const by_default   = xstd::hasher<>();
        BOOST_CHECK_EQUAL(fnv1a(std::uint8_t{0x00}) & low_bits, fnv1a(std::uint8_t{0x80}) & low_bits);
        BOOST_CHECK_NE(by_default(std::uint8_t{0x00}) & low_bits, by_default(std::uint8_t{0x80}) & low_bits);
}

// A hasher names its algorithm, and only something that is one.
BOOST_AUTO_TEST_CASE(TheAlgorithmIsAHashAlgorithm)
{
        XSTD_CONSTEXPR_CHECK(hasher_argument<boost::hash2::xxhash_64>);
        XSTD_CONSTEXPR_CHECK(not hasher_argument<std::hash<int>>);
        XSTD_CONSTEXPR_CHECK(not hasher_argument<xstd::hasher<>>);
}

// No integer converts to a seed by accident.
BOOST_AUTO_TEST_CASE(SeedsAreExplicit)
{
        XSTD_CONSTEXPR_CHECK((std::is_constructible_v<xstd::hasher<>, std::uint64_t>));
        XSTD_CONSTEXPR_CHECK((not std::is_convertible_v<std::uint64_t, xstd::hasher<>>));
        XSTD_CONSTEXPR_CHECK((std::is_constructible_v<xstd::hasher<>, unsigned char const*, std::size_t>));
}

// One hasher, no key type of its own: each call hashes its argument as Hash2 would.
BOOST_AUTO_TEST_CASE_TEMPLATE(OneHasherHashesKeysOfEveryType, H, algorithms)
{
        auto const h       = xstd::hasher<H>(seed);
        auto const word    = std::string("word");
        auto const numbers = std::vector<int>{1, 2, 3};
        auto const entry   = std::pair<int, std::string>(42, "word");

        BOOST_CHECK_EQUAL(h(42), hash2_of(H(seed), 42));
        BOOST_CHECK_EQUAL(h(2.5), hash2_of(H(seed), 2.5));
        BOOST_CHECK_EQUAL(h(word), hash2_of(H(seed), word));
        BOOST_CHECK_EQUAL(h(numbers), hash2_of(H(seed), numbers));
        BOOST_CHECK_EQUAL(h(entry), hash2_of(H(seed), entry));
}

// 42 and std::int64_t{42} compare equal but append four bytes and eight, so a lookup by one would miss the other.
BOOST_AUTO_TEST_CASE(IsNotTransparent)
{
        XSTD_CONSTEXPR_CHECK(transparent<std::equal_to<>>);
        XSTD_CONSTEXPR_CHECK(not transparent<xstd::hasher<>>);

        auto const h = xstd::hasher<>();
        BOOST_CHECK_NE(h(42), h(std::int64_t{42}));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(EqualValuesHashEqual, H, algorithms)
{
        auto const literal = std::string("the quick brown fox");
        auto built         = std::string("the quick");
        built += " brown fox";

        for (auto const& h : {xstd::hasher<H>(), xstd::hasher<H>(seed), xstd::hasher<H>(key.data(), key.size())}) {
                BOOST_CHECK_EQUAL(h(literal), h(built));
        }
}

// The prototype is copied, not consumed: a second call, or a copy of the hasher, starts where the first did.
BOOST_AUTO_TEST_CASE_TEMPLATE(EachCallStartsFromThePrototype, H, algorithms)
{
        auto const h    = xstd::hasher<H>(seed);
        auto const copy = h;

        BOOST_CHECK_EQUAL(h(42), h(42));
        BOOST_CHECK_EQUAL(copy(42), h(42));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(SeedsChangeTheResult, H, algorithms)
{
        auto const unseeded_hasher = xstd::hasher<H>();
        auto const seeded_hasher   = xstd::hasher<H>(seed);
        auto const reseeded_hasher = xstd::hasher<H>(other_seed);
        auto const keyed_hasher    = xstd::hasher<H>(key.data(), key.size());

        auto const unseeded = unseeded_hasher(42);
        auto const seeded   = seeded_hasher(42);
        auto const keyed    = keyed_hasher(42);

        BOOST_CHECK_NE(seeded, unseeded);
        BOOST_CHECK_NE(seeded, reseeded_hasher(42));
        BOOST_CHECK_NE(keyed, unseeded);
        BOOST_CHECK_NE(keyed, seeded);
}

// Each constructor is a constant expression; the call is not, as Hash2 1.92 has no constexpr get_integral_result.
BOOST_AUTO_TEST_CASE_TEMPLATE(IsConstructibleInConstantExpressions, H, constexpr_algorithms)
{
        [[maybe_unused]] constexpr auto unseeded = xstd::hasher<H>();
        [[maybe_unused]] constexpr auto seeded   = xstd::hasher<H>(seed);
        [[maybe_unused]] constexpr auto keyed    = xstd::hasher<H>(key.data(), key.size());

        BOOST_CHECK(true); // silence Boost.Test's "test case did not check any assertions"
}

// Hash2's advice for keys an adversary chooses: SipHash, seeded per container.
BOOST_AUTO_TEST_CASE(SeedsAStdUnorderedSet)
{
        using siphasher = xstd::hasher<boost::hash2::siphash_64>;

        auto s = std::unordered_set<std::string, siphasher>(0, siphasher(seed));
        s.insert({"one", "two", "three"});

        auto const two      = std::string("two");
        auto const used     = s.hash_function();
        auto const seeded   = siphasher(seed);
        auto const unseeded = siphasher();

        BOOST_CHECK_EQUAL(s.size(), std::size_t{3});
        BOOST_CHECK(s.contains("two"));
        BOOST_CHECK(not s.contains("four"));
        BOOST_CHECK_EQUAL(used(two), seeded(two));
        BOOST_CHECK_NE(used(two), unseeded(two));
}

#ifdef TEST_HAS_BOOST_UNORDERED

BOOST_AUTO_TEST_CASE(SeedsABoostUnorderedFlatSet)
{
        using siphasher = xstd::hasher<boost::hash2::siphash_64>;

        auto s = boost::unordered_flat_set<std::string, siphasher>(0, siphasher(seed));
        s.insert({"one", "two", "three"});

        auto const two      = std::string("two");
        auto const used     = s.hash_function();
        auto const seeded   = siphasher(seed);
        auto const unseeded = siphasher();

        BOOST_CHECK_EQUAL(s.size(), std::size_t{3});
        BOOST_CHECK(s.contains("two"));
        BOOST_CHECK(not s.contains("four"));
        BOOST_CHECK_EQUAL(used(two), seeded(two));
        BOOST_CHECK_NE(used(two), unseeded(two));
}

#endif

#else

BOOST_AUTO_TEST_CASE(NeedsBoostHash2)
{
        BOOST_CHECK(true);
}

#endif

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
