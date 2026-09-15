//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/concepts/specialization_of.hpp> // specialization_of, specialization_of_N, specialization_of_T, specialization_of_TN
#include <test/constexpr_check.hpp>                 // XSTD_CONSTEXPR_CHECK, XSTD_CONSTEXPR_CHECK_EQUAL
#include <boost/test/unit_test.hpp>                 // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <array>                                    // array
#include <bitset>                                   // bitset
#include <complex>                                  // complex
#include <concepts>                                 // integral
#include <cstddef>                                  // size_t
#include <ratio>                                    // ratio
#include <tuple>                                    // tuple, tuple_element
#include <utility>                                  // integer_sequence
#include <type_traits>                              // is_trivially_copyable_v
#include <vector>                                   // vector

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(Concepts)
BOOST_AUTO_TEST_SUITE(SpecializationOf)

// A storage naming its blocks and then its extent, its first parameter constrained.
template<std::integral Block, std::size_t N = 0>
struct blocks
{};

// The adaptors this is for: each backend named by its own template, whatever shape it has.
template<xstd::specialization_of_T<std::vector> Owner>
class vector_view
{
        Owner* m_owner{};
};

template<xstd::specialization_of_N<std::bitset> Bits>
class bitset_adaptor
{
        Bits* m_bits{};
};

template<xstd::specialization_of_TN<blocks> Bits>
class sequence_adaptor
{
        Bits* m_bits{};
};

// the partial application a type-constraint needs: the primary template alone
template<xstd::specialization_of_T<std::complex> T>
[[nodiscard]] constexpr auto as_complex(T z) noexcept
        -> T
{
        return z;
}

template<class T>
concept has_as_complex = requires (T t) { as_complex(t); };

BOOST_AUTO_TEST_CASE(ConstrainsToSpecializationsOfThePrimaryTemplate)
{
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of_T<std::complex<int>, std::complex>));
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of_T<std::tuple<int, char>, std::tuple>));
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of_N<std::bitset<8>, std::bitset>));
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of_N<std::ratio<1, 2>, std::ratio>));
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of_TN<std::array<int, 3>, std::array>));
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of_TN<blocks<unsigned, 64>, blocks>));

        XSTD_CONSTEXPR_CHECK((not xstd::specialization_of_T<int, std::complex>));
        XSTD_CONSTEXPR_CHECK((not xstd::specialization_of_N<std::ratio<1, 2>, std::bitset>));
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of_TN<std::integer_sequence<int, 1, 2>, std::integer_sequence>));
        XSTD_CONSTEXPR_CHECK((not xstd::specialization_of_TN<std::vector<int>, std::array>));
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of_NT<std::enable_if<true, int>, std::enable_if>));
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of_NT<std::tuple_element<0, std::tuple<int>>, std::tuple_element>));
        XSTD_CONSTEXPR_CHECK((not xstd::specialization_of_NT<int, std::enable_if>));

        // used as a type-constraint, it constrains rather than hard-errors
        XSTD_CONSTEXPR_CHECK(has_as_complex<std::complex<double>>);
        XSTD_CONSTEXPR_CHECK(not has_as_complex<int>);

        // and it runs; double rather than int, the MSVC STL deprecating the others
        XSTD_CONSTEXPR_CHECK_EQUAL((as_complex(std::complex<double>{1.0, 2.0})), (std::complex<double>{1.0, 2.0}));
}

// An adaptor over a const owner names Container const, which the constraints see through.
BOOST_AUTO_TEST_CASE(TheConstraintsTakeAConstOwner)
{
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of_T<std::vector<int> const, std::vector>));
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of_N<std::bitset<8> const, std::bitset>));
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of_TN<blocks<unsigned, 64> const, blocks>));
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of_NT<std::enable_if<true, int> const, std::enable_if>));

        // only the const: a reference is a specialization of nothing
        XSTD_CONSTEXPR_CHECK((not xstd::specialization_of_T<std::complex<int>&, std::complex>));
        XSTD_CONSTEXPR_CHECK((not xstd::specialization_of_T<std::complex<int> const&, std::complex>));
        XSTD_CONSTEXPR_CHECK((not xstd::specialization_of_TN<std::array<int, 3>&, std::array>));
}

BOOST_AUTO_TEST_CASE(AnAdaptorTakesAConstOwnerAndAMutableOne)
{
        static_assert(sizeof(vector_view<std::vector<int>>) == sizeof(void*));
        static_assert(sizeof(vector_view<std::vector<int> const>) == sizeof(void*));
        static_assert(sizeof(bitset_adaptor<std::bitset<8> const>) == sizeof(void*));
        static_assert(sizeof(sequence_adaptor<blocks<unsigned, 64> const>) == sizeof(void*));
        BOOST_CHECK(true);
}

// The suffixed spelling under a constraint strictly stronger than the unsuffixed one.
template<xstd::specialization_of<std::complex> T>
[[nodiscard]] constexpr auto which(T) noexcept
        -> int
{
        return 1;
}

template<xstd::specialization_of_T<std::complex> T>
        requires std::is_trivially_copyable_v<T>
[[nodiscard]] constexpr auto which(T) noexcept
        -> int
{
        return 2;
}

// An alias is only an alias if it normalizes to what it names: the two order rather than clash.
BOOST_AUTO_TEST_CASE(TheUnsuffixedNameIsTheSuffixedOne)
{
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of<std::complex<double>, std::complex>));
        XSTD_CONSTEXPR_CHECK((not xstd::specialization_of<int, std::complex>));
        XSTD_CONSTEXPR_CHECK_EQUAL(which(std::complex<double>{}), 2);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
