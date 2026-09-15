//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/type_traits/is_specialization_of_T.hpp> // is_specialization_of_T
#include <test/constexpr_check.hpp>                         // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>                         // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <complex>                                          // complex
#include <tuple>                                            // tuple
#include <vector>                                           // vector

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(TypeTraits)
BOOST_AUTO_TEST_SUITE(IsSpecializationOfT)

template<class T>
using is_complex = xstd::is_specialization_of_T<T, std::complex>;

BOOST_AUTO_TEST_CASE(HoldsOnlyForSpecializationsOfTheTypeTakingTemplate)
{
        XSTD_CONSTEXPR_CHECK((is_complex<std::complex<int>>::value));
        XSTD_CONSTEXPR_CHECK((xstd::is_specialization_of_T_v<std::tuple<int, char>, std::tuple>));

        // a defaulted parameter is deduced with the rest, so one argument names both
        XSTD_CONSTEXPR_CHECK((xstd::is_specialization_of_T_v<std::vector<int>, std::vector>));

        XSTD_CONSTEXPR_CHECK((not is_complex<int>::value));
        XSTD_CONSTEXPR_CHECK((not xstd::is_specialization_of_T_v<std::vector<int>, std::tuple>));

        // the trait is exact: the const of an owner comes off in the concept, not here
        XSTD_CONSTEXPR_CHECK((not xstd::is_specialization_of_T_v<std::complex<int> const, std::complex>));
        XSTD_CONSTEXPR_CHECK((not xstd::is_specialization_of_T_v<std::complex<int>&, std::complex>));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
