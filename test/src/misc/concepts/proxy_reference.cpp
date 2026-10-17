//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/concepts/proxy_reference.hpp> // proxy_reference
#include <test/proxy.hpp>                         // element_reference, one_way_reference, shade, wrapper
#include <boost/test/unit_test.hpp>               // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK
#include <atomic>                                 // atomic
#include <bitset>                                 // bitset
#include <concepts>                               // assignable_from, common_reference_with
#include <type_traits>                            // integral_constant
#include <vector>                                 // vector

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(Concepts)
BOOST_AUTO_TEST_SUITE(ProxyReference)

// The address leads to an iterator and the iterator back to the proxy, which writes through to the value it stands for.
BOOST_AUTO_TEST_CASE(AWritableProxyClosedUnderAddressAndDereferenceIsOne)
{
        static_assert(xstd::proxy_reference<test::element_reference<test::shade>>);
        static_assert(xstd::proxy_reference<test::element_reference<bool>>);

        // The common reference std::ranges asks of a proxy follows from its address being a semi-iterator.
        static_assert(std::common_reference_with<test::element_reference<test::shade>&&, test::shade&>);
        BOOST_CHECK(true);
}

// libc++'s const_reference round-trips through its const iterator, libstdc++'s is bool, and neither writes through.
BOOST_AUTO_TEST_CASE(AReadOnlyBitReferenceIsNone)
{
        static_assert(not xstd::proxy_reference<std::vector<bool>::const_reference>);
        BOOST_CHECK(true);
}

// Whether a type declares an operator& of its own, which each standard library decides for its bit references.
template<class R>
constexpr bool declares_address_of = requires (R& ref) { ref.operator&(); };

// Whether a proxy assigns through when const, which P2321 asks of vector<bool>'s reference and not of bitset's.
template<class R>
constexpr bool writes_through_const = std::assignable_from<R const&, bool>;

// A bit reference is one where its library gives it an operator&, as libc++ 22 does, and assignment through const.
BOOST_AUTO_TEST_CASE(ALibraryBitReferenceIsOneWhereItsAddressRoundTripsAndItWritesThrough)
{
        static_assert(xstd::proxy_reference<std::vector<bool>::reference> == (declares_address_of<std::vector<bool>::reference> and writes_through_const<std::vector<bool>::reference>));
        static_assert(xstd::proxy_reference<std::bitset<8>::reference> == (declares_address_of<std::bitset<8>::reference> and writes_through_const<std::bitset<8>::reference>));
        BOOST_CHECK(true);
}

// Converting is not enough: with no way back from the address, the value type has no name to be read from.
BOOST_AUTO_TEST_CASE(AConversionWithoutTheRoundTripIsNone)
{
        static_assert(not xstd::proxy_reference<test::wrapper>);
        static_assert(not xstd::proxy_reference<test::one_way_reference>);
        static_assert(not xstd::proxy_reference<std::atomic<test::shade>>);
        static_assert(not xstd::proxy_reference<std::integral_constant<test::shade, test::shade::dark>>);
        static_assert(not xstd::proxy_reference<test::shade>);
        static_assert(not xstd::proxy_reference<test::element_reference<test::shade>&>);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
