//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/type_traits/is_specialization_of.hpp> // is_specialization_of_v
#include <iostream>                                       // boolalpha, cout
#include <optional>                                       // optional
#include <string>                                         // basic_string, string

// Prints a value, and what an optional holds rather than the optional itself.
template<class T>
auto print(T const& value) -> void
{
        if constexpr (xstd::is_specialization_of_v<T, std::optional>) {
                if (value.has_value()) {
                        print(*value);
                } else {
                        std::cout << "nothing";
                }
        } else {
                std::cout << value;
        }
}

auto main() -> int
{
        print(42);
        std::cout << ' ';
        print(std::optional<int>(7));
        std::cout << ' ';
        print(std::optional<int>());
        std::cout << ' ';
        print(std::optional<std::string>("text"));
        std::cout << '\n';

        // The question is exact: a const specialization is not one.
        std::cout << std::boolalpha;
        std::cout << "is_specialization_of_v<std::string, std::basic_string> is " << xstd::is_specialization_of_v<std::string, std::basic_string> << '\n';
        std::cout << "is_specialization_of_v<std::optional<int> const, std::optional> is " << xstd::is_specialization_of_v<std::optional<int> const, std::optional> << '\n';
}
