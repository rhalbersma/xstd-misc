//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <test/constexpr_check.hpp> // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_CHECK_NE
#include <cstddef>                  // size_t
#include <cstdint>                  // uint64_t
#include <string>                   // string
#include <type_traits>              // conditional_t

// Reached the way a consumer reaches it: the probe here, the adapters behind the umbrella.
#if __has_include(<boost/hash2/hash_append.hpp>)
#define TEST_HAS_BOOST_HASH2
#include <xstd/misc/ext/boost/hash2.hpp> // digester, hash_algorithm, hasher
#include <boost/hash2/fnv1a.hpp>         // fnv1a_32, fnv1a_64
#include <boost/hash2/xxhash.hpp>        // xxhash_64
#endif

#ifdef TEST_HAS_BOOST_HASH2

namespace {

// Whether a hasher over H can be named at all, rather than failing to compile.
template<class H>
concept hasher_argument = requires { typename xstd::hasher<H>; };

} // namespace

#endif

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(Ext)
BOOST_AUTO_TEST_SUITE(Boost)
BOOST_AUTO_TEST_SUITE(Hash2)

#ifdef TEST_HAS_BOOST_HASH2

// The front door opens on every room behind it: reached through the umbrella alone.
BOOST_AUTO_TEST_CASE(ReExportsTheWholeDirectory)
{
        auto const default_hasher   = xstd::hasher<>();
        auto const seeded_hasher    = xstd::hasher<>(1);
        auto const default_digester = xstd::digester<>();
        auto const seeded_digester  = xstd::digester<>(1);

        BOOST_CHECK(xstd::hash_algorithm<boost::hash2::xxhash_64>);
        BOOST_CHECK_NE(seeded_hasher(42), default_hasher(42));
        BOOST_CHECK(seeded_digester(42) != default_digester(42));
}

// Where the digest is as wide as std::size_t, the hasher returns it unchanged; elsewhere Hash2 narrows or widens it.
BOOST_AUTO_TEST_CASE(AHasherIsTheDigestAtTheWidthOfSizeT)
{
        using fnv1a_word      = std::conditional_t<sizeof(std::size_t) == sizeof(std::uint64_t), boost::hash2::fnv1a_64, boost::hash2::fnv1a_32>;
        auto const string_key = std::string("piece");
        BOOST_CHECK_EQUAL(xstd::hasher<fnv1a_word>(7)(string_key), static_cast<std::size_t>(xstd::digester<fnv1a_word>(7)(string_key)));
}

// A digester, like a hasher, digests keys and is no algorithm a hasher could run.
BOOST_AUTO_TEST_CASE(ADigesterIsNotAHashAlgorithm)
{
        XSTD_CONSTEXPR_CHECK(not xstd::hash_algorithm<xstd::digester<>>);
        XSTD_CONSTEXPR_CHECK(not hasher_argument<xstd::digester<>>);
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
