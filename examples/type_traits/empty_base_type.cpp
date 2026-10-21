//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/type_traits/empty_base_type.hpp> // empty_base_type
#include <iostream>                                  // boolalpha, cout
#include <type_traits>                               // conditional_t

struct use_count
{
        int uses = 0;
};

// A widget counts its uses only where Counted holds, and where it does not its base costs no storage.
template<bool Counted>
struct widget : std::conditional_t<Counted, use_count, xstd::empty_base_type<use_count>>
{
        int id = 0;
};

// Whether a class counts its uses, read from whether it has the member that counts them.
template<class T>
concept counts_uses = requires (T const& t) { t.uses; };

auto main() -> int
{
        auto counted = widget<true>();
        ++counted.uses;
        std::cout << "uses: " << counted.uses << '\n';

        std::cout << std::boolalpha;
        std::cout << "widget<true> counts its uses: " << counts_uses<widget<true>> << '\n';
        std::cout << "widget<false> counts its uses: " << counts_uses<widget<false>> << '\n';
        std::cout << "sizeof(widget<false>) == sizeof(int) is " << (sizeof(widget<false>) == sizeof(int)) << '\n';
}
