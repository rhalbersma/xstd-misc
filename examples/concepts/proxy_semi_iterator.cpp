//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/concepts/proxy_semi_iterator.hpp> // proxy_semi_iterator
#include <iostream>                                   // boolalpha, cout
#include <iterator>                                   // iter_value_t, next
#include <vector>                                     // vector

// Copies out the element an iterator points to, as its value type rather than as the proxy a semi-iterator yields.
template<class I>
[[nodiscard]] auto value_at(I it)
        -> std::iter_value_t<I>
{
        if constexpr (xstd::proxy_semi_iterator<I>) {
                std::cout << "through a proxy: ";
        } else {
                std::cout << "through a language reference: ";
        }
        return *it;
}

auto main() -> int
{
        auto bits        = std::vector<bool>{false, true};
        auto const words = std::vector<int>{1, 2, 3};

        std::cout << std::boolalpha;
        std::cout << value_at(std::next(bits.begin())) << '\n';
        std::cout << value_at(words.begin()) << '\n';
        std::cout << value_at(words.data()) << '\n';

        std::cout << "proxy_semi_iterator<vector<bool>::iterator> is " << xstd::proxy_semi_iterator<std::vector<bool>::iterator> << '\n';
        std::cout << "proxy_semi_iterator<vector<int>::iterator> is " << xstd::proxy_semi_iterator<std::vector<int>::iterator> << '\n';
        std::cout << "proxy_semi_iterator<int*> is " << xstd::proxy_semi_iterator<int*> << '\n';
}
