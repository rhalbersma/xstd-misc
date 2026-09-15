//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/type_traits/is_same_template_as.hpp> // is_same_template_as, is_same_template_as_v
#include <test/constexpr_check.hpp>                      // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>                      // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <array>                                         // array
#include <bitset>                                        // bitset
#include <complex>                                       // complex
#include <cstddef>                                       // size_t
#include <ratio>                                         // ratio
#include <span>                                          // span
#include <tuple>                                         // tuple
#include <vector>                                        // vector

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(TypeTraits)
BOOST_AUTO_TEST_SUITE(IsSameTemplateAs)

// The shape the standard library does not have, and the one the trait does not spell.
template<std::size_t N, class T, class... Ts>
struct value_first
{};

template<class T, std::size_t N, class U>
struct unlisted
{};

// One name over all four parameter-kind shapes, where a template template parameter
// reaches only one: <class...> misses std::array, <auto...> misses std::complex.
BOOST_AUTO_TEST_CASE(HoldsOverEveryParameterKindOfOneTemplate)
{
        // types only
        XSTD_CONSTEXPR_CHECK((xstd::is_same_template_as_v<std::complex<double>, std::complex<float>>));
        XSTD_CONSTEXPR_CHECK((xstd::is_same_template_as_v<std::vector<int>, std::vector<char>>));
        XSTD_CONSTEXPR_CHECK((xstd::is_same_template_as_v<std::tuple<int, char>, std::tuple<>>));
        XSTD_CONSTEXPR_CHECK((xstd::is_same_template_as_v<std::tuple<>, std::tuple<>>));

        // values only
        XSTD_CONSTEXPR_CHECK((xstd::is_same_template_as_v<std::bitset<8>, std::bitset<64>>));
        XSTD_CONSTEXPR_CHECK((xstd::is_same_template_as_v<std::ratio<1, 2>, std::ratio<3, 4>>));

        // a type, then values
        XSTD_CONSTEXPR_CHECK((xstd::is_same_template_as_v<std::array<int, 3>, std::array<char, 7>>));
        XSTD_CONSTEXPR_CHECK((xstd::is_same_template_as_v<std::span<int, 4>, std::span<char, 9>>));

        // a value, then types
        XSTD_CONSTEXPR_CHECK((xstd::is_same_template_as_v<value_first<1, int>, value_first<2, char, bool>>));
}

BOOST_AUTO_TEST_CASE(FailsForTwoTemplatesAndForWhatIsNoSpecialization)
{
        // one shape does not make one template
        XSTD_CONSTEXPR_CHECK((not xstd::is_same_template_as_v<std::array<int, 3>, std::span<int, 3>>));
        XSTD_CONSTEXPR_CHECK((not xstd::is_same_template_as_v<std::bitset<2>, std::ratio<2, 1>>));
        XSTD_CONSTEXPR_CHECK((not xstd::is_same_template_as_v<std::vector<int>, std::tuple<int>>));

        // nor does one template make one shape
        XSTD_CONSTEXPR_CHECK((not xstd::is_same_template_as_v<std::array<int, 3>, std::vector<int>>));
        XSTD_CONSTEXPR_CHECK((not xstd::is_same_template_as_v<std::bitset<8>, std::array<int, 8>>));

        XSTD_CONSTEXPR_CHECK((not xstd::is_same_template_as_v<int, std::complex<double>>));
        XSTD_CONSTEXPR_CHECK((not xstd::is_same_template_as_v<int, int>));

        // the match is on the type itself, as is_specialization_of's is
        XSTD_CONSTEXPR_CHECK((not xstd::is_same_template_as_v<std::complex<double>&, std::complex<double>>));
        XSTD_CONSTEXPR_CHECK((not xstd::is_same_template_as_v<std::vector<int> const, std::vector<int>>));
}

// A trailing pack deduces to empty, so a shape that did not demand what tells it from the
// others would match where another already does, and the two would be ambiguous. Every
// shape demanding its own parameter is what keeps these answers single.
BOOST_AUTO_TEST_CASE(TheShapesDoNotOverlap)
{
        XSTD_CONSTEXPR_CHECK((xstd::is_same_template_as<std::complex<double>, std::complex<float>>::value));
        XSTD_CONSTEXPR_CHECK((xstd::is_same_template_as<std::bitset<8>, std::bitset<8>>::value));

        // A shape none of them spells is false against even itself, rather than ill-formed.
        XSTD_CONSTEXPR_CHECK((not xstd::is_same_template_as_v<unlisted<int, 1, char>, unlisted<int, 1, char>>));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
