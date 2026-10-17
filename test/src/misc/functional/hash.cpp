//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/functional/hash.hpp>   // hash
#include <xstd/misc/functional/hasher.hpp> // hasher
#include <test/constexpr_check.hpp>        // XSTD_CONSTEXPR_CHECK
#include <test/hash2_algorithms.hpp>       // algorithms
#include <boost/hash2/fnv1a.hpp>           // fnv1a_64
#include <boost/hash2/siphash.hpp>         // siphash_64
#include <boost/hash2/xxhash.hpp>          // xxhash_64
#include <boost/test/unit_test.hpp>        // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_CHECK_NE
#include <concepts>                        // same_as
#include <cstddef>                         // size_t
#include <cstdint>                         // int64_t, uint64_t
#include <functional>                      // equal_to
#include <string>                          // string
#include <string_view>                     // string_view
#include <type_traits>                     // is_constructible_v, is_convertible_v
#include <unordered_set>                   // unordered_set

#if __has_include(<boost/unordered/unordered_flat_set.hpp>)
#define TEST_HAS_BOOST_UNORDERED
#include <boost/container_hash/hash_is_avalanching.hpp> // hash_is_avalanching
#include <boost/unordered/unordered_flat_set.hpp>       // unordered_flat_set
#endif

namespace {

constexpr auto seed = std::uint64_t{0x9e37'79b9'7f4a'7c15};

template<class Hash>
concept transparent = requires { typename Hash::is_transparent; };

} // namespace

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(Functional)
BOOST_AUTO_TEST_SUITE(Hash)

BOOST_AUTO_TEST_CASE(TheDefaultAlgorithmIsXxhash64)
{
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::hash<int>, xstd::hash<int, boost::hash2::xxhash_64>>));
}

// No integer converts to a seed by accident.
BOOST_AUTO_TEST_CASE(SeedsAreExplicit)
{
        XSTD_CONSTEXPR_CHECK((std::is_constructible_v<xstd::hash<int>, std::uint64_t>));
        XSTD_CONSTEXPR_CHECK((not std::is_convertible_v<std::uint64_t, xstd::hash<int>>));
        XSTD_CONSTEXPR_CHECK((std::is_constructible_v<xstd::hash<int>, unsigned char const*, std::size_t>));
}

BOOST_AUTO_TEST_CASE(IsConstructibleInConstantExpressions)
{
        [[maybe_unused]] constexpr auto unseeded = xstd::hash<int>();
        [[maybe_unused]] constexpr auto seeded   = xstd::hash<int>(seed);

        BOOST_CHECK(true); // silence Boost.Test's "test case did not check any assertions"
}

// At its own key type, the hash is the hasher under the same algorithm and seed.
BOOST_AUTO_TEST_CASE_TEMPLATE(IsTheHasherAtItsKeyType, H, test::algorithms)
{
        auto const h = xstd::hash<std::int64_t, H>(seed);
        auto const g = xstd::hasher<H>(seed);

        BOOST_CHECK_EQUAL(h(std::int64_t{42}), g(std::int64_t{42}));
        BOOST_CHECK(h.digest(std::int64_t{42}) == g.digest(std::int64_t{42}));
}

// 42 converts to the key's type first, so it hashes as the std::int64_t{42} a table holds.
BOOST_AUTO_TEST_CASE(ConvertsEachArgumentToItsKeyType)
{
        auto const h = xstd::hash<std::int64_t>();

        BOOST_CHECK_EQUAL(h(42), h(std::int64_t{42}));
        BOOST_CHECK_NE(xstd::hasher<>()(42), h(std::int64_t{42}));
}

// A string, its view and a C string append the same characters, under every algorithm.
BOOST_AUTO_TEST_CASE_TEMPLATE(AStringHashesAsItsViewAndItsCString, H, test::algorithms)
{
        auto const h      = xstd::hash<std::string, H>(seed);
        auto const string = std::string("piece");

        BOOST_CHECK_EQUAL(h(std::string_view(string)), h(string));
        BOOST_CHECK_EQUAL(h("piece"), h(string));
        BOOST_CHECK_EQUAL(h(string), xstd::hasher<H>(seed)(string));
}

// Only a string key is transparent: elsewhere equal values of other types need not append the same bytes.
BOOST_AUTO_TEST_CASE(IsTransparentOverStringsOnly)
{
        XSTD_CONSTEXPR_CHECK(transparent<xstd::hash<std::string>>);
        XSTD_CONSTEXPR_CHECK(not transparent<xstd::hash<std::string_view>>);
        XSTD_CONSTEXPR_CHECK(not transparent<xstd::hash<std::int64_t>>);
}

BOOST_AUTO_TEST_CASE(ReportsWhetherItsAlgorithmAvalanches)
{
        XSTD_CONSTEXPR_CHECK(xstd::hash<std::string>::is_avalanching::value);
        XSTD_CONSTEXPR_CHECK((not xstd::hash<std::string, boost::hash2::fnv1a_64>::is_avalanching::value));
#ifdef TEST_HAS_BOOST_UNORDERED
        XSTD_CONSTEXPR_CHECK(boost::hash_is_avalanching<xstd::hash<std::string>>::value);
#endif
}

// Heterogeneous lookup in a set of strings, by a view and by a literal, with no string built for either.
BOOST_AUTO_TEST_CASE(FindsAStringKeyByItsViewInAStdUnorderedSet)
{
        using string_hash = xstd::hash<std::string, boost::hash2::siphash_64>;

        auto s = std::unordered_set<std::string, string_hash, std::equal_to<>>(0, string_hash(seed));
        s.insert({"one", "two", "three"});

        BOOST_CHECK(s.contains(std::string_view("two")));
        BOOST_CHECK(s.contains("two"));
        BOOST_CHECK(not s.contains(std::string_view("four")));
}

#ifdef TEST_HAS_BOOST_UNORDERED

BOOST_AUTO_TEST_CASE(FindsAStringKeyByItsViewInABoostUnorderedFlatSet)
{
        using string_hash = xstd::hash<std::string, boost::hash2::siphash_64>;

        auto s = boost::unordered_flat_set<std::string, string_hash, std::equal_to<>>(0, string_hash(seed));
        s.insert({"one", "two", "three"});

        BOOST_CHECK(s.contains(std::string_view("two")));
        BOOST_CHECK(s.contains("two"));
        BOOST_CHECK(not s.contains(std::string_view("four")));
}

#endif

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
