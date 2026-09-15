//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/concepts/specialization_of_N.hpp> // specialization_of_N
#include <test/constexpr_check.hpp>                   // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>                   // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <bitset>                                     // bitset
#include <ratio>                                      // ratio

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(Concepts)
BOOST_AUTO_TEST_SUITE(SpecializationOfN)

// An adaptor over a value-taking storage, the owner named as the argument.
template<xstd::specialization_of_N<std::bitset> Bits>
class bitset_adaptor
{
        Bits* m_bits{};
};

BOOST_AUTO_TEST_CASE(ConstrainsToSpecializationsOfAValueTakingTemplate)
{
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of_N<std::bitset<8>, std::bitset>));
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of_N<std::ratio<1, 2>, std::ratio>));

        XSTD_CONSTEXPR_CHECK((not xstd::specialization_of_N<int, std::bitset>));
        XSTD_CONSTEXPR_CHECK((not xstd::specialization_of_N<std::ratio<1, 2>, std::bitset>));

        // the const of an owner comes off, a reference does not
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of_N<std::bitset<8> const, std::bitset>));
        XSTD_CONSTEXPR_CHECK((not xstd::specialization_of_N<std::bitset<8>&, std::bitset>));
        XSTD_CONSTEXPR_CHECK((not xstd::specialization_of_N<std::bitset<8> const&, std::bitset>));
}

BOOST_AUTO_TEST_CASE(AnAdaptorTakesAConstOwnerAndAMutableOne)
{
        static_assert(sizeof(bitset_adaptor<std::bitset<8>>) == sizeof(void*));
        static_assert(sizeof(bitset_adaptor<std::bitset<8> const>) == sizeof(void*));
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
