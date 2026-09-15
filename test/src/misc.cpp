//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc.hpp>            // the whole misc surface
#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <concepts>                 // same_as
#include <type_traits>              // integral_constant, is_empty_v, remove_const_t

BOOST_AUTO_TEST_SUITE(Misc)

template<class>
struct box
{};

// concepts and type_traits answer alike, up to the const an adaptor over a const owner names.
template<class T>
concept agrees = xstd::specialization_of<T, box> == xstd::is_specialization_of<std::remove_const_t<T>, box>::value;

BOOST_AUTO_TEST_CASE(TheConceptAndTheTraitAreOnePredicate)
{
        static_assert(agrees<box<int>>);
        static_assert(agrees<box<box<int>>>);
        static_assert(agrees<int>);
        static_assert(agrees<box<int>&>);
        static_assert(agrees<box<int> const>);
        static_assert(agrees<void>);

        // And they are not both vacuously false: the yes and the no are each reached.
        static_assert(xstd::specialization_of<box<int>, box>);
        static_assert(not xstd::specialization_of<int, box>);

        // where they part: the constraint takes the const owner, the trait does not
        static_assert(xstd::specialization_of<box<int> const, box>);
        static_assert(not xstd::is_specialization_of<box<int> const, box>::value);
        BOOST_CHECK(true);
}

// What the const is for: a view over a const owner names it const.
template<xstd::specialization_of<box> Owner>
class view
{
        Owner* m_owner;
};

BOOST_AUTO_TEST_CASE(AnAdaptorTakesAConstOwnerAndAMutableOne)
{
        static_assert(std::same_as<view<box<int>>, view<box<int>>>);
        static_assert(std::same_as<view<box<int> const>, view<box<int> const>>);
        static_assert(not std::same_as<view<box<int>>, view<box<int> const>>);
        BOOST_CHECK(true);
}

// The tag empty_member_type carries is a type like any other, so the trait sees through the alias to it.
enum class shade : int { one = 1 };

BOOST_AUTO_TEST_CASE(TheUtilitiesMeetOverTheLibrarysOwnTemplates)
{
        static_assert(std::is_empty_v<xstd::empty_member_type<shade>>);
        static_assert(xstd::is_specialization_of<xstd::empty_member_type<shade>, xstd::empty_member_type>::value);
        static_assert(xstd::specialization_of<xstd::empty_member_type<shade>, xstd::empty_member_type>);
        static_assert(xstd::to_underlying(std::integral_constant<shade, shade::one>{}) == 1);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
