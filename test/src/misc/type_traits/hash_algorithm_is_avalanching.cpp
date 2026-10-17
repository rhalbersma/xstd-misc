//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/concepts/hash_algorithm.hpp>                   // hash_algorithm
#include <xstd/misc/functional/hash.hpp>                           // hash
#include <xstd/misc/type_traits/hash_algorithm_is_avalanching.hpp> // hash_algorithm_is_avalanching, hash_algorithm_is_avalanching_v
#include <test/constexpr_check.hpp>                                // XSTD_CONSTEXPR_CHECK
#include <test/hash2_algorithms.hpp>                               // algorithms
#include <boost/hash2/fnv1a.hpp>                                   // fnv1a_32, fnv1a_64
#include <boost/hash2/legacy/murmur3.hpp>                          // murmur3_128, murmur3_32
#include <boost/hash2/legacy/spooky2.hpp>                          // spooky2_128
#include <boost/hash2/siphash.hpp>                                 // siphash_32, siphash_64
#include <boost/hash2/xxh3.hpp>                                    // xxh3_128
#include <boost/hash2/xxhash.hpp>                                  // xxhash_32, xxhash_64
#include <boost/test/unit_test.hpp>                                // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK_GT, BOOST_CHECK_LT
#include <algorithm>                                               // max
#include <cmath>                                                   // abs, log, sqrt
#include <concepts>                                                // same_as
#include <cstddef>                                                 // size_t
#include <cstdint>                                                 // uint64_t
#include <limits>                                                  // numeric_limits
#include <random>                                                  // mt19937_64
#include <tuple>                                                   // tuple
#include <type_traits>                                             // true_type
#include <vector>                                                  // vector

namespace {

constexpr auto seed      = std::uint64_t{0x9e37'79b9'7f4a'7c15};
constexpr auto key_bits  = std::size_t{std::numeric_limits<std::uint64_t>::digits};
constexpr auto hash_bits = std::size_t{std::numeric_limits<std::size_t>::digits};

template<class H, class Algorithms>
constexpr auto listed = false;

template<class H, class... Algorithms>
constexpr auto listed<H, std::tuple<Algorithms...>> = (std::same_as<H, Algorithms> or ...);

using table_algorithms = std::tuple<
        boost::hash2::fnv1a_32, boost::hash2::fnv1a_64,
        boost::hash2::xxhash_32, boost::hash2::xxhash_64, boost::hash2::xxh3_128,
        boost::hash2::siphash_32, boost::hash2::siphash_64,
        boost::hash2::murmur3_32, boost::hash2::murmur3_128, boost::hash2::spooky2_128>;

// The algorithms a table runs are measured finely; a cryptographic one avalanches by design, and is checked coarsely.
template<class H>
constexpr auto avalanche_samples = listed<H, table_algorithms> ? std::size_t{4096} : std::size_t{256};

// The worst |rate - 1/2| at which flipping one bit of a random key flips one bit of its hash, over every such pair.
template<class H>
auto worst_avalanche_bias()
        -> double
{
        // NOLINTNEXTLINE(bugprone-random-generator-seed): a fixed seed gives every run and platform the same verdict.
        auto keys    = std::mt19937_64(seed);
        auto const h = xstd::hash<std::uint64_t, H>(seed);
        auto flips   = std::vector<std::size_t>(key_bits * hash_bits);
        for (auto n = std::size_t{0}; n < avalanche_samples<H>; ++n) {
                auto const k    = std::uint64_t{keys()};
                auto const hash = h(k);
                for (auto i = std::size_t{0}; i < key_bits; ++i) {
                        auto const flipped = hash ^ h(k ^ (std::uint64_t{1} << i));
                        for (auto j = std::size_t{0}; j < hash_bits; ++j) {
                                flips[(i * hash_bits) + j] += (flipped >> j) & 1U;
                        }
                }
        }
        auto worst = 0.0;
        for (auto const count : flips) {
                worst = std::max(worst, std::abs((static_cast<double>(count) / avalanche_samples<H>)-0.5));
        }
        return worst;
}

// Hoeffding's inequality, union-bounded over every pair: an ideal hash exceeds it less than once in a million.
auto avalanche_bound(std::size_t samples)
        -> double
{
        constexpr auto false_failure = 1e-6;
        return std::sqrt(std::log(2.0 * key_bits * hash_bits / false_failure) / (2.0 * static_cast<double>(samples)));
}

template<class H>
constexpr auto is_fnv1a = std::same_as<H, boost::hash2::fnv1a_32> or std::same_as<H, boost::hash2::fnv1a_64>;

// A user's own algorithm, built on xxHash and declared avalanching by its author.
class declared_xxhash : public boost::hash2::xxhash_64
{
public:
        using xxhash_64::xxhash_64;
};

// The same, with no declaration: deriving from an avalanching algorithm does not inherit its answer.
class undeclared_xxhash : public boost::hash2::xxhash_64
{
public:
        using xxhash_64::xxhash_64;
};

} // namespace

template<>
struct xstd::hash_algorithm_is_avalanching<declared_xxhash> : std::true_type
{};

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(TypeTraits)
BOOST_AUTO_TEST_SUITE(HashAlgorithmIsAvalanching)

BOOST_AUTO_TEST_CASE_TEMPLATE(EveryHash2AlgorithmButFnv1aAvalanches, H, test::algorithms)
{
        XSTD_CONSTEXPR_CHECK((xstd::hash_algorithm_is_avalanching_v<H> == not is_fnv1a<H>));
}

// The Strict Avalanche Criterion, measured through the hash's whole path: seed, hash_append, and the fold to size_t.
BOOST_AUTO_TEST_CASE_TEMPLATE(AgreesWithTheAvalancheCriterion, H, test::algorithms)
{
        if constexpr (xstd::hash_algorithm_is_avalanching_v<H>) {
                BOOST_CHECK_LT(worst_avalanche_bias<H>(), avalanche_bound(avalanche_samples<H>));
        } else {
                BOOST_CHECK_GT(worst_avalanche_bias<H>(), avalanche_bound(avalanche_samples<H>));
        }
}

BOOST_AUTO_TEST_CASE(AnAlgorithmOutsideHash2AvalanchesOnceItsAuthorSaysSo)
{
        XSTD_CONSTEXPR_CHECK(xstd::hash_algorithm<declared_xxhash>);
        XSTD_CONSTEXPR_CHECK(xstd::hash_algorithm_is_avalanching_v<declared_xxhash>);
        XSTD_CONSTEXPR_CHECK((xstd::hash<int, declared_xxhash>::is_avalanching::value));
        XSTD_CONSTEXPR_CHECK(not xstd::hash_algorithm_is_avalanching_v<undeclared_xxhash>);
        XSTD_CONSTEXPR_CHECK((not xstd::hash<int, undeclared_xxhash>::is_avalanching::value));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
