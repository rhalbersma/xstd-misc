//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/concepts/hash_algorithm.hpp> // hash_algorithm
#include <xstd/misc/functional/digest.hpp>       // digest
#include <test/constexpr_check.hpp>              // XSTD_CONSTEXPR_CHECK
#include <test/hash2_algorithms.hpp>             // algorithms
#include <boost/hash2/digest.hpp>                // digest, to_string
#include <boost/hash2/flavor.hpp>                // big_endian_flavor, little_endian_flavor
#include <boost/hash2/fnv1a.hpp>                 // fnv1a_32
#include <boost/hash2/hash_append.hpp>           // hash_append
#include <boost/hash2/xxh3.hpp>                  // xxh3_128
#include <boost/hash2/xxhash.hpp>                // xxhash_64
#include <boost/test/unit_test.hpp>              // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK, BOOST_CHECK_EQUAL
#include <array>                                 // array
#include <concepts>                              // same_as
#include <cstddef>                               // size_t
#include <cstdint>                               // int64_t, uint32_t, uint64_t
#include <string>                                // string
#include <string_view>                           // string_view
#include <type_traits>                           // is_constructible_v, is_convertible_v
#include <utility>                               // pair

namespace {

constexpr auto seed       = std::uint64_t{0x9e37'79b9'7f4a'7c15};
constexpr auto other_seed = std::uint64_t{0xc2b2'ae3d'27d4'eb4f};

// What Hash2 makes of a key appended in little-endian bytes to the seeded algorithm, with no digest in between.
template<class H, class T>
auto hash2_digest_of(H algorithm, T const& v)
{
        boost::hash2::hash_append(algorithm, boost::hash2::little_endian_flavor(), v);
        return algorithm.result();
}

} // namespace

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(Functional)
BOOST_AUTO_TEST_SUITE(Digest)

// A digest has a result_type and both seeds, as an algorithm does, yet it takes a key rather than updates.
BOOST_AUTO_TEST_CASE(ADigestIsNotAHashAlgorithm)
{
        XSTD_CONSTEXPR_CHECK(not xstd::hash_algorithm<xstd::digest<int>>);
}

BOOST_AUTO_TEST_CASE(TheDefaultAlgorithmIsXxh3_128InLittleEndianBytes)
{
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::digest<int>, xstd::digest<int, boost::hash2::xxh3_128, boost::hash2::little_endian_flavor>>));
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::digest<int>, xstd::digest<int, boost::hash2::xxh3_128>>));
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::digest<int>::result_type, boost::hash2::digest<16>>));
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::digest<std::string>::result_type, boost::hash2::digest<16>>));
        XSTD_CONSTEXPR_CHECK((std::same_as<decltype(xstd::digest<int, boost::hash2::fnv1a_32>()(42)), std::uint32_t>));
}

// No integer converts to a seed by accident.
BOOST_AUTO_TEST_CASE(SeedsAreExplicit)
{
        XSTD_CONSTEXPR_CHECK((std::is_constructible_v<xstd::digest<int>, std::uint64_t>));
        XSTD_CONSTEXPR_CHECK((not std::is_convertible_v<std::uint64_t, xstd::digest<int>>));
        XSTD_CONSTEXPR_CHECK((std::is_constructible_v<xstd::digest<int>, unsigned char const*, std::size_t>));
}

// The digest is what Hash2 gives for the key appended to the seeded algorithm, and seeds tell digests apart.
BOOST_AUTO_TEST_CASE_TEMPLATE(IsWhatHash2Gives, H, test::algorithms)
{
        using pair_digest   = xstd::digest<std::pair<std::string, int>, H>;
        auto const pair_key = std::pair{std::string("piece"), 42};

        BOOST_CHECK(pair_digest(seed)(pair_key) == hash2_digest_of(H(seed), pair_key));
        BOOST_CHECK(pair_digest(seed)(pair_key) != pair_digest(other_seed)(pair_key));
}

// 42 converts to the key's type first, so it digests as the std::int64_t{42} that was persisted.
BOOST_AUTO_TEST_CASE(ConvertsEachArgumentToItsKeyType)
{
        auto const d = xstd::digest<std::int64_t>(seed);

        BOOST_CHECK(d(42) == hash2_digest_of(boost::hash2::xxh3_128(seed), std::int64_t{42}));
        BOOST_CHECK(d(42) != hash2_digest_of(boost::hash2::xxh3_128(seed), 42));
}

// A string, its view and a C string append the same characters, under every algorithm.
BOOST_AUTO_TEST_CASE_TEMPLATE(AStringDigestsAsItsViewAndItsCString, H, test::algorithms)
{
        auto const d      = xstd::digest<std::string, H>(seed);
        auto const string = std::string("piece");

        BOOST_CHECK(d(std::string_view(string)) == d(string));
        BOOST_CHECK(d("piece") == d(string));
        BOOST_CHECK(d(string) == hash2_digest_of(H(seed), string));
}

// The flavor fixes the bytes appended: 42 in big-endian bytes is its byte swap in little-endian ones, on every target.
BOOST_AUTO_TEST_CASE_TEMPLATE(TheFlavorFixesTheByteOrder, H, test::algorithms)
{
        using big_endian    = xstd::digest<std::uint32_t, H, boost::hash2::big_endian_flavor>;
        using little_endian = xstd::digest<std::uint32_t, H, boost::hash2::little_endian_flavor>;

        BOOST_CHECK(big_endian(seed)(0x0000'002A) == little_endian(seed)(0x2A00'0000));
        BOOST_CHECK(big_endian(seed)(0x0000'002A) != little_endian(seed)(0x0000'002A));
}

// Reference xxHash digests of the bytes appended: little-endian integers, a string's bytes then a uint32 size.
BOOST_AUTO_TEST_CASE(PersistedDigestsMatchTheReferenceImplementation)
{
        using pair_of_words = std::array<std::uint64_t, 2>;
        auto const to_hex   = [](auto const& digest) { return boost::hash2::to_string(digest); };

        BOOST_CHECK_EQUAL(to_hex(xstd::digest<std::uint64_t>(seed)(42)), "c8ad00538ea09cd0bf9c129cd1b5be7c");
        BOOST_CHECK_EQUAL(to_hex(xstd::digest<std::uint32_t>(seed)(42)), "1e4cc47a745f03cc0e89e603d693903f");
        BOOST_CHECK_EQUAL(to_hex(xstd::digest<std::string>(seed)("r1/p1/...")), "ef4ce4cfdd731616a44b2e886dd62750");
        BOOST_CHECK_EQUAL(to_hex(xstd::digest<pair_of_words>(seed)({1, 2})), "9048472c37ed51b2a12d920dba98de05");

        BOOST_CHECK_EQUAL((xstd::digest<std::uint64_t, boost::hash2::xxhash_64>(seed)(42)), std::uint64_t{0xf92a'4389'bd62'ee57});
        BOOST_CHECK_EQUAL((xstd::digest<std::uint32_t, boost::hash2::xxhash_64>(seed)(42)), std::uint64_t{0xfe17'f842'1622'fcd2});
        BOOST_CHECK_EQUAL((xstd::digest<std::string, boost::hash2::xxhash_64>(seed)("r1/p1/...")), std::uint64_t{0xd7e9'ed42'f1d4'b1ce});
        BOOST_CHECK_EQUAL((xstd::digest<pair_of_words, boost::hash2::xxhash_64>(seed)({1, 2})), std::uint64_t{0x3105'4751'462b'dd42});
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
