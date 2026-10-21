//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/concepts/hash_algorithm.hpp> // hash_algorithm
#include <xstd/misc/functional/hash.hpp>         // hash
#include <boost/hash2/fnv1a.hpp>                 // fnv1a_32
#include <boost/hash2/xxhash.hpp>                // xxhash_32
#include <functional>                            // hash
#include <iomanip>                               // setfill, setw
#include <iostream>                              // boolalpha, cout, hex
#include <string_view>                           // string_view

// Hashes the bytes of a message under any Boost.Hash2 algorithm, unseeded.
template<xstd::hash_algorithm H>
[[nodiscard]] auto hash_bytes(std::string_view message)
        -> typename H::result_type
{
        auto h = H();
        h.update(message.data(), message.size());
        return h.result();
}

auto main() -> int
{
        std::cout << std::hex << std::setfill('0');
        std::cout << "fnv1a_32(\"xstd\")  = " << std::setw(8) << hash_bytes<boost::hash2::fnv1a_32>("xstd") << '\n';
        std::cout << "xxhash_32(\"xstd\") = " << std::setw(8) << hash_bytes<boost::hash2::xxhash_32>("xstd") << '\n';

        // A hasher takes a key where an algorithm takes bytes, so neither the standard's nor this library's is one.
        std::cout << std::boolalpha;
        std::cout << "hash_algorithm<boost::hash2::xxhash_32> is " << xstd::hash_algorithm<boost::hash2::xxhash_32> << '\n';
        std::cout << "hash_algorithm<std::hash<int>> is " << xstd::hash_algorithm<std::hash<int>> << '\n';
        std::cout << "hash_algorithm<xstd::hash<int>> is " << xstd::hash_algorithm<xstd::hash<int>> << '\n';
}
