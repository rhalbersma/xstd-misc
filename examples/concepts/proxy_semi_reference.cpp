//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/concepts/proxy_semi_reference.hpp> // proxy_semi_reference
#include <bitset>                                      // bitset
#include <iostream>                                    // boolalpha, cout
#include <vector>                                      // vector

// What a generic function is handed for one bit: a bool, or a class that stands in for one.
template<class R>
auto print(R const& bit) -> void
{
        if constexpr (xstd::proxy_semi_reference<R, bool>) {
                std::cout << "a proxy for bool, reading " << static_cast<bool>(bit) << '\n';
        } else {
                std::cout << "a bool, reading " << bit << '\n';
        }
}

auto main() -> int
{
        auto bits       = std::vector<bool>{true, false};
        auto flags      = std::bitset<2>("01");
        auto const flag = true;

        std::cout << std::boolalpha;
        print(bits[0]);
        print(flags[0]);
        print(flag);

        // auto deduces the proxy, not the bool it stands for.
        auto const first = bits[1];
        print(first);
}
