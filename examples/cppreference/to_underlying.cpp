// Ported from the example on https://en.cppreference.com/w/cpp/utility/to_underlying by cppreference.com
// contributors, at revision https://en.cppreference.com/mwiki/index.php?title=cpp/utility/to_underlying&oldid=158870,
// and adapted to xstd::to_underlying by Rein Halbersma, 2026: it calls xstd::to_underlying where the original calls
// std::to_underlying, and adds an enumerator kept in std::integral_constant, which only xstd::to_underlying takes.
// Distributed under the Creative Commons Attribution-ShareAlike 3.0 Unported License and the GNU Free Documentation
// License, as cppreference.com's content is: see LICENSE in this directory.

#include <xstd/misc/utility/to_underlying.hpp> // to_underlying
#include <cstdint>                             // uint32_t
#include <iomanip>                             // setfill, setw
#include <iostream>                            // cout, hex, uppercase
#include <type_traits>                         // integral_constant, is_same_v, underlying_type_t

enum class E1 : char
{
        e
};
static_assert(std::is_same_v<char, decltype(xstd::to_underlying(E1::e))>);

enum struct E2 : long
{
        e
};
static_assert(std::is_same_v<long, decltype(xstd::to_underlying(E2::e))>);

enum E3 : unsigned
{
        e
};

static_assert(std::is_same_v<unsigned, decltype(xstd::to_underlying(e))>);

// An enumerator kept at the type level converts to a value kept at the type level.
static_assert(std::is_same_v<std::integral_constant<char, 0>, decltype(xstd::to_underlying(std::integral_constant<E1, E1::e>()))>);

auto main() -> int
{
        enum class ColorMask : std::uint32_t
        {
                red   = 0xFF,
                green = (red << 8),
                blue  = (green << 8),
                alpha = (blue << 8)
        };

        std::cout << std::hex << std::uppercase << std::setfill('0')
                  << std::setw(8) << xstd::to_underlying(ColorMask::red) << '\n'
                  << std::setw(8) << xstd::to_underlying(ColorMask::green) << '\n'
                  << std::setw(8) << xstd::to_underlying(ColorMask::blue) << '\n'
                  << std::setw(8) << xstd::to_underlying(ColorMask::alpha) << '\n';

        // std::underlying_type_t<ColorMask> x = ColorMask::alpha; // Error: no known conversion
        [[maybe_unused]] std::underlying_type_t<ColorMask> const y = xstd::to_underlying(ColorMask::alpha); // OK
}
