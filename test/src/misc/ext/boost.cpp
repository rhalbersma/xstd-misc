//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_CHECK_NE

// Reached the way a consumer reaches it: the probe here, the adapter behind the umbrella.
#if __has_include(<boost/hash2/hash_append.hpp>)
#define TEST_HAS_BOOST_HASH2
#include <xstd/misc/ext/boost.hpp> // hash_algorithm, hasher, long_hash, short_hash
#endif

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(Ext)
BOOST_AUTO_TEST_SUITE(Boost)

// One adapter behind this door, so no invariant spans two; that it arrives is what the umbrella answers for.
BOOST_AUTO_TEST_CASE(ReExportsTheWholeDirectory)
{
#ifdef TEST_HAS_BOOST_HASH2
        auto const default_hasher = xstd::hasher<>();
        auto const long_hasher    = xstd::hasher<xstd::long_hash>();
        auto const short_hasher   = xstd::hasher<xstd::short_hash>();

        BOOST_CHECK(xstd::hash_algorithm<xstd::short_hash>);
        BOOST_CHECK_EQUAL(long_hasher(42), default_hasher(42));
        BOOST_CHECK_NE(short_hasher(42), default_hasher(42));
#else
        BOOST_CHECK(true);
#endif
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
