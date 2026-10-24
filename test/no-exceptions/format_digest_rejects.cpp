//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/format/digest.hpp> // formatter<boost::hash2::digest<N>>
#include <boost/hash2/digest.hpp>      // digest
#include <format>                      // make_format_args, vformat

// A width is the habit from integers, and a digest takes none: the formatter aborts with its message.
auto main() -> int
{
        auto d = boost::hash2::digest<4>();
        static_cast<void>(std::vformat("{:08x}", std::make_format_args(d)));
        return 0;
}
