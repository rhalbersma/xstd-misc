//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/concepts/same_template_as.hpp> // same_template_as
#include <test/constexpr_check.hpp>                // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>                // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <array>                                   // array
#include <bitset>                                  // bitset
#include <complex>                                 // complex
#include <vector>                                  // vector

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(Concepts)
BOOST_AUTO_TEST_SUITE(SameTemplateAs)

// the partial application a type-constraint needs: an example specialization, the
// primary template alone being unspellable for a template taking a value
template<xstd::same_template_as<std::array<int, 1>> T>
[[nodiscard]] constexpr auto as_array(T a) noexcept
        -> T
{
        return a;
}

template<class T>
concept has_as_array = requires (T t) { as_array(t); };

BOOST_AUTO_TEST_CASE(ConstrainsToSpecializationsOfOneTemplate)
{
        XSTD_CONSTEXPR_CHECK((xstd::same_template_as<std::array<int, 3>, std::array<char, 7>>));
        XSTD_CONSTEXPR_CHECK((xstd::same_template_as<std::bitset<8>, std::bitset<64>>));
        XSTD_CONSTEXPR_CHECK((xstd::same_template_as<std::complex<double>, std::complex<float>>));
        XSTD_CONSTEXPR_CHECK((xstd::same_template_as<std::vector<int>, std::vector<char>>));

        XSTD_CONSTEXPR_CHECK((not xstd::same_template_as<std::array<int, 3>, std::vector<int>>));
        XSTD_CONSTEXPR_CHECK((not xstd::same_template_as<int, std::complex<double>>));
        XSTD_CONSTEXPR_CHECK((not xstd::same_template_as<std::array<int, 3>&, std::array<int, 3>>));

        // the relation reads the same either way round
        XSTD_CONSTEXPR_CHECK((xstd::same_template_as<std::array<char, 7>, std::array<int, 3>>));

        // used as a type-constraint, it constrains rather than hard-errors
        XSTD_CONSTEXPR_CHECK((has_as_array<std::array<double, 2>>));
        XSTD_CONSTEXPR_CHECK((not has_as_array<int>));
        XSTD_CONSTEXPR_CHECK((not has_as_array<std::vector<int>>));

        // and it runs, over an extent the constraining example does not have
        XSTD_CONSTEXPR_CHECK((as_array(std::array<double, 2>{1.0, 2.0}) == std::array<double, 2>{1.0, 2.0}));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
