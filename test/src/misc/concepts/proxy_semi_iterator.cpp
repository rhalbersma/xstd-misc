//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/concepts/proxy_semi_iterator.hpp> // proxy_semi_iterator
#include <test/proxy.hpp>                             // element_iterator, shade, value_iterator
#include <boost/test/unit_test.hpp>                   // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK
#include <ranges>                                     // iota_view, iterator_t, ref_view, transform_view, zip_view
#include <string>                                     // string
#include <type_traits>                                // is_class_v
#include <vector>                                     // vector
#include <version>                                    // IWYU pragma: keep; __cpp_lib_ranges_zip

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(Concepts)
BOOST_AUTO_TEST_SUITE(ProxySemiIterator)

using ints = std::ranges::ref_view<std::vector<int>>;

// A reference that is a class prvalue converting to the value makes a semi-iterator, whoever wrote it.
BOOST_AUTO_TEST_CASE(AClassPrvalueStandingInForTheValueIsAProxy)
{
        static_assert(xstd::proxy_semi_iterator<test::element_iterator<test::shade>>);
        static_assert(xstd::proxy_semi_iterator<test::element_iterator<bool>>);
        static_assert(xstd::proxy_semi_iterator<test::value_iterator>);
        static_assert(xstd::proxy_semi_iterator<std::vector<bool>::iterator>);
#ifdef __cpp_lib_ranges_zip
        static_assert(xstd::proxy_semi_iterator<std::ranges::iterator_t<std::ranges::zip_view<ints, ints>>>);
#endif

        // libstdc++'s const_reference is bool itself, libc++'s a class of its own.
        static_assert(xstd::proxy_semi_iterator<std::vector<bool>::const_iterator> == std::is_class_v<std::vector<bool>::const_reference>);
        BOOST_CHECK(true);
}

// A language reference, a scalar prvalue and the value itself by value are what a proxy is not.
BOOST_AUTO_TEST_CASE(AReferenceOrTheValueItselfIsNoProxy)
{
        static_assert(not xstd::proxy_semi_iterator<int*>);
        static_assert(not xstd::proxy_semi_iterator<std::vector<int>::iterator>);
        static_assert(not xstd::proxy_semi_iterator<std::ranges::iterator_t<std::ranges::iota_view<int, int>>>);
        static_assert(not xstd::proxy_semi_iterator<std::ranges::iterator_t<std::ranges::transform_view<ints, std::string (*)(int)>>>);
        static_assert(not xstd::proxy_semi_iterator<int>);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
