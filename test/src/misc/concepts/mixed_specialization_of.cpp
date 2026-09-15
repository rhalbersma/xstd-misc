//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/concepts/mixed_specialization_of.hpp> // mixed_specialization_of
#include <test/constexpr_check.hpp>                       // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>                       // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <array>                                          // array
#include <concepts>                                       // integral
#include <cstddef>                                        // size_t
#include <span>                                           // span
#include <vector>                                         // vector

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(Concepts)
BOOST_AUTO_TEST_SUITE(MixedSpecializationOf)

// A storage naming its blocks and then its extent, its first parameter constrained.
template<std::integral Block, std::size_t N = 0>
struct blocks
{};

// The adaptor this is for: the backend named as the argument, over a mutable owner or a const one.
template<xstd::mixed_specialization_of<blocks> Bits>
class sequence_adaptor
{
        Bits* m_bits{};
};

BOOST_AUTO_TEST_CASE(ConstrainsToSpecializationsOfATypeThenValueTemplate)
{
        XSTD_CONSTEXPR_CHECK((xstd::mixed_specialization_of<std::array<int, 3>, std::array>));
        XSTD_CONSTEXPR_CHECK((xstd::mixed_specialization_of<std::span<int, 4>, std::span>));
        XSTD_CONSTEXPR_CHECK((xstd::mixed_specialization_of<blocks<unsigned, 64>, blocks>));

        XSTD_CONSTEXPR_CHECK((not xstd::mixed_specialization_of<int, std::array>));
        XSTD_CONSTEXPR_CHECK((not xstd::mixed_specialization_of<std::vector<int>, std::array>));
        XSTD_CONSTEXPR_CHECK((not xstd::mixed_specialization_of<std::array<int, 3>, blocks>));

        // the const of an owner comes off, a reference does not
        XSTD_CONSTEXPR_CHECK((xstd::mixed_specialization_of<blocks<unsigned, 64> const, blocks>));
        XSTD_CONSTEXPR_CHECK((xstd::mixed_specialization_of<std::array<int, 3> const, std::array>));
        XSTD_CONSTEXPR_CHECK((not xstd::mixed_specialization_of<std::array<int, 3>&, std::array>));
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
