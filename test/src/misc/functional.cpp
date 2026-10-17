//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/functional.hpp> // hash, hasher
#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_CHECK_NE
#include <cstdint>                  // int64_t

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(Functional)

// The front door opens on every room behind it: reached through the umbrella alone.
BOOST_AUTO_TEST_CASE(ReExportsTheWholeDirectory)
{
        auto const unseeded = xstd::hasher<>();
        auto const seeded   = xstd::hasher<>(1);

        BOOST_CHECK_NE(seeded(42), unseeded(42));
        BOOST_CHECK(seeded.digest(42) != unseeded.digest(42));
        BOOST_CHECK_EQUAL(xstd::hash<std::int64_t>(1)(42), seeded(std::int64_t{42}));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
