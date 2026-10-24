//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/format/digest.hpp> // formatter<boost::hash2::digest<N>>
#include <boost/hash2/digest.hpp>      // digest
#include <algorithm>                   // copy
#include <array>                       // array
#include <format>                      // format
#include <string_view>                 // string_view

auto main() -> int
{
        constexpr auto bytes = std::array<unsigned char, 4>{0x00, 0x01, 0xab, 0xff};
        auto d = boost::hash2::digest<4>();
        std::ranges::copy(bytes, d.begin());

        using namespace std::string_view_literals;
        auto const ok = std::format("{}", d) == "0001abff"sv and std::format("{:x}", d) == "0001abff"sv and std::format("{:X}", d) == "0001ABFF"sv;
        return ok ? 0 : 1;
}
