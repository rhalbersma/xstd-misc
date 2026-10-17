//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/concepts/hash_algorithm.hpp>                   // hash_algorithm
#include <xstd/misc/functional/hasher.hpp>                         // hasher
#include <xstd/misc/type_traits/hash_algorithm_is_avalanching.hpp> // hash_algorithm_is_avalanching_v
#include <test/constexpr_check.hpp>                                // XSTD_CONSTEXPR_CHECK
#include <test/hash2_algorithms.hpp>                               // algorithms, constexpr_algorithms
#include <boost/hash2/digest.hpp>                                  // digest, to_string
#include <boost/hash2/fnv1a.hpp>                                   // fnv1a_32, fnv1a_64
#include <boost/hash2/get_integral_result.hpp>                     // get_integral_result
#include <boost/hash2/hash_append.hpp>                             // hash_append
#include <boost/hash2/siphash.hpp>                                 // siphash_64
#include <boost/hash2/xxh3.hpp>                                    // xxh3_128
#include <boost/hash2/xxhash.hpp>                                  // xxhash_64
#include <boost/test/unit_test.hpp>                                // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_CHECK_GT, BOOST_CHECK_NE
#include <array>                                                   // array
#include <bit>                                                     // endian, has_single_bit
#include <concepts>                                                // same_as
#include <cstddef>                                                 // size_t
#include <cstdint>                                                 // int64_t, uint32_t, uint64_t
#include <functional>                                              // equal_to, hash
#include <initializer_list>                                        // initializer_list
#include <limits>                                                  // numeric_limits
#include <set>                                                     // set
#include <string>                                                  // string
#include <thread>                                                  // thread
#include <type_traits>                                             // conditional_t, is_constructible_v, is_convertible_v
#include <unordered_set>                                           // unordered_set
#include <utility>                                                 // declval, pair
#include <vector>                                                  // vector

#if __has_include(<boost/unordered/unordered_flat_set.hpp>)
#define TEST_HAS_BOOST_UNORDERED
#include <boost/container_hash/hash_is_avalanching.hpp> // hash_is_avalanching
#include <boost/unordered/unordered_flat_set.hpp>       // unordered_flat_set
#endif

