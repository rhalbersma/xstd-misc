//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/type_traits/empty_base_type.hpp>   // empty_base_type
#include <xstd/misc/type_traits/empty_member_type.hpp> // empty_member_type
#include <test/constexpr_check.hpp>                    // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>                    // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <compare>                                     // strong_ordering
#include <concepts>                                    // regular, same_as
#include <type_traits>                                 // is_empty_v, is_nothrow_default_constructible_v, is_trivially_copyable_v

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(TypeTraits)
BOOST_AUTO_TEST_SUITE(EmptyBaseType)

BOOST_AUTO_TEST_CASE(IsEmpty)
{
        using empty = xstd::empty_base_type<struct tag>;

        XSTD_CONSTEXPR_CHECK((std::is_empty_v<empty>));
}

BOOST_AUTO_TEST_CASE(IsRegular)
{
        using empty = xstd::empty_base_type<struct tag>;

        XSTD_CONSTEXPR_CHECK((std::semiregular<empty>));
}

BOOST_AUTO_TEST_CASE(IsTrivial)
{
        using empty = xstd::empty_base_type<struct tag>;

        XSTD_CONSTEXPR_CHECK(std::is_trivially_destructible_v<empty>);
        XSTD_CONSTEXPR_CHECK(std::is_trivially_default_constructible_v<empty>);
        XSTD_CONSTEXPR_CHECK(std::is_trivially_copy_constructible_v<empty>);
        XSTD_CONSTEXPR_CHECK(std::is_trivially_copy_assignable_v<empty>);
        XSTD_CONSTEXPR_CHECK(std::is_trivially_move_constructible_v<empty>);
        XSTD_CONSTEXPR_CHECK(std::is_trivially_move_assignable_v<empty>);
}

BOOST_AUTO_TEST_CASE(IsNoThrow)
{
        using empty = xstd::empty_base_type<struct tag>;

        XSTD_CONSTEXPR_CHECK(std::is_nothrow_destructible_v<empty>);
        XSTD_CONSTEXPR_CHECK(std::is_nothrow_default_constructible_v<empty>);
        XSTD_CONSTEXPR_CHECK(std::is_nothrow_copy_constructible_v<empty>);
        XSTD_CONSTEXPR_CHECK(std::is_nothrow_copy_assignable_v<empty>);
        XSTD_CONSTEXPR_CHECK(std::is_nothrow_move_constructible_v<empty>);
        XSTD_CONSTEXPR_CHECK(std::is_nothrow_move_assignable_v<empty>);
}

// The default serves the uses with nothing to keep distinct, which for a base is most of them.
BOOST_AUTO_TEST_CASE(TagDefaultsToVoid)
{
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::empty_base_type<>, xstd::empty_base_type<void>>));
}

// A class cannot derive from the same base twice, which is what the tag is for here.
BOOST_AUTO_TEST_CASE(DifferentTagsGiveDistinctTypes)
{
        using empty1 = xstd::empty_base_type<struct tag1>;
        using empty2 = xstd::empty_base_type<struct tag2>;

        XSTD_CONSTEXPR_CHECK((not std::same_as<empty1, empty2>));
}

// The whole of what this type is: nothing to compare, and so nothing reaching a derived class by ADL. Named on
// a template parameter, an absent operator in a requires-expression over a concrete type being a hard error.
namespace {

template<class T>
concept eq = requires (T a, T b) { a == b; };
template<class T>
concept ne = requires (T a, T b) { a != b; };
template<class T>
concept three_way = requires (T a, T b) { a <=> b; };
template<class T>
concept lt = requires (T a, T b) { a < b; };

struct over_base : xstd::empty_base_type<struct b>
{
        int* p;
};
struct over_member : xstd::empty_member_type<struct m>
{
        int* p;
};

} // namespace

BOOST_AUTO_TEST_CASE(ItHasNoComparisonAndHandsNoneToADerivedClass)
{
        using empty = xstd::empty_base_type<struct tag>;

        XSTD_CONSTEXPR_CHECK(not eq<empty>);
        XSTD_CONSTEXPR_CHECK(not ne<empty>);
        XSTD_CONSTEXPR_CHECK(not three_way<empty>);
        XSTD_CONSTEXPR_CHECK(not lt<empty>);

        // The contrast that makes the two halves two types: empty_member_type hands its comparison down, and
        // the derived class then answers equal for every pair of objects, having only the empty base to compare.
        XSTD_CONSTEXPR_CHECK(not three_way<over_base>);
        XSTD_CONSTEXPR_CHECK(three_way<over_member>);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
