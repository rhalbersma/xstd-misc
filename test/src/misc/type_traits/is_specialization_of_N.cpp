//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/type_traits/is_specialization_of_N.hpp> // is_specialization_of_N
#include <test/constexpr_check.hpp>                         // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>                         // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <bitset>                                           // bitset
#include <ratio>                                            // ratio

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(TypeTraits)
BOOST_AUTO_TEST_SUITE(IsSpecializationOfN)

template<class T>
using is_bitset = xstd::is_specialization_of_N<T, std::bitset>;

BOOST_AUTO_TEST_CASE(HoldsOnlyForSpecializationsOfTheValueTakingTemplate)
{
        XSTD_CONSTEXPR_CHECK((is_bitset<std::bitset<8>>::value));
        XSTD_CONSTEXPR_CHECK((xstd::is_specialization_of_N_v<std::bitset<64>, std::bitset>));
        XSTD_CONSTEXPR_CHECK((xstd::is_specialization_of_N_v<std::ratio<1, 2>, std::ratio>));

        XSTD_CONSTEXPR_CHECK((not is_bitset<int>::value));
        XSTD_CONSTEXPR_CHECK((not xstd::is_specialization_of_N_v<std::ratio<1, 2>, std::bitset>));

        // the trait is exact: the const of an owner comes off in the concept, not here
        XSTD_CONSTEXPR_CHECK((not xstd::is_specialization_of_N_v<std::bitset<8> const, std::bitset>));
        XSTD_CONSTEXPR_CHECK((not xstd::is_specialization_of_N_v<std::bitset<8>&, std::bitset>));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
