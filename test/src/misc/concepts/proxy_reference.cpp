//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/concepts/proxy_iterator.hpp>       // proxy_iterator
#include <xstd/misc/concepts/proxy_reference.hpp>      // proxy_reference
#include <xstd/misc/concepts/proxy_semi_reference.hpp> // proxy_semi_reference
#include <test/proxy.hpp>                              // element_reference, shade
#include <boost/test/unit_test.hpp>                    // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK
#include <atomic>                                      // atomic
#include <bitset>                                      // bitset
#include <concepts>                                    // assignable_from, common_reference_with
#include <functional>                                  // reference_wrapper
#include <iterator>                                    // iter_value_t
#include <string_view>                                 // string_view
#include <type_traits>                                 // integral_constant
#include <utility>                                     // declval
#include <vector>                                      // vector

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
        static_assert(not xstd::proxy_reference<std::reference_wrapper<test::shade>>);
        static_assert(not xstd::proxy_reference<std::atomic<test::shade>>);
        static_assert(not xstd::proxy_reference<std::integral_constant<test::shade, test::shade::dark>>);
        static_assert(not xstd::proxy_reference<test::shade>);
        static_assert(not xstd::proxy_reference<test::element_reference<test::shade>&>);
        BOOST_CHECK(true);
}

// What a proxy reference is unasked: a semi-reference for the value its address names, which is a proxy iterator.
template<class R, class I = decltype(&std::declval<R&>())>
constexpr bool layers_over_its_address = xstd::proxy_semi_reference<R, std::iter_value_t<I>> and xstd::proxy_iterator<I>;

// Each follows from the round trip, so every proxy reference has both, on whichever library makes one.
BOOST_AUTO_TEST_CASE(IsASemiReferenceWhoseAddressIsAProxyIterator)
{
        static_assert(layers_over_its_address<test::element_reference<test::shade>>);
        static_assert(layers_over_its_address<test::element_reference<bool>>);
        static_assert(not xstd::proxy_reference<std::vector<bool>::reference> or layers_over_its_address<std::vector<bool>::reference>);
        static_assert(not xstd::proxy_reference<std::bitset<8>::reference> or layers_over_its_address<std::bitset<8>::reference>);
        BOOST_CHECK(true);
}

// One overload per strength: a semi-reference names its value, as a caller converting to bool would write it.
template<class R>
        requires xstd::proxy_semi_reference<R, bool>
[[nodiscard]] constexpr auto overload_taken() noexcept
        -> std::string_view
{
        return "proxy_semi_reference";
}

template<xstd::proxy_reference R>
[[nodiscard]] constexpr auto overload_taken() noexcept
        -> std::string_view
{
        return "proxy_reference";
}

// The weaker overload set aside for a proxy reference, which no subsumption between the two does by itself.
template<class R>
        requires xstd::proxy_semi_reference<R, bool> and (not xstd::proxy_reference<R>)
[[nodiscard]] constexpr auto overload_set_aside() noexcept
        -> std::string_view
{
        return "proxy_semi_reference";
}

template<xstd::proxy_reference R>
[[nodiscard]] constexpr auto overload_set_aside() noexcept
        -> std::string_view
{
        return "proxy_reference";
}

template<class R>
concept takes_an_overload = requires { overload_taken<R>(); };

// The value a semi-reference is for is the caller's, not read from an address, so the two do not order by subsumption.
BOOST_AUTO_TEST_CASE(DoesNotSubsumeTheSemiReference)
{
        static_assert(not takes_an_overload<test::element_reference<bool>>);
        static_assert(takes_an_overload<std::vector<bool>::reference> == not xstd::proxy_reference<std::vector<bool>::reference>);
        static_assert(takes_an_overload<std::bitset<8>::reference> == not xstd::proxy_reference<std::bitset<8>::reference>);

        static_assert(overload_set_aside<test::element_reference<bool>>() == "proxy_reference");
        static_assert(overload_set_aside<std::vector<bool>::reference>() == (xstd::proxy_reference<std::vector<bool>::reference> ? "proxy_reference" : "proxy_semi_reference"));
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
