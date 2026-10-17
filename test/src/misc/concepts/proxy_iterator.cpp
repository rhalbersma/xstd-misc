//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/concepts/proxy_iterator.hpp> // proxy_iterator
#include <test/proxy.hpp>                        // element_iterator, element_reference, shade, value_iterator
#include <boost/test/unit_test.hpp>              // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK
#include <concepts>                              // assignable_from
#include <ranges>                                // iterator_t, ref_view, zip_view
#include <utility>                               // declval
#include <vector>                                // vector
#include <version>                               // IWYU pragma: keep; __cpp_lib_ranges_zip

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(Concepts)
BOOST_AUTO_TEST_SUITE(ProxyIterator)

using ints = std::ranges::ref_view<std::vector<int>>;

// Whether a type declares an operator& of its own, which each standard library decides for its bit references.
template<class R>
constexpr bool declares_address_of = requires (R& ref) { ref.operator&(); };

// Whether a proxy assigns through when const, which P2321 asks of vector<bool>'s reference.
template<class R>
constexpr bool writes_through_const = std::assignable_from<R const&, bool>;

// What it yields is a proxy reference, whose address is again such an iterator.
BOOST_AUTO_TEST_CASE(ItsReferenceIsAProxyReference)
{
        static_assert(xstd::proxy_iterator<test::element_iterator<test::shade>>);
        static_assert(xstd::proxy_iterator<test::element_iterator<bool>>);
        static_assert(xstd::proxy_iterator<decltype(&std::declval<test::element_reference<test::shade>&>())>);

        // vector<bool>'s iterator is where its reference has an operator& and writes through, its const_iterator never.
        static_assert(xstd::proxy_iterator<std::vector<bool>::iterator> == (declares_address_of<std::vector<bool>::reference> and writes_through_const<std::vector<bool>::reference>));
        static_assert(not xstd::proxy_iterator<std::vector<bool>::const_iterator>);
        BOOST_CHECK(true);
}

// A semi-iterator whose reference keeps the built-in address is no more than that.
BOOST_AUTO_TEST_CASE(ASemiIteratorWithoutTheRoundTripIsNone)
{
        static_assert(not xstd::proxy_iterator<test::value_iterator>);
#ifdef __cpp_lib_ranges_zip
        static_assert(not xstd::proxy_iterator<std::ranges::iterator_t<std::ranges::zip_view<ints, ints>>>);
#endif
        static_assert(not xstd::proxy_iterator<int*>);
        static_assert(not xstd::proxy_iterator<std::vector<int>::iterator>);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
