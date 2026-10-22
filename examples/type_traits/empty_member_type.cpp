//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/type_traits/empty_member_type.hpp> // empty_member_type
#include <iostream>                                    // boolalpha, cout
#include <string>                                      // string
#include <type_traits>                                 // conditional_t

// An entry keeps a label only where Labelled holds; elsewhere an empty stand-in is built and compared in its place.
template<bool Labelled>
struct entry
{
        int value;
        std::conditional_t<Labelled, std::string, xstd::empty_member_type<std::string>> label;

        [[nodiscard]] entry(int v, char const* name)
                : value(v)
                , label(name)
        {}

        [[nodiscard]] friend auto operator==(entry const&, entry const&) -> bool = default;
};

auto main() -> int
{
        std::cout << std::boolalpha;
        std::cout << "labelled entries 1 \"one\" and 1 \"uno\" are equal: " << (entry<true>(1, "one") == entry<true>(1, "uno")) << '\n';
        std::cout << "unlabelled entries 1 \"one\" and 1 \"uno\" are equal: " << (entry<false>(1, "one") == entry<false>(1, "uno")) << '\n';
        std::cout << "unlabelled entries 1 \"one\" and 2 \"two\" are equal: " << (entry<false>(1, "one") == entry<false>(2, "two")) << '\n';
}
