//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/type_traits/is_specialization_of.hpp> // is_specialization_of, is_specialization_of_N, is_specialization_of_T, is_specialization_of_TN
#include <test/constexpr_check.hpp>                       // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>                       // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <array>                                          // array
#include <bitset>                                         // bitset
#include <complex>                                        // complex
#include <concepts>                                       // integral
#include <cstddef>                                        // size_t
#include <ratio>                                          // ratio
#include <span>                                           // span
#include <type_traits>                                    // conditional, enable_if, integral_constant
#include <tuple>                                          // tuple, tuple_element
#include <variant>                                        // variant, variant_alternative
#include <vector>                                         // vector

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(TypeTraits)
BOOST_AUTO_TEST_SUITE(IsSpecializationOf)

// A storage naming its blocks and then its extent, its first parameter constrained.
template<std::integral Block, std::size_t N = 0>
struct blocks
{};

template<class T>
using is_complex = xstd::is_specialization_of_T<T, std::complex>;

BOOST_AUTO_TEST_CASE(TypesOnly)
{
        XSTD_CONSTEXPR_CHECK((is_complex<std::complex<int>>::value));
        XSTD_CONSTEXPR_CHECK((xstd::is_specialization_of_T_v<std::tuple<int, char>, std::tuple>));

        // a defaulted parameter is deduced with the rest, so one argument names both
        XSTD_CONSTEXPR_CHECK((xstd::is_specialization_of_T_v<std::vector<int>, std::vector>));

        XSTD_CONSTEXPR_CHECK((not is_complex<int>::value));
        XSTD_CONSTEXPR_CHECK((not xstd::is_specialization_of_T_v<std::vector<int>, std::tuple>));
}

BOOST_AUTO_TEST_CASE(ValuesOnly)
{
        XSTD_CONSTEXPR_CHECK((xstd::is_specialization_of_N<std::bitset<8>, std::bitset>::value));
        XSTD_CONSTEXPR_CHECK((xstd::is_specialization_of_N_v<std::ratio<1, 2>, std::ratio>));

        XSTD_CONSTEXPR_CHECK((not xstd::is_specialization_of_N_v<int, std::bitset>));
        XSTD_CONSTEXPR_CHECK((not xstd::is_specialization_of_N_v<std::ratio<1, 2>, std::bitset>));
}

BOOST_AUTO_TEST_CASE(ATypeThenValues)
{
        XSTD_CONSTEXPR_CHECK((xstd::is_specialization_of_TN<std::array<int, 3>, std::array>::value));
        XSTD_CONSTEXPR_CHECK((xstd::is_specialization_of_TN_v<std::span<int, 4>, std::span>));

        XSTD_CONSTEXPR_CHECK((not xstd::is_specialization_of_TN_v<int, std::array>));
        XSTD_CONSTEXPR_CHECK((not xstd::is_specialization_of_TN_v<std::array<int, 3>, std::span>));

        // a types-only template belongs to the other shape
        XSTD_CONSTEXPR_CHECK((not xstd::is_specialization_of_TN_v<std::vector<int>, std::array>));
}

BOOST_AUTO_TEST_CASE(AValueThenTypes)
{
        XSTD_CONSTEXPR_CHECK((xstd::is_specialization_of_NT<std::enable_if<true, int>, std::enable_if>::value));
        XSTD_CONSTEXPR_CHECK((xstd::is_specialization_of_NT_v<std::conditional<true, int, char>, std::conditional>));
        XSTD_CONSTEXPR_CHECK((xstd::is_specialization_of_NT_v<std::tuple_element<0, std::tuple<int>>, std::tuple_element>));
        XSTD_CONSTEXPR_CHECK((xstd::is_specialization_of_NT_v<std::variant_alternative<0, std::variant<int>>, std::variant_alternative>));

        XSTD_CONSTEXPR_CHECK((not xstd::is_specialization_of_NT_v<int, std::enable_if>));
        XSTD_CONSTEXPR_CHECK((not xstd::is_specialization_of_NT_v<std::enable_if<true, int>, std::conditional>));
}

// An unconstrained template template parameter does not consider its argument's constraints.
BOOST_AUTO_TEST_CASE(AConstrainedParameterIsNoObstacle)
{
        XSTD_CONSTEXPR_CHECK((xstd::is_specialization_of_TN_v<blocks<unsigned, 64>, blocks>));
        XSTD_CONSTEXPR_CHECK((xstd::is_specialization_of_TN_v<blocks<int>, blocks>));
        XSTD_CONSTEXPR_CHECK((not xstd::is_specialization_of_TN_v<std::array<int, 3>, blocks>));
}

// The traits are the exact question: the const of an owner comes off in the concepts, not here.
BOOST_AUTO_TEST_CASE(TheTraitsAreExact)
{
        XSTD_CONSTEXPR_CHECK((not xstd::is_specialization_of_T_v<std::complex<int> const, std::complex>));
        XSTD_CONSTEXPR_CHECK((not xstd::is_specialization_of_N_v<std::bitset<8> const, std::bitset>));
        XSTD_CONSTEXPR_CHECK((not xstd::is_specialization_of_TN_v<std::array<int, 3> const, std::array>));
        XSTD_CONSTEXPR_CHECK((not xstd::is_specialization_of_NT_v<std::enable_if<true, int> const, std::enable_if>));

        XSTD_CONSTEXPR_CHECK((not xstd::is_specialization_of_T_v<std::complex<int>&, std::complex>));
        XSTD_CONSTEXPR_CHECK((not xstd::is_specialization_of_N_v<std::bitset<8>&, std::bitset>));
        XSTD_CONSTEXPR_CHECK((not xstd::is_specialization_of_TN_v<std::array<int, 3>&, std::array>));
}

// The unsuffixed spelling is the all-types one, alias and aliased answering alike.
BOOST_AUTO_TEST_CASE(TheUnsuffixedNameIsTheSuffixedOne)
{
        XSTD_CONSTEXPR_CHECK((xstd::is_specialization_of_v<std::vector<int>, std::vector>));
        XSTD_CONSTEXPR_CHECK((xstd::is_specialization_of<std::vector<int>, std::vector>::value));
        XSTD_CONSTEXPR_CHECK((not xstd::is_specialization_of_v<int, std::vector>));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
