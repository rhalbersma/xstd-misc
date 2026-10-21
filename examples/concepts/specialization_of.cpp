//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/concepts/specialization_of.hpp> // specialization_of
#include <deque>                                    // deque
#include <iostream>                                 // boolalpha, cout
#include <vector>                                   // vector

// A view of the last element of any std::vector, which may be const: the owner's type names its constness.
template<xstd::specialization_of<std::vector> Owner>
class back_view
{
        Owner* m_owner;

public:
        [[nodiscard]] constexpr explicit back_view(Owner& owner) noexcept
                : m_owner(&owner)
        {}

        [[nodiscard]] constexpr auto get() const
                -> auto&
        {
                return m_owner->back();
        }
};

template<class Owner>
back_view(Owner&) -> back_view<Owner>;

auto main() -> int
{
        auto numbers       = std::vector<int>{1, 2, 3};
        auto const letters = std::vector<char>{'a', 'b', 'c'};

        back_view(numbers).get() = 4;
        std::cout << numbers.back() << ' ' << back_view(letters).get() << '\n';

        std::cout << std::boolalpha;
        std::cout << "specialization_of<std::vector<int>, std::vector> is " << xstd::specialization_of<std::vector<int>, std::vector> << '\n';
        std::cout << "specialization_of<std::vector<int> const, std::vector> is " << xstd::specialization_of<std::vector<int> const, std::vector> << '\n';
        std::cout << "specialization_of<std::vector<int>&, std::vector> is " << xstd::specialization_of<std::vector<int>&, std::vector> << '\n';
        std::cout << "specialization_of<std::deque<int>, std::vector> is " << xstd::specialization_of<std::deque<int>, std::vector> << '\n';
}
