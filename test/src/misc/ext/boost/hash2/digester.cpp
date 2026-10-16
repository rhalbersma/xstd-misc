//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <test/constexpr_check.hpp> // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK, BOOST_CHECK_EQUAL
#include <array>                    // array
#include <bit>                      // endian
#include <concepts>                 // same_as
#include <cstddef>                  // size_t
#include <cstdint>                  // uint64_t
#include <string>                   // string
#include <type_traits>              // is_constructible_v, is_convertible_v
#include <utility>                  // pair

// Reached the way a consumer reaches it: the probe here, the adapter behind it.
#if __has_include(<boost/hash2/hash_append.hpp>)
#define TEST_HAS_BOOST_HASH2
#include <xstd/misc/ext/boost/hash2/digester.hpp> // digester
#include <boost/hash2/digest.hpp>                 // digest, to_string
#include <boost/hash2/fnv1a.hpp>                  // fnv1a_64
#include <boost/hash2/hash_append.hpp>            // hash_append
#include <boost/hash2/xxh3.hpp>                   // xxh3_128
#include <boost/hash2/xxhash.hpp>                 // xxhash_64
#endif

#ifdef TEST_HAS_BOOST_HASH2

namespace {

// Never read: a namespace-scope seed as a consumer may declare one, which the seeded constructor must compile beside.
[[maybe_unused]] constexpr auto seed = std::uint64_t{7};

constexpr auto persisted_seed = std::uint64_t{0x9e37'79b9'7f4a'7c15};

} // namespace

#endif

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(Ext)
BOOST_AUTO_TEST_SUITE(Boost)
BOOST_AUTO_TEST_SUITE(Hash2)
BOOST_AUTO_TEST_SUITE(Digester)

#ifdef TEST_HAS_BOOST_HASH2

// No integer converts to a seed by accident.
BOOST_AUTO_TEST_CASE(SeedsAreExplicit)
{
        XSTD_CONSTEXPR_CHECK((std::is_constructible_v<xstd::digester<>, std::uint64_t>));
        XSTD_CONSTEXPR_CHECK((not std::is_convertible_v<std::uint64_t, xstd::digester<>>));
        XSTD_CONSTEXPR_CHECK((std::is_constructible_v<xstd::digester<>, unsigned char const*, std::size_t>));
}

// A fingerprint is as wide as its algorithm makes it, the same on every target, where a hasher's is std::size_t.
BOOST_AUTO_TEST_CASE(ADigestIsTheAlgorithmsWholeResult)
{
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::digester<>, xstd::digester<boost::hash2::xxh3_128>>));
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::digester<>::result_type, boost::hash2::digest<16>>));
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::digester<boost::hash2::fnv1a_64>::result_type, std::uint64_t>));
        XSTD_CONSTEXPR_CHECK((std::same_as<decltype(xstd::digester<boost::hash2::fnv1a_64>()(42)), std::uint64_t>));
}

// The digest is what Hash2 gives for the key appended to the seeded algorithm, and seeds tell digests apart.
BOOST_AUTO_TEST_CASE(ADigestIsWhatHash2Gives)
{
        auto const by_hand = [](std::uint64_t s, auto const& v) -> boost::hash2::digest<16> {
                auto h = boost::hash2::xxh3_128(s);
                boost::hash2::hash_append(h, {}, v);
                return h.result();
        };
        auto const pair_key = std::pair{std::string("piece"), 42};
        BOOST_CHECK(xstd::digester<>(7)(pair_key) == by_hand(7, pair_key));
        BOOST_CHECK(xstd::digester<>(7)(std::string("piece")) == by_hand(7, std::string("piece")));
        BOOST_CHECK(xstd::digester<>(7)(pair_key) != xstd::digester<>(8)(pair_key));
}

// Reference xxHash digests of the bytes Hash2 appends: little-endian integers, a string's bytes then a uint32 size.
BOOST_AUTO_TEST_CASE(PersistedFingerprintsMatchTheReferenceImplementation)
{
        if constexpr (std::endian::native == std::endian::little) {
                auto const fingerprint = xstd::digester<>(persisted_seed);
                auto const key64       = xstd::digester<boost::hash2::xxhash_64>(persisted_seed);

                BOOST_CHECK_EQUAL(boost::hash2::to_string(fingerprint(std::uint64_t{42})), "c8ad00538ea09cd0bf9c129cd1b5be7c");
                BOOST_CHECK_EQUAL(boost::hash2::to_string(fingerprint(std::uint32_t{42})), "1e4cc47a745f03cc0e89e603d693903f");
                BOOST_CHECK_EQUAL(boost::hash2::to_string(fingerprint(std::string("r1/p1/..."))), "ef4ce4cfdd731616a44b2e886dd62750");
                BOOST_CHECK_EQUAL(boost::hash2::to_string(fingerprint(std::array<std::uint64_t, 2>{1, 2})), "9048472c37ed51b2a12d920dba98de05");

                BOOST_CHECK_EQUAL(key64(std::uint64_t{42}), std::uint64_t{0xf92a'4389'bd62'ee57});
                BOOST_CHECK_EQUAL(key64(std::uint32_t{42}), std::uint64_t{0xfe17'f842'1622'fcd2});
                BOOST_CHECK_EQUAL(key64(std::string("r1/p1/...")), std::uint64_t{0xd7e9'ed42'f1d4'b1ce});
                BOOST_CHECK_EQUAL(key64(std::array<std::uint64_t, 2>{1, 2}), std::uint64_t{0x3105'4751'462b'dd42});
        } else {
                BOOST_CHECK(true); // silence Boost.Test's "test case did not check any assertions"
        }
}

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
BOOST_AUTO_TEST_SUITE_END()
