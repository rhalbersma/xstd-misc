//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/functional.hpp> // digest, hash
#include <boost/hash2/flavor.hpp>   // default_flavor
#include <boost/hash2/fnv1a.hpp>    // fnv1a_32, fnv1a_64
#include <boost/hash2/xxhash.hpp>   // xxhash_64
#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_CHECK_NE
#include <cstddef>                  // size_t
#include <cstdint>                  // int64_t, uint64_t
#include <string>                   // string
#include <type_traits>              // conditional_t

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(Functional)

// The front door opens on every room behind it: reached through the umbrella alone.
BOOST_AUTO_TEST_CASE(ReExportsTheWholeDirectory)
{
        auto const unseeded = xstd::hash<std::int64_t>();
        auto const seeded   = xstd::hash<std::int64_t>(1);

        BOOST_CHECK_NE(seeded(42), unseeded(42));
        BOOST_CHECK_NE((xstd::digest<std::int64_t, boost::hash2::xxhash_64>(1)(42)), (xstd::digest<std::int64_t, boost::hash2::xxhash_64>()(42)));
}

// Where the digest is as wide as std::size_t, the hash is that digest unchanged; elsewhere Hash2 narrows or widens it.
BOOST_AUTO_TEST_CASE(AHashIsTheDigestAtTheWidthOfSizeT)
{
        using fnv1a_word      = std::conditional_t<sizeof(std::size_t) == sizeof(std::uint64_t), boost::hash2::fnv1a_64, boost::hash2::fnv1a_32>;
        auto const string_key = std::string("piece");
        BOOST_CHECK_EQUAL((xstd::hash<std::string, fnv1a_word>(7)(string_key)), static_cast<std::size_t>(xstd::digest<std::string, fnv1a_word, boost::hash2::default_flavor>(7)(string_key)));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
