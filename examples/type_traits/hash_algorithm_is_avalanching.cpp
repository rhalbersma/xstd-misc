//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/functional/hash.hpp>                           // hash
#include <xstd/misc/type_traits/hash_algorithm_is_avalanching.hpp> // hash_algorithm_is_avalanching_v
#include <boost/hash2/fnv1a.hpp>                                   // fnv1a_64
#include <boost/hash2/hmac.hpp>                                    // hmac
#include <boost/hash2/sha2.hpp>                                    // sha2_256
#include <boost/hash2/xxhash.hpp>                                  // xxhash_64
#include <iostream>                                                // boolalpha, cout
#include <string>                                                  // string

auto main() -> int
{
        std::cout << std::boolalpha;
        std::cout << "xxhash_64 avalanches: " << xstd::hash_algorithm_is_avalanching_v<boost::hash2::xxhash_64> << '\n';
        std::cout << "fnv1a_64 avalanches: " << xstd::hash_algorithm_is_avalanching_v<boost::hash2::fnv1a_64> << '\n';
        std::cout << "hmac<sha2_256> avalanches: " << xstd::hash_algorithm_is_avalanching_v<boost::hash2::hmac<boost::hash2::sha2_256>> << '\n';

        // A hash declares what its algorithm answers, and an open-addressing table that reads it skips its own mixing.
        std::cout << "hash<std::string>::is_avalanching: " << xstd::hash<std::string>::is_avalanching::value << '\n';
        std::cout << "hash<std::string, fnv1a_64>::is_avalanching: " << xstd::hash<std::string, boost::hash2::fnv1a_64>::is_avalanching::value << '\n';
}
