// Ported from the example on https://en.cppreference.com/w/cpp/language/attributes/no_unique_address by
// cppreference.com contributors, at revision
// https://en.cppreference.com/mwiki/index.php?title=cpp/language/attributes/no_unique_address&oldid=174656, and
// adapted to XSTD_NO_UNIQUE_ADDRESS by Rein Halbersma, 2026: the attribute is spelled through the macro, so MSVC
// gives it the standard's semantics; Y's and W's empty members precede the member they share an address with; and
// W's two empty members are distinct types, through tags, rather than one type twice.
// Distributed under the Creative Commons Attribution-ShareAlike 3.0 Unported License and the GNU Free Documentation
// License, as cppreference.com's content is: see LICENSE in this directory.

#include <xstd/misc/type_traits/empty_member_type.hpp> // empty_member_type
#include <xstd/misc/type_traits/no_unique_address.hpp> // XSTD_NO_UNIQUE_ADDRESS
#include <iostream>                                    // boolalpha, cout

struct Empty
{}; // empty class

struct X
{
        int i;
        Empty e;
};

struct Y
{
        [[XSTD_NO_UNIQUE_ADDRESS]] Empty e;
        int i;
};

struct Z
{
        char c;
        [[XSTD_NO_UNIQUE_ADDRESS]] Empty e1, e2;
};

struct W
{
        [[XSTD_NO_UNIQUE_ADDRESS]] xstd::empty_member_type<struct first> e1;
        [[XSTD_NO_UNIQUE_ADDRESS]] xstd::empty_member_type<struct second> e2;
        char c;
};

auto main() -> int
{
        // the size of any object of empty class type is at least 1
        static_assert(sizeof(Empty) >= 1);

        // at least one more byte is needed to give e a unique address
        static_assert(sizeof(X) >= sizeof(int) + 1);

        // empty member optimized out
        std::cout << "sizeof(Y) == sizeof(int) is " << std::boolalpha << (sizeof(Y) == sizeof(int)) << '\n';

        // e1 and e2 cannot share an address, having the same type, but either may share one with c
        static_assert(sizeof(Z) >= 2);

        // e1 and e2 have different types, so both can share an address with c
        std::cout << "sizeof(W) == 1 is " << (sizeof(W) == 1) << '\n';
}
