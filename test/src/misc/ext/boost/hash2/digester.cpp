//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <test/constexpr_check.hpp> // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK
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
#include <boost/hash2/digest.hpp>                 // digest
#include <boost/hash2/fnv1a.hpp>                  // fnv1a_64
#include <boost/hash2/hash_append.hpp>            // hash_append
#include <boost/hash2/xxh3.hpp>                   // xxh3_128
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
