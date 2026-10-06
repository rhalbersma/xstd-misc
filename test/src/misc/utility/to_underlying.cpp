//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/utility/to_underlying.hpp> // to_underlying
#include <test/constexpr_check.hpp>            // XSTD_CONSTEXPR_CHECK, XSTD_CONSTEXPR_CHECK_EQUAL
#include <test/proxy.hpp>                      // element_iterator, element_reference, one_way_reference, shade, wrapper
#include <boost/test/unit_test.hpp>            // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <array>                               // array
#include <concepts>                            // same_as
#include <type_traits>                         // integral_constant, underlying_type_t

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(Utility)
BOOST_AUTO_TEST_SUITE(ToUnderlying)

enum class e1 {};
enum class e2 : bool {};
enum class e3 : char {};
enum class e4 : unsigned char {};
enum class e5 : unsigned {};

template<e1 N>
using e1_ = std::integral_constant<e1, N>;
template<e2 N>
using e2_ = std::integral_constant<e2, N>;
template<e3 N>
using e3_ = std::integral_constant<e3, N>;
template<e4 N>
using e4_ = std::integral_constant<e4, N>;
template<e5 N>
using e5_ = std::integral_constant<e5, N>;

BOOST_AUTO_TEST_CASE(YieldsTheUnderlyingValue)
{
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::to_underlying(e1()), 0);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::to_underlying(e2()), false);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::to_underlying(e3()), static_cast<char>(0));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::to_underlying(e4()), static_cast<unsigned char>(0));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::to_underlying(e5()), static_cast<unsigned>(0));

        // use {} instead of () inside <> to avoid vexing parse
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::to_underlying(e1_<e1{}>()), 0);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::to_underlying(e2_<e2{}>()), false);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::to_underlying(e3_<e3{}>()), static_cast<char>(0));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::to_underlying(e4_<e4{}>()), static_cast<unsigned char>(0));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::to_underlying(e5_<e5{}>()), static_cast<unsigned>(0));
}

template<class T>
concept has_to_underlying = requires (T t) { xstd::to_underlying(t); };

BOOST_AUTO_TEST_CASE(IsConstrainedToEnumerations)
{
        XSTD_CONSTEXPR_CHECK(has_to_underlying<e1_<e1{}>>);

        // the constraint precedes the return type, so a non-enum is a substitution failure
        XSTD_CONSTEXPR_CHECK((not has_to_underlying<std::integral_constant<int, 0>>));
        XSTD_CONSTEXPR_CHECK(not has_to_underlying<double>);
}

constexpr auto shades = std::array{test::shade::light, test::shade::dark};

// Through a proxy closed under & and *, the value is the enumerator the proxy converts to.
BOOST_AUTO_TEST_CASE(ReadsThroughAProxyForAnEnumeration)
{
        static_assert(std::same_as<decltype(xstd::to_underlying(*test::element_iterator<test::shade>(shades, 0))), std::underlying_type_t<test::shade>>);
        static_assert(noexcept(xstd::to_underlying(*test::element_iterator<test::shade>(shades, 0))));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::to_underlying(*test::element_iterator<test::shade>(shades, 0)), static_cast<unsigned char>(1));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::to_underlying(*++test::element_iterator<test::shade>(shades, 0)), static_cast<unsigned char>(2));
}

// A proxy for a non-enumeration, and a type that converts without the round trip, have no underlying value to give.
BOOST_AUTO_TEST_CASE(IsConstrainedToProxiesForEnumerations)
{
        XSTD_CONSTEXPR_CHECK(has_to_underlying<test::element_reference<test::shade>>);
        XSTD_CONSTEXPR_CHECK(not has_to_underlying<test::element_reference<bool>>);
        XSTD_CONSTEXPR_CHECK(not has_to_underlying<test::wrapper>);
        XSTD_CONSTEXPR_CHECK(not has_to_underlying<test::one_way_reference>);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
