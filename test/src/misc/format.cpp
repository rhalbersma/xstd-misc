//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/format.hpp>     // formatter<boost::hash2::digest<N>>
#include <boost/hash2/digest.hpp>   // digest
#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK_EQUAL
#include <format>                   // format

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(Format)

// The front door opens on every room behind it: reached through the umbrella alone.
BOOST_AUTO_TEST_CASE(ReExportsTheWholeDirectory)
{
        BOOST_CHECK_EQUAL(std::format("{}", boost::hash2::digest<2>()), "0000");
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
