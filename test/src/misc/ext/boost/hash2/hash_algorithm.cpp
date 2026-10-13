//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <test/constexpr_check.hpp> // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK
#include <array>                    // array
#include <concepts>                 // same_as
#include <cstdint>                  // uint32_t

// Reached the way a consumer reaches it: the probe here, the adapter behind it.
#if __has_include(<boost/hash2/hash_append.hpp>)
#define TEST_HAS_BOOST_HASH2
#include <xstd/misc/ext/boost/hash2/hash_algorithm.hpp> // hash_algorithm
#include <test/hash2_algorithms.hpp>                    // algorithms
#include <boost/hash2/digest.hpp>                       // digest
#include <boost/hash2/fnv1a.hpp>                        // fnv1a_32
#include <boost/hash2/legacy/murmur3.hpp>               // murmur3_128
#include <boost/hash2/md5.hpp>                          // md5_128
#endif

#ifdef TEST_HAS_BOOST_HASH2

namespace {

template<class H>
concept has_block_size = requires { H::block_size; };

} // namespace

#endif

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(Ext)
BOOST_AUTO_TEST_SUITE(Boost)
BOOST_AUTO_TEST_SUITE(Hash2)
BOOST_AUTO_TEST_SUITE(HashAlgorithm)

#ifdef TEST_HAS_BOOST_HASH2

BOOST_AUTO_TEST_CASE_TEMPLATE(EveryHash2AlgorithmIsAHashAlgorithm, H, test::algorithms)
{
        XSTD_CONSTEXPR_CHECK(xstd::hash_algorithm<H>);
}

// An unsigned integer or a fixed-size byte array for a result, a block size or none: Hash2 has each form.
BOOST_AUTO_TEST_CASE(EachFormHash2TakesIsAHashAlgorithm)
{
        XSTD_CONSTEXPR_CHECK((std::same_as<boost::hash2::fnv1a_32::result_type, std::uint32_t>));
        XSTD_CONSTEXPR_CHECK((std::same_as<boost::hash2::md5_128::result_type, boost::hash2::digest<16>>));
        XSTD_CONSTEXPR_CHECK((std::same_as<boost::hash2::murmur3_128::result_type, std::array<unsigned char, 16>>));
        XSTD_CONSTEXPR_CHECK(not has_block_size<boost::hash2::fnv1a_32>);
        XSTD_CONSTEXPR_CHECK(has_block_size<boost::hash2::md5_128>);

        XSTD_CONSTEXPR_CHECK(xstd::hash_algorithm<boost::hash2::fnv1a_32>);
        XSTD_CONSTEXPR_CHECK(xstd::hash_algorithm<boost::hash2::md5_128>);
        XSTD_CONSTEXPR_CHECK(xstd::hash_algorithm<boost::hash2::murmur3_128>);
}

#else

BOOST_AUTO_TEST_CASE(NeedsBoostHash2)
{
        BOOST_CHECK(true);
}

#endif

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
