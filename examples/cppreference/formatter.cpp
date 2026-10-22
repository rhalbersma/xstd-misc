// Ported from the example on https://en.cppreference.com/w/cpp/utility/format/formatter by cppreference.com
// contributors, at revision
// https://en.cppreference.com/mwiki/index.php?title=cpp/utility/format/formatter&oldid=177164, and adapted to the
// formatter for boost::hash2::digest by Rein Halbersma, 2026: the formatter is the library's rather than one the
// example defines, the arguments are digests rather than strings, and the specifications are x and X rather than #.
// Distributed under the Creative Commons Attribution-ShareAlike 3.0 Unported License and the GNU Free Documentation
// License, as cppreference.com's content is: see LICENSE in this directory.

#include <xstd/misc/format/digest.hpp> // formatter
#include <boost/hash2/digest.hpp>      // digest
#include <format>                      // format
#include <iostream>                    // cout

auto main() -> int
{
        auto const a = boost::hash2::digest<1>({0xbe});
        auto const b = boost::hash2::digest<4>({0xde, 0xad, 0xbe, 0xef});
        std::cout << std::format("To {0} or not to {0}, that is {1}.\n", a, b);
        std::cout << std::format("To {0:x} or not to {0:x}, that is {1:x}.\n", a, b);
        std::cout << std::format("To {0:X} or not to {0:X}, that is {1:X}.\n", a, b);
}
