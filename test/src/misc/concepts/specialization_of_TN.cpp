//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/concepts/specialization_of_TN.hpp> // specialization_of_TN
#include <test/constexpr_check.hpp>                    // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>                    // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <array>                                       // array
#include <concepts>                                    // integral
#include <cstddef>                                     // size_t
#include <span>                                        // span
#include <vector>                                      // vector

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(Concepts)
BOOST_AUTO_TEST_SUITE(SpecializationOfTN)

// A storage naming its blocks and then its extent, its first parameter constrained.
template<std::integral Block, std::size_t N = 0>
struct blocks
{};

// The adaptor this is for: the backend named as the argument.
template<xstd::specialization_of_TN<blocks> Bits>
class sequence_adaptor
{
        Bits* m_bits{};
};

BOOST_AUTO_TEST_CASE(ConstrainsToSpecializationsOfATypeThenValueTemplate)
{
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of_TN<std::array<int, 3>, std::array>));
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of_TN<std::span<int, 4>, std::span>));
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of_TN<blocks<unsigned, 64>, blocks>));

        XSTD_CONSTEXPR_CHECK((not xstd::specialization_of_TN<int, std::array>));
        XSTD_CONSTEXPR_CHECK((not xstd::specialization_of_TN<std::vector<int>, std::array>));
        XSTD_CONSTEXPR_CHECK((not xstd::specialization_of_TN<std::array<int, 3>, blocks>));

        // the const of an owner comes off, a reference does not
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of_TN<blocks<unsigned, 64> const, blocks>));
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of_TN<std::array<int, 3> const, std::array>));
        XSTD_CONSTEXPR_CHECK((not xstd::specialization_of_TN<std::array<int, 3>&, std::array>));
}

BOOST_AUTO_TEST_CASE(AnAdaptorTakesAConstOwnerAndAMutableOne)
{
        static_assert(sizeof(sequence_adaptor<blocks<unsigned, 64>>) == sizeof(void*));
        static_assert(sizeof(sequence_adaptor<blocks<unsigned, 64> const>) == sizeof(void*));
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
