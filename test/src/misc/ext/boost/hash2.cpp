//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <test/constexpr_check.hpp> // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_CHECK_NE
#include <array>                    // array
#include <concepts>                 // same_as
#include <cstddef>                  // size_t
#include <cstdint>                  // uint64_t
#include <initializer_list>         // initializer_list
#include <string>                   // string
#include <tuple>                    // tuple, tuple_cat
#include <type_traits>              // is_constructible_v, is_convertible_v
#include <unordered_set>            // unordered_set
#include <utility>                  // declval

// Reached the way a consumer reaches it: the probe here, the adapter behind it.
#if __has_include(<boost/hash2/hash_append.hpp>)
#define TEST_HAS_BOOST_HASH2
#include <xstd/misc/ext/boost/hash2.hpp>  // hash, long_hash, short_hash
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

// Counts the bytes appended and nothing more, with neither seeded constructor a Hash2 algorithm has.
class unseedable
{
        std::uint64_t m_count = 0;

public:
        using result_type = std::uint64_t;

        constexpr auto update(void const*, std::size_t n)
                -> void
        {
                m_count += n;
        }

        [[nodiscard]] constexpr auto result() const
                -> result_type
        {
                return m_count;
        }
};

} // namespace

#endif

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(Ext)
BOOST_AUTO_TEST_SUITE(Boost)
BOOST_AUTO_TEST_SUITE(Hash2)

#ifdef TEST_HAS_BOOST_HASH2

// The width of std::size_t picks each pair's member, so the result fills a size_t whatever the platform.
BOOST_AUTO_TEST_CASE(TheAliasesMatchTheWidthOfSizeT)
{
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::short_hash, boost::hash2::fnv1a_32> or std::same_as<xstd::short_hash, boost::hash2::fnv1a_64>));
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::long_hash, boost::hash2::xxhash_32> or std::same_as<xstd::long_hash, boost::hash2::xxhash_64>));
        XSTD_CONSTEXPR_CHECK(sizeof(xstd::short_hash::result_type) == sizeof(std::size_t));
        XSTD_CONSTEXPR_CHECK(sizeof(xstd::long_hash::result_type) == sizeof(std::size_t));
}

BOOST_AUTO_TEST_CASE(TheDefaultAlgorithmIsLongHash)
{
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::hash<int>, xstd::hash<int, xstd::long_hash>>));
}

// No integer converts to a seed by accident, and an algorithm without a seed declines one rather than failing inside.
BOOST_AUTO_TEST_CASE(SeedingFollowsTheAlgorithm)
{
        XSTD_CONSTEXPR_CHECK((std::is_constructible_v<xstd::hash<int>, std::uint64_t>));
        XSTD_CONSTEXPR_CHECK((not std::is_convertible_v<std::uint64_t, xstd::hash<int>>));
        XSTD_CONSTEXPR_CHECK((std::is_constructible_v<xstd::hash<int>, unsigned char const*, std::size_t>));

        XSTD_CONSTEXPR_CHECK((std::is_default_constructible_v<xstd::hash<int, unseedable>>));
        XSTD_CONSTEXPR_CHECK((not std::is_constructible_v<xstd::hash<int, unseedable>, std::uint64_t>));
        XSTD_CONSTEXPR_CHECK((not std::is_constructible_v<xstd::hash<int, unseedable>, unsigned char const*, std::size_t>));
        BOOST_CHECK_EQUAL((xstd::hash<int, unseedable>()(42)), sizeof(int));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(EqualValuesHashEqual, H, algorithms)
{
        auto const literal = std::string("the quick brown fox");
        auto built         = std::string("the quick");
        built += " brown fox";

        for (auto const& h : {xstd::hash<std::string, H>(), xstd::hash<std::string, H>(seed), xstd::hash<std::string, H>(key.data(), key.size())}) {
                BOOST_CHECK_EQUAL(h(literal), h(built));
        }
}

// The prototype is copied, not consumed: a second call, or a copy of the hasher, starts where the first did.
BOOST_AUTO_TEST_CASE_TEMPLATE(EachCallStartsFromThePrototype, H, algorithms)
{
        auto const h    = xstd::hash<int, H>(seed);
        auto const copy = h;

        BOOST_CHECK_EQUAL(h(42), h(42));
        BOOST_CHECK_EQUAL(copy(42), h(42));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(SeedsChangeTheResult, H, algorithms)
{
        auto const unseeded = xstd::hash<int, H>()(42);
        auto const seeded   = xstd::hash<int, H>(seed)(42);
        auto const keyed    = xstd::hash<int, H>(key.data(), key.size())(42);

        BOOST_CHECK_NE(seeded, unseeded);
        BOOST_CHECK_NE(seeded, (xstd::hash<int, H>(other_seed)(42)));
        BOOST_CHECK_NE(keyed, unseeded);
        BOOST_CHECK_NE(keyed, seeded);
}

// Each constructor is a constant expression; the call is not, as Hash2 1.92 has no constexpr get_integral_result.
BOOST_AUTO_TEST_CASE_TEMPLATE(IsConstructibleInConstantExpressions, H, constexpr_algorithms)
{
        [[maybe_unused]] constexpr auto unseeded = xstd::hash<int, H>();
        [[maybe_unused]] constexpr auto seeded   = xstd::hash<int, H>(seed);
        [[maybe_unused]] constexpr auto keyed    = xstd::hash<int, H>(key.data(), key.size());

        BOOST_CHECK(true); // silence Boost.Test's "test case did not check any assertions"
}

// Hash2's advice for keys an adversary chooses: SipHash, seeded per container.
BOOST_AUTO_TEST_CASE(SeedsAStdUnorderedSet)
{
        using hasher = xstd::hash<std::string, boost::hash2::siphash_64>;

        auto s = std::unordered_set<std::string, hasher>(0, hasher(seed));
        s.insert({"one", "two", "three"});

        BOOST_CHECK_EQUAL(s.size(), std::size_t{3});
        BOOST_CHECK(s.contains("two"));
        BOOST_CHECK(not s.contains("four"));
        BOOST_CHECK_EQUAL(s.hash_function()("two"), hasher(seed)("two"));
        BOOST_CHECK_NE(s.hash_function()("two"), hasher()("two"));
}

#ifdef TEST_HAS_BOOST_UNORDERED

BOOST_AUTO_TEST_CASE(SeedsABoostUnorderedFlatSet)
{
        using hasher = xstd::hash<std::string, boost::hash2::siphash_64>;

        auto s = boost::unordered_flat_set<std::string, hasher>(0, hasher(seed));
        s.insert({"one", "two", "three"});

        BOOST_CHECK_EQUAL(s.size(), std::size_t{3});
        BOOST_CHECK(s.contains("two"));
        BOOST_CHECK(not s.contains("four"));
        BOOST_CHECK_EQUAL(s.hash_function()("two"), hasher(seed)("two"));
        BOOST_CHECK_NE(s.hash_function()("two"), hasher()("two"));
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
