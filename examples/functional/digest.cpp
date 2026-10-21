//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/functional/digest.hpp> // digest
#include <boost/hash2/digest.hpp>          // operator<<
#include <boost/hash2/xxhash.hpp>          // xxhash_64
#include <cstdint>                         // uint64_t
#include <iostream>                        // boolalpha, cout
#include <string>                          // string
#include <string_view>                     // string_view

auto main() -> int
{
        // A digest is the same on every target and in every run, so it can be stored and compared later.
        auto const fingerprint = xstd::digest<std::string>();
        std::cout << fingerprint("Meet the new boss...") << '\n';

        // A string, its view and a C string digest the same.
        auto const text = std::string("Meet the new boss...");
        std::cout << std::boolalpha << (fingerprint(text) == fingerprint(std::string_view(text))) << '\n';

        // A seed gives another digest, at the algorithm's own width: 64 bits for xxhash_64, on a 32-bit target too.
        auto const seeded = xstd::digest<std::uint64_t, boost::hash2::xxhash_64>(42);
        std::cout << seeded(std::uint64_t{2026}) << '\n';
}