namespace {

constexpr auto seed       = std::uint64_t{0x9e37'79b9'7f4a'7c15};
constexpr auto other_seed = std::uint64_t{0xc2b2'ae3d'27d4'eb4f};
constexpr auto key        = std::array<unsigned char, 16>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

// Whether a hasher over H can be named at all, rather than failing to compile.
template<class H>
concept hasher_argument = requires { typename xstd::hasher<H>; };

template<class Hash>
concept transparent = requires { typename Hash::is_transparent; };

// Sixteen IDs that differ only in a tag in their top four bits.
constexpr auto tagged_ids = []() -> std::array<std::uint64_t, 16> {
        auto ids = std::array<std::uint64_t, 16>();
        for (auto i = std::size_t{0}; i < ids.size(); ++i) {
                ids[i] = static_cast<std::uint64_t>(i) << 60;
        }
        return ids;
}();

// The bucket count of a standard table asked for 8 and never growing, and how many of them the tagged IDs occupy.
template<class Hash>
auto bucket_spread()
        -> std::pair<std::size_t, std::size_t>
{
        auto table = std::unordered_set<std::uint64_t, Hash>();
        table.max_load_factor(static_cast<float>(tagged_ids.size()));
        table.rehash(8);
        table.insert(tagged_ids.begin(), tagged_ids.end());
        auto occupied = std::set<std::size_t>();
        for (auto const id : tagged_ids) {
                occupied.insert(table.bucket(id));
        }
        return {table.bucket_count(), occupied.size()};
}

// What Hash2 makes of a key, with no hasher in between.
template<class H, class T>
auto hash2_of(H algorithm, T const& v)
        -> std::size_t
{
        boost::hash2::hash_append(algorithm, {}, v);
        return boost::hash2::get_integral_result<std::size_t>(algorithm);
}

} // namespace

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(Functional)
BOOST_AUTO_TEST_SUITE(Hasher)

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

// FNV-1a carries no bit of a byte down to the bits below it, so masking its 64-bit hash erases the tag.
BOOST_AUTO_TEST_CASE(TaggedIdsShareABucketUnderFnv1aWhereATableMasksTheirHash)
{
        auto const [bucket_count, fnv1a_buckets]    = bucket_spread<xstd::hasher<boost::hash2::fnv1a_64>>();
        auto const [default_count, default_buckets] = bucket_spread<xstd::hasher<>>();
        BOOST_CHECK_EQUAL(default_count, bucket_count);
#if defined(_LIBCPP_VERSION) || defined(_MSVC_STL_VERSION)
        BOOST_CHECK(std::has_single_bit(bucket_count));
#endif
#ifdef __GLIBCXX__
        BOOST_CHECK(not std::has_single_bit(bucket_count));
#endif
        // A prime count, as libstdc++ rounds up to, or the multiply-high fold to a 32-bit size_t mixes the tag back in.
        if (std::has_single_bit(bucket_count) and std::numeric_limits<std::size_t>::digits >= 64) {
                BOOST_CHECK_EQUAL(fnv1a_buckets, 1U);
        } else {
                BOOST_CHECK_GT(fnv1a_buckets, 1U);
        }
        BOOST_CHECK_GT(default_buckets, bucket_count / 2);
}

// The 64-bit digest itself where std::size_t holds it, and get_integral_result's multiply-high fold where it is 32.
BOOST_AUTO_TEST_CASE(IsOneValuePerWidthOfSizeT)
{
        if constexpr (std::endian::native == std::endian::little) {
                auto const h       = xstd::hasher<>(seed);
                auto const at_size = [](std::uint64_t digest, std::uint32_t fold) -> std::size_t {
                        if constexpr (std::numeric_limits<std::size_t>::digits >= 64) {
                                return static_cast<std::size_t>(digest);
                        } else {
                                return fold;
                        }
                };
                BOOST_CHECK_EQUAL(h(std::uint64_t{42}), at_size(0xf92a'4389'bd62'ee57, 0xb9d9'e2fa));
                BOOST_CHECK_EQUAL(h(std::string("r1/p1/...")), at_size(0xd7e9'ed42'f1d4'b1ce, 0xab90'41e8));
        } else {
                BOOST_CHECK(true); // silence Boost.Test's "test case did not check any assertions"
        }
}

// Boost.Unordered reads the algorithm's answer through the hasher, and skips its post-mix only where it is true.
BOOST_AUTO_TEST_CASE_TEMPLATE(ReportsWhetherItsAlgorithmAvalanches, H, test::algorithms)
{
        XSTD_CONSTEXPR_CHECK((xstd::hasher<H>::is_avalanching::value == xstd::hash_algorithm_is_avalanching_v<H>));
#ifdef TEST_HAS_BOOST_UNORDERED
        XSTD_CONSTEXPR_CHECK((boost::hash_is_avalanching<xstd::hasher<H>>::value == xstd::hash_algorithm_is_avalanching_v<H>));
#endif
}

// The parameter is the algorithm: a key type, as std::hash takes, is the habit to expect, and is turned away.
BOOST_AUTO_TEST_CASE(TheAlgorithmIsAHashAlgorithm)
{
        XSTD_CONSTEXPR_CHECK(hasher_argument<boost::hash2::xxhash_64>);
        XSTD_CONSTEXPR_CHECK(not hasher_argument<std::hash<int>>);
        XSTD_CONSTEXPR_CHECK(not hasher_argument<std::string>);
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
BOOST_AUTO_TEST_CASE_TEMPLATE(OneHasherHashesKeysOfEveryType, H, test::algorithms)
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

// Hash2 declares hash_append void for every key, so a constraint on the call admits std::thread::id, which it rejects.
BOOST_AUTO_TEST_CASE(HashAppendIsDeclaredForKeysHash2CannotHash)
{
        XSTD_CONSTEXPR_CHECK((std::same_as<decltype(boost::hash2::hash_append(std::declval<boost::hash2::xxhash_64&>(), {}, std::declval<std::thread::id const&>())), void>));
}

// 42 and std::int64_t{42} compare equal but append four bytes and eight, so a lookup by one would miss the other.
BOOST_AUTO_TEST_CASE(IsNotTransparent)
{
        XSTD_CONSTEXPR_CHECK(transparent<std::equal_to<>>);
        XSTD_CONSTEXPR_CHECK(not transparent<xstd::hasher<>>);

        auto const h = xstd::hasher<>();
        BOOST_CHECK_NE(h(42), h(std::int64_t{42}));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(EqualValuesHashEqual, H, test::algorithms)
{
        auto const literal = std::string("the quick brown fox");
        auto built         = std::string("the quick");
        built += " brown fox";

        for (auto const& h : {xstd::hasher<H>(), xstd::hasher<H>(seed), xstd::hasher<H>(key.data(), key.size())}) {
                BOOST_CHECK_EQUAL(h(literal), h(built));
        }
}

// The prototype is copied, not consumed: a second call, or a copy of the hasher, starts where the first did.
BOOST_AUTO_TEST_CASE_TEMPLATE(EachCallStartsFromThePrototype, H, test::algorithms)
{
        auto const h    = xstd::hasher<H>(seed);
        auto const copy = h;

        BOOST_CHECK_EQUAL(h(42), h(42));
        BOOST_CHECK_EQUAL(copy(42), h(42));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(SeedsChangeTheResult, H, test::algorithms)
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
BOOST_AUTO_TEST_CASE_TEMPLATE(IsConstructibleInConstantExpressions, H, test::constexpr_algorithms)
{
        [[maybe_unused]] constexpr auto unseeded = xstd::hasher<H>();
        [[maybe_unused]] constexpr auto seeded   = xstd::hasher<H>(seed);
        [[maybe_unused]] constexpr auto keyed    = xstd::hasher<H>(key.data(), key.size());

        BOOST_CHECK(true); // silence Boost.Test's "test case did not check any assertions"
}

// A digest is as wide as its algorithm makes it, the same on every target, where the call's is std::size_t.
BOOST_AUTO_TEST_CASE(ADigestIsTheAlgorithmsWholeResult)
{
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::hasher<>::digest_type, std::uint64_t>));
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::hasher<boost::hash2::xxh3_128>::digest_type, boost::hash2::digest<16>>));
        XSTD_CONSTEXPR_CHECK((std::same_as<decltype(xstd::hasher<boost::hash2::fnv1a_32>().digest(42)), std::uint32_t>));
}

// The digest is what Hash2 gives for the key appended to the seeded algorithm, and seeds tell digests apart.
BOOST_AUTO_TEST_CASE_TEMPLATE(ADigestIsWhatHash2Gives, H, test::algorithms)
{
        auto const by_hand = [](H algorithm, auto const& v) {
                boost::hash2::hash_append(algorithm, {}, v);
                return algorithm.result();
        };
        auto const pair_key = std::pair{std::string("piece"), 42};
        BOOST_CHECK(xstd::hasher<H>(seed).digest(pair_key) == by_hand(H(seed), pair_key));
        BOOST_CHECK(xstd::hasher<H>(seed).digest(pair_key) != xstd::hasher<H>(other_seed).digest(pair_key));
}

// Where the digest is as wide as std::size_t, the call returns it unchanged; elsewhere Hash2 narrows or widens it.
BOOST_AUTO_TEST_CASE(TheCallIsTheDigestAtTheWidthOfSizeT)
{
        using fnv1a_word      = std::conditional_t<sizeof(std::size_t) == sizeof(std::uint64_t), boost::hash2::fnv1a_64, boost::hash2::fnv1a_32>;
        auto const h          = xstd::hasher<fnv1a_word>(seed);
        auto const string_key = std::string("piece");
        BOOST_CHECK_EQUAL(h(string_key), static_cast<std::size_t>(h.digest(string_key)));
}

// Reference xxHash digests of the bytes Hash2 appends: little-endian integers, a string's bytes then a uint32 size.
BOOST_AUTO_TEST_CASE(PersistedDigestsMatchTheReferenceImplementation)
{
        if constexpr (std::endian::native == std::endian::little) {
                auto const xxh3_128  = xstd::hasher<boost::hash2::xxh3_128>(seed);
                auto const xxhash_64 = xstd::hasher<boost::hash2::xxhash_64>(seed);

                BOOST_CHECK_EQUAL(boost::hash2::to_string(xxh3_128.digest(std::uint64_t{42})), "c8ad00538ea09cd0bf9c129cd1b5be7c");
                BOOST_CHECK_EQUAL(boost::hash2::to_string(xxh3_128.digest(std::uint32_t{42})), "1e4cc47a745f03cc0e89e603d693903f");
                BOOST_CHECK_EQUAL(boost::hash2::to_string(xxh3_128.digest(std::string("r1/p1/..."))), "ef4ce4cfdd731616a44b2e886dd62750");
                BOOST_CHECK_EQUAL(boost::hash2::to_string(xxh3_128.digest(std::array<std::uint64_t, 2>{1, 2})), "9048472c37ed51b2a12d920dba98de05");

                BOOST_CHECK_EQUAL(xxhash_64.digest(std::uint64_t{42}), std::uint64_t{0xf92a'4389'bd62'ee57});
                BOOST_CHECK_EQUAL(xxhash_64.digest(std::uint32_t{42}), std::uint64_t{0xfe17'f842'1622'fcd2});
                BOOST_CHECK_EQUAL(xxhash_64.digest(std::string("r1/p1/...")), std::uint64_t{0xd7e9'ed42'f1d4'b1ce});
                BOOST_CHECK_EQUAL(xxhash_64.digest(std::array<std::uint64_t, 2>{1, 2}), std::uint64_t{0x3105'4751'462b'dd42});
        } else {
                BOOST_CHECK(true); // silence Boost.Test's "test case did not check any assertions"
        }
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

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
