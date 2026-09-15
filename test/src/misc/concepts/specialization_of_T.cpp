//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/concepts/specialization_of_T.hpp> // specialization_of_T
#include <test/constexpr_check.hpp>                   // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>                   // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <complex>                                    // complex
#include <vector>                                     // vector

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(Concepts)
BOOST_AUTO_TEST_SUITE(SpecializationOfT)

// An adaptor over a types-only storage, the owner named as the argument.
template<xstd::specialization_of_T<std::vector> Owner>
class view
{
        Owner* m_owner{};
};

BOOST_AUTO_TEST_CASE(ConstrainsToSpecializationsOfATypeTakingTemplate)
{
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of_T<std::vector<int>, std::vector>));
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of_T<std::complex<double>, std::complex>));

        XSTD_CONSTEXPR_CHECK((not xstd::specialization_of_T<int, std::vector>));
        XSTD_CONSTEXPR_CHECK((not xstd::specialization_of_T<std::vector<int>, std::complex>));

        // the const of an owner comes off, a reference does not
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of_T<std::vector<int> const, std::vector>));
        XSTD_CONSTEXPR_CHECK((not xstd::specialization_of_T<std::vector<int>&, std::vector>));
}

BOOST_AUTO_TEST_CASE(AnAdaptorTakesAConstOwnerAndAMutableOne)
{
        static_assert(sizeof(view<std::vector<int>>) == sizeof(void*));
        static_assert(sizeof(view<std::vector<int> const>) == sizeof(void*));
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
