//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/type_traits/is_mixed_specialization_of.hpp> // is_mixed_specialization_of
#include <test/constexpr_check.hpp>                             // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>                             // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <array>                                                // array
#include <concepts>                                             // integral
#include <cstddef>                                              // size_t
#include <span>                                                 // span
#include <vector>                                               // vector

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(TypeTraits)
BOOST_AUTO_TEST_SUITE(IsMixedSpecializationOf)

// A storage naming its blocks and then its extent, as a bit container does, with the first
// parameter constrained: what an unconstrained template template parameter must still accept.
template<std::integral Block, std::size_t N = 0>
struct blocks
{};

template<class T>
using is_array = xstd::is_mixed_specialization_of<T, std::array>;

BOOST_AUTO_TEST_CASE(HoldsOnlyForSpecializationsOfTheTypeThenValueTemplate)
{
        XSTD_CONSTEXPR_CHECK((is_array<std::array<int, 3>>::value));
        XSTD_CONSTEXPR_CHECK((xstd::is_mixed_specialization_of_v<std::span<int, 4>, std::span>));

        XSTD_CONSTEXPR_CHECK((not is_array<int>::value));
        XSTD_CONSTEXPR_CHECK((not xstd::is_mixed_specialization_of_v<std::array<int, 3>, std::span>));

        // a types-only template is a specialization of nothing here, its own shape being the other one
        XSTD_CONSTEXPR_CHECK((not xstd::is_mixed_specialization_of_v<std::vector<int>, std::array>));

        // the trait is exact: the const of an owner comes off in the concept, not here
        XSTD_CONSTEXPR_CHECK((not xstd::is_mixed_specialization_of_v<std::array<int, 3> const, std::array>));
}

// A constrained first parameter still binds: the constraints on an argument template are not
// considered when the template template parameter is unconstrained.
BOOST_AUTO_TEST_CASE(AConstrainedParameterIsNoObstacle)
{
        XSTD_CONSTEXPR_CHECK((xstd::is_mixed_specialization_of_v<blocks<unsigned, 64>, blocks>));
        XSTD_CONSTEXPR_CHECK((xstd::is_mixed_specialization_of_v<blocks<int>, blocks>));
        XSTD_CONSTEXPR_CHECK((not xstd::is_mixed_specialization_of_v<std::array<int, 3>, blocks>));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
