//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/concepts/proxy_semi_reference.hpp> // proxy_semi_reference
#include <test/proxy.hpp>                              // element_reference, one_way_reference, shade, wrapper
#include <boost/test/unit_test.hpp>                    // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK
#include <bitset>                                      // bitset
#include <type_traits>                                 // is_class_v
#include <vector>                                      // vector

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(Concepts)
BOOST_AUTO_TEST_SUITE(ProxySemiReference)

// Every library's bit references stand in for bool, whatever their & does.
BOOST_AUTO_TEST_CASE(TheStandardBitReferencesStandInForBool)
{
        static_assert(xstd::proxy_semi_reference<std::vector<bool>::reference, bool>);
        static_assert(xstd::proxy_semi_reference<std::bitset<8>::reference, bool>);

        // libstdc++'s const_reference is bool itself, libc++'s a class of its own.
        static_assert(xstd::proxy_semi_reference<std::vector<bool>::const_reference, bool> == std::is_class_v<std::vector<bool>::const_reference>);
        BOOST_CHECK(true);
}

// The value is named by the caller, so a class that only converts is one, with or without a round trip.
BOOST_AUTO_TEST_CASE(AClassConvertingToTheNamedValueIsOne)
{
        static_assert(xstd::proxy_semi_reference<test::element_reference<test::shade>, test::shade>);
        static_assert(xstd::proxy_semi_reference<test::wrapper, test::shade>);
        static_assert(xstd::proxy_semi_reference<test::one_way_reference, test::shade>);
        BOOST_CHECK(true);
}

// The value itself, a scalar, and a class naming a value it does not convert to are none.
BOOST_AUTO_TEST_CASE(TheValueAScalarOrAnotherValueIsNone)
{
        static_assert(not xstd::proxy_semi_reference<test::shade, test::shade>);
        static_assert(not xstd::proxy_semi_reference<int, long>);
        static_assert(not xstd::proxy_semi_reference<test::wrapper, bool>);
        static_assert(not xstd::proxy_semi_reference<test::element_reference<test::shade>&, test::shade>);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
