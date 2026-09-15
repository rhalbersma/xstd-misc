//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/type_traits/is_nontype_specialization_of.hpp> // is_nontype_specialization_of
#include <test/constexpr_check.hpp>                               // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>                               // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <bitset>                                                 // bitset
#include <ratio>                                                  // ratio

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(TypeTraits)
BOOST_AUTO_TEST_SUITE(IsNontypeSpecializationOf)

template<class T>
using is_bitset = xstd::is_nontype_specialization_of<T, std::bitset>;

BOOST_AUTO_TEST_CASE(HoldsOnlyForSpecializationsOfTheValueTakingTemplate)
{
        XSTD_CONSTEXPR_CHECK((is_bitset<std::bitset<8>>::value));
        XSTD_CONSTEXPR_CHECK((xstd::is_nontype_specialization_of_v<std::bitset<64>, std::bitset>));
        XSTD_CONSTEXPR_CHECK((xstd::is_nontype_specialization_of_v<std::ratio<1, 2>, std::ratio>));

        XSTD_CONSTEXPR_CHECK((not is_bitset<int>::value));
        XSTD_CONSTEXPR_CHECK((not xstd::is_nontype_specialization_of_v<std::ratio<1, 2>, std::bitset>));

        // the trait is the exact question: a const owner is not one, the concept being where that comes off
        XSTD_CONSTEXPR_CHECK((not xstd::is_nontype_specialization_of_v<std::bitset<8> const, std::bitset>));
        XSTD_CONSTEXPR_CHECK((not xstd::is_nontype_specialization_of_v<std::bitset<8>&, std::bitset>));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
