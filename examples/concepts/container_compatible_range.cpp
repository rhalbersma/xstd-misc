//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/concepts/container_compatible_range.hpp> // container_compatible_range
#include <iostream>                                          // boolalpha, cout
#include <list>                                              // list
#include <ranges>                                            // iota
#include <string>                                            // string
#include <vector>                                            // vector

// Builds a vector of T from any range whose elements convert to T, as a container's from_range constructor does.
template<class T, xstd::container_compatible_range<T> R>
[[nodiscard]] auto to_vector(R&& r)
        -> std::vector<T>
{
        auto v = std::vector<T>();
        for (auto&& e : r) {
                v.emplace_back(e);
        }
        return v;
}

template<class T>
auto print(std::vector<T> const& v) -> void
{
        auto separator = "";
        for (auto const& e : v) {
                std::cout << separator << e;
                separator = " ";
        }
        std::cout << '\n';
}

auto main() -> int
{
        print(to_vector<long>(std::list<int>{1, 2, 3}));
        print(to_vector<std::string>(std::vector<char const*>{"from", "a", "range"}));
        print(to_vector<unsigned>(std::views::iota(4U, 7U)));

        // A string is a range of characters, none of which converts to a string.
        std::cout << std::boolalpha;
        std::cout << "container_compatible_range<std::string, std::string> is " << xstd::container_compatible_range<std::string, std::string> << '\n';
        std::cout << "container_compatible_range<std::vector<std::string>, std::string> is " << xstd::container_compatible_range<std::vector<std::string>, std::string> << '\n';
}
