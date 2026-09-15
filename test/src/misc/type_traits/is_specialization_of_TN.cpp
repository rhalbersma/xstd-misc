//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/type_traits/is_specialization_of_TN.hpp> // is_specialization_of_TN
#include <test/constexpr_check.hpp>                          // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>                          // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <array>                                             // array
#include <concepts>                                          // integral
#include <cstddef>                                           // size_t
#include <span>                                              // span
#include <vector>                                            // vector

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(TypeTraits)
BOOST_AUTO_TEST_SUITE(IsSpecializationOfTN)

// A storage naming its blocks and then its extent, its first parameter constrained.
template<std::integral Block, std::size_t N = 0>
struct blocks
{};

template<class T>
using is_array = xstd::is_specialization_of_TN<T, std::array>;

BOOST_AUTO_TEST_CASE(HoldsOnlyForSpecializationsOfTheTypeThenValueTemplate)
{
        XSTD_CONSTEXPR_CHECK((is_array<std::array<int, 3>>::value));
        XSTD_CONSTEXPR_CHECK((xstd::is_specialization_of_TN_v<std::span<int, 4>, std::span>));

        XSTD_CONSTEXPR_CHECK((not is_array<int>::value));
        XSTD_CONSTEXPR_CHECK((not xstd::is_specialization_of_TN_v<std::array<int, 3>, std::span>));

        // a types-only template belongs to the other shape
        XSTD_CONSTEXPR_CHECK((not xstd::is_specialization_of_TN_v<std::vector<int>, std::array>));

        // the trait is exact: the const of an owner comes off in the concept, not here
        XSTD_CONSTEXPR_CHECK((not xstd::is_specialization_of_TN_v<std::array<int, 3> const, std::array>));
}

// An unconstrained template template parameter does not consider its argument's constraints.
BOOST_AUTO_TEST_CASE(AConstrainedParameterIsNoObstacle)
{
        XSTD_CONSTEXPR_CHECK((xstd::is_specialization_of_TN_v<blocks<unsigned, 64>, blocks>));
        XSTD_CONSTEXPR_CHECK((xstd::is_specialization_of_TN_v<blocks<int>, blocks>));
        XSTD_CONSTEXPR_CHECK((not xstd::is_specialization_of_TN_v<std::array<int, 3>, blocks>));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
