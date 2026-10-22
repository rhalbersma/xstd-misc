// Ported from the example on https://en.cppreference.com/w/cpp/types/conditional by cppreference.com contributors, at
// revision https://en.cppreference.com/mwiki/index.php?title=cpp/types/conditional&oldid=152806, and adapted to
// xstd::conditional_data_member by Rein Halbersma, 2026: the type picked where the condition fails is an empty
// member type tagged by the member it stands in for, and the example prints each type's name from a function of its
// own rather than from typeid, whose names differ between compilers.
// Distributed under the Creative Commons Attribution-ShareAlike 3.0 Unported License and the GNU Free Documentation
// License, as cppreference.com's content is: see LICENSE in this directory.

#include <xstd/misc/type_traits/conditional_data_member.hpp> // conditional_data_member, empty_member_type
#include <iostream>                                          // cout
#include <string_view>                                       // string_view
#include <type_traits>                                       // is_same_v

// The tag of the empty member that stands in for an absent counter.
struct counter;

// The name of each type the example picks, the same on every compiler.
template<class T>
[[nodiscard]] constexpr auto name() -> std::string_view
{
        if constexpr (std::is_same_v<T, int>) {
                return "int";
        } else if constexpr (std::is_same_v<T, xstd::empty_member_type<counter>>) {
                return "xstd::empty_member_type<counter>";
        } else {
                return "another type";
        }
}

auto main() -> int
{
        using Type1 = xstd::conditional_data_member<true, int, counter>::type;
        using Type2 = xstd::conditional_data_member<false, int, counter>::type;
        using Type3 = xstd::conditional_data_member<sizeof(int) >= sizeof(double), int, counter>::type;

        std::cout << name<Type1>() << '\n';
        std::cout << name<Type2>() << '\n';
        std::cout << name<Type3>() << '\n';
}
