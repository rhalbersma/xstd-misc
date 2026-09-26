//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/concepts/specialization_of.hpp> // specialization_of
#include <test/constexpr_check.hpp>                 // XSTD_CONSTEXPR_CHECK, XSTD_CONSTEXPR_CHECK_EQUAL
#include <boost/test/unit_test.hpp>                 // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <complex>                                  // complex
#include <concepts>                                 // integral
#include <vector>                                   // vector

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(Concepts)
BOOST_AUTO_TEST_SUITE(SpecializationOf)

// A storage whose one parameter is constrained.
template<std::integral Block>
struct blocks
{};

// The adaptors this is for: each backend named by its own template.
template<xstd::specialization_of<std::vector> Owner>
class vector_view
{
        Owner* m_owner{};
};

template<xstd::specialization_of<blocks> Bits>
class blocks_adaptor
{
        Bits* m_bits{};
};

// the partial application a type-constraint needs: the primary template alone
template<xstd::specialization_of<std::complex> T>
[[nodiscard]] constexpr auto as_complex(T z) noexcept
        -> T
{
        return z;
}

template<class T>
concept has_as_complex = requires (T t) { as_complex(t); };

BOOST_AUTO_TEST_CASE(ConstrainsToSpecializationsOfThePrimaryTemplate)
{
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of<std::complex<int>, std::complex>));
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of<std::vector<int>, std::vector>));
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of<blocks<unsigned>, blocks>));
        XSTD_CONSTEXPR_CHECK((not xstd::specialization_of<int, std::complex>));

        // used as a type-constraint, it constrains rather than hard-errors
        XSTD_CONSTEXPR_CHECK(has_as_complex<std::complex<double>>);
        XSTD_CONSTEXPR_CHECK(not has_as_complex<int>);

        // and it runs; double rather than int, the MSVC STL deprecating the others
        XSTD_CONSTEXPR_CHECK_EQUAL((as_complex(std::complex<double>{1.0, 2.0})), (std::complex<double>{1.0, 2.0}));
}

// An adaptor over a const owner names Container const, which the constraint sees through; a reference it does not.
BOOST_AUTO_TEST_CASE(TheConstraintTakesAConstOwner)
{
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of<std::vector<int> const, std::vector>));
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of<blocks<unsigned> const, blocks>));
        XSTD_CONSTEXPR_CHECK((not xstd::specialization_of<std::complex<int>&, std::complex>));
        XSTD_CONSTEXPR_CHECK((not xstd::specialization_of<std::complex<int> const&, std::complex>));
}

BOOST_AUTO_TEST_CASE(AnAdaptorTakesAConstOwnerAndAMutableOne)
{
        static_assert(sizeof(vector_view<std::vector<int>>) == sizeof(void*));
        static_assert(sizeof(vector_view<std::vector<int> const>) == sizeof(void*));
        static_assert(sizeof(blocks_adaptor<blocks<unsigned> const>) == sizeof(void*));
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
