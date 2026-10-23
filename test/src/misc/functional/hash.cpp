//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/concepts/hash_algorithm.hpp>                   // hash_algorithm
#include <xstd/misc/functional/hash.hpp>                           // hash
#include <xstd/misc/type_traits/hash_algorithm_is_avalanching.hpp> // hash_algorithm_is_avalanching_v
#include <test/constexpr_check.hpp>                                // XSTD_CONSTEXPR_CHECK
#include <test/hash2_algorithms.hpp>                               // algorithms, constexpr_algorithms
#include <boost/hash2/flavor.hpp>                                  // default_flavor
#include <boost/hash2/fnv1a.hpp>                                   // fnv1a_64
#include <boost/hash2/get_integral_result.hpp>                     // get_integral_result
#include <boost/hash2/hash_append.hpp>                             // hash_append
#include <boost/hash2/siphash.hpp>                                 // siphash_32, siphash_64
#include <boost/hash2/xxhash.hpp>                                  // xxhash_32, xxhash_64
#include <boost/test/unit_test.hpp>                                // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_CHECK_GT, BOOST_CHECK_LT, BOOST_CHECK_NE
#include <array>                                                   // array
#include <bit>                                                     // endian, has_single_bit
#include <bitset>                                                  // bitset
#include <chrono>                                                  // Friday, October, Sunday, last, seconds, sys_seconds, weekday, year_month_day
#include <concepts>                                                // same_as
#include <coroutine>                                               // coroutine_handle, noop_coroutine
#include <cstddef>                                                 // size_t
#include <cstdint>                                                 // int64_t, uint32_t, uint64_t
#include <filesystem>                                              // path
#include <functional>                                              // equal_to, hash, not_equal_to
#include <initializer_list>                                        // initializer_list
#include <limits>                                                  // numeric_limits
#include <memory>                                                  // make_shared, make_unique, weak_ptr
#include <optional>                                                // nullopt, optional
#include <set>                                                     // set
#include <string>                                                  // string
#include <string_view>                                             // string_view
#include <system_error>                                            // error_code, error_condition, generic_category, system_category
#include <thread>                                                  // thread
#include <type_traits>                                             // conditional_t, is_constructible_v, is_convertible_v
#include <typeindex>                                               // type_index
#include <unordered_set>                                           // unordered_set
#include <utility>                                                 // declval, pair
#include <vector>                                                  // vector
#include <version>                                                 // IWYU pragma: keep; __cpp_lib_stacktrace, __cpp_lib_text_encoding

#ifdef __cpp_lib_stacktrace
#include <stacktrace> // IWYU pragma: keep; stacktrace_entry
#endif

#if defined(__STDCPP_FLOAT16_T__) || defined(__STDCPP_FLOAT128_T__)
#include <stdfloat> // IWYU pragma: keep; float128_t, float16_t
#endif

#ifdef __cpp_lib_text_encoding
#include <text_encoding> // IWYU pragma: keep; text_encoding
#endif

#if __has_include(<boost/unordered/unordered_flat_set.hpp>)
#define TEST_HAS_BOOST_UNORDERED
#define BOOST_UNORDERED_ENABLE_STATS
#include <boost/container_hash/hash_is_avalanching.hpp> // hash_is_avalanching
#include <boost/unordered/unordered_flat_set.hpp>       // unordered_flat_set
#endif

namespace {

constexpr auto seed       = std::uint64_t{0x9e37'79b9'7f4a'7c15};
constexpr auto other_seed = std::uint64_t{0xc2b2'ae3d'27d4'eb4f};
constexpr auto key        = std::array<unsigned char, 16>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

// Whether a hash over H can be named at all, rather than failing to compile.
template<class H>
concept algorithm_argument = requires { typename xstd::hash<int, H>; };

// Whether a hash of Key over a pair of algorithms, the 64-bit one first, can be named at all.
template<class H64, class H32, class Key = int>
concept algorithm_pair = requires { typename xstd::hash<Key, H64, H32>; };

template<class Hash>
concept transparent = requires { typename Hash::is_transparent; };

// Sixteen IDs that differ only in a tag in the high four bits of the byte hash_append writes last, in native order.
constexpr auto tagged_ids = []() -> std::array<std::uint64_t, 16> {
        constexpr auto tag_shift = []() -> unsigned {
                if constexpr (std::endian::native == std::endian::little) {
                        return 60U;
                } else {
                        return 4U;
                }
        }();
        auto ids = std::array<std::uint64_t, 16>();
        for (auto i = std::size_t{0}; i < ids.size(); ++i) {
                ids[i] = static_cast<std::uint64_t>(i) << tag_shift;
        }
        return ids;
}();

// The bucket count of a standard table asked for 8 and never growing, and how many of them the tagged IDs occupy.
template<class Hash>
auto bucket_spread()
        -> std::pair<std::size_t, std::size_t>
{
        auto table = std::unordered_set<std::uint64_t, Hash>();
        table.max_load_factor(static_cast<float>(tagged_ids.size()));
        table.rehash(8);
        table.insert(tagged_ids.begin(), tagged_ids.end());
        auto occupied = std::set<std::size_t>();
        for (auto const id : tagged_ids) {
                occupied.insert(table.bucket(id));
        }
        return {table.bucket_count(), occupied.size()};
}

// What Hash2 makes of a key, with no hash in between.
template<class H, class T>
auto hash2_of(H algorithm, T const& v)
        -> std::size_t
{
        boost::hash2::hash_append(algorithm, {}, v);
        return boost::hash2::get_integral_result<std::size_t>(algorithm);
}

// Two equal keys hash equal, and a third, unequal to them, hashes apart.
template<class Key>
auto check_equal_and_apart(std::pair<Key, Key> const& equal_keys, Key const& other_key)
        -> void
{
        auto const h = xstd::hash<Key>(seed);
        BOOST_CHECK(std::equal_to<>()(equal_keys.first, equal_keys.second));
        BOOST_CHECK(std::not_equal_to<>()(equal_keys.first, other_key));
        BOOST_CHECK_EQUAL(h(equal_keys.first), h(equal_keys.second));
        BOOST_CHECK_NE(h(equal_keys.first), h(other_key));
}

} // namespace

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(Functional)
BOOST_AUTO_TEST_SUITE(Hash)

// A hash is the mistake to expect: each hashes a key, and xstd::hash even takes both seeds, yet neither updates.
BOOST_AUTO_TEST_CASE(AHashIsNotAHashAlgorithm)
{
        XSTD_CONSTEXPR_CHECK(not xstd::hash_algorithm<std::hash<int>>);
        XSTD_CONSTEXPR_CHECK(not xstd::hash_algorithm<xstd::hash<int>>);
}

// A table's hash never leaves the process, so it appends in the target's own byte order.
BOOST_AUTO_TEST_CASE(TheDefaultAlgorithmIsXxhash64InNativeBytes)
{
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::hash<int>, xstd::hash<int, boost::hash2::xxhash_64, boost::hash2::xxhash_64, boost::hash2::default_flavor>>));
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::hash<int>::hash_type, boost::hash2::xxhash_64>));
}

// One algorithm named alone runs on every target, so a seeded SipHash is SipHash wherever the table is built.
BOOST_AUTO_TEST_CASE(OneAlgorithmNamedAloneRunsOnEveryTarget)
{
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::hash<std::string, boost::hash2::siphash_64>::hash_type, boost::hash2::siphash_64>));
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::hash<std::string, boost::hash2::siphash_32>::hash_type, boost::hash2::siphash_32>));
}

// A pair runs the one for the target's std::size_t; given 32-bit first, as two names may be read, it is rejected.
BOOST_AUTO_TEST_CASE(APairRunsTheAlgorithmForTheTargetsSizeT)
{
        using siphash = std::conditional_t<std::numeric_limits<std::size_t>::digits <= 32, boost::hash2::siphash_32, boost::hash2::siphash_64>;
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::hash<std::string, boost::hash2::siphash_64, boost::hash2::siphash_32>::hash_type, siphash>));
        XSTD_CONSTEXPR_CHECK((algorithm_pair<boost::hash2::xxhash_64, boost::hash2::xxhash_32>));
        XSTD_CONSTEXPR_CHECK((not algorithm_pair<boost::hash2::xxhash_32, boost::hash2::xxhash_64>));
        XSTD_CONSTEXPR_CHECK((not algorithm_pair<boost::hash2::siphash_32, boost::hash2::siphash_64>));
        XSTD_CONSTEXPR_CHECK((not algorithm_pair<boost::hash2::xxhash_32, boost::hash2::xxhash_64, std::string>));
}

// FNV-1a carries no bit of a byte down to the bits below it, so masking its 64-bit hash erases the tag.
BOOST_AUTO_TEST_CASE(TaggedIdsShareABucketUnderFnv1aWhereATableMasksTheirHash)
{
        auto const [bucket_count, fnv1a_buckets]    = bucket_spread<xstd::hash<std::uint64_t, boost::hash2::fnv1a_64>>();
        auto const [default_count, default_buckets] = bucket_spread<xstd::hash<std::uint64_t>>();
        BOOST_CHECK_EQUAL(default_count, bucket_count);
#if defined(_LIBCPP_VERSION) || defined(_MSVC_STL_VERSION)
        BOOST_CHECK(std::has_single_bit(bucket_count));
#endif
#ifdef __GLIBCXX__
        BOOST_CHECK(not std::has_single_bit(bucket_count));
#endif
        // A prime count, as libstdc++ rounds up to, or the multiply-high fold to a 32-bit size_t mixes the tag back in.
        if constexpr (std::numeric_limits<std::size_t>::digits >= 64) {
                if (std::has_single_bit(bucket_count)) {
                        BOOST_CHECK_EQUAL(fnv1a_buckets, 1U);
                } else {
                        BOOST_CHECK_GT(fnv1a_buckets, 1U);
                }
        } else {
                BOOST_CHECK_GT(fnv1a_buckets, 1U);
        }
        BOOST_CHECK_GT(default_buckets, bucket_count / 2);
}

// The 64-bit digest itself where std::size_t holds it, and get_integral_result's multiply-high fold where it is 32.
BOOST_AUTO_TEST_CASE(IsOneValuePerWidthOfSizeT)
{
        if constexpr (std::endian::native == std::endian::little) {
                auto const integer = xstd::hash<std::uint64_t>(seed);
                auto const string  = xstd::hash<std::string>(seed);
                auto const at_size = [](std::uint64_t digest, std::uint32_t fold) -> std::size_t {
                        if constexpr (std::numeric_limits<std::size_t>::digits >= 64) {
                                return static_cast<std::size_t>(digest);
                        } else {
                                return fold;
                        }
                };
                BOOST_CHECK_EQUAL(integer(std::uint64_t{42}), at_size(0xf92a'4389'bd62'ee57, 0xb9d9'e2fa));
                BOOST_CHECK_EQUAL(string("r1/p1/..."), at_size(0xd7e9'ed42'f1d4'b1ce, 0xab90'41e8));
        } else {
                BOOST_CHECK(true); // silence Boost.Test's "test case did not check any assertions"
        }
}

// Boost.Unordered reads the algorithm's answer through the hash, and skips its post-mix only where it is true.
BOOST_AUTO_TEST_CASE_TEMPLATE(ReportsWhetherItsAlgorithmAvalanches, H, test::algorithms)
{
        XSTD_CONSTEXPR_CHECK((xstd::hash<int, H>::is_avalanching::value == xstd::hash_algorithm_is_avalanching_v<H>));
#ifdef TEST_HAS_BOOST_UNORDERED
        XSTD_CONSTEXPR_CHECK((boost::hash_is_avalanching<xstd::hash<int, H>>::value == xstd::hash_algorithm_is_avalanching_v<H>));
#endif
}

// The second parameter is the algorithm: a second key type, as a pair of keys reads, is the slip to expect.
BOOST_AUTO_TEST_CASE(TheAlgorithmIsAHashAlgorithm)
{
        XSTD_CONSTEXPR_CHECK(algorithm_argument<boost::hash2::xxhash_64>);
        XSTD_CONSTEXPR_CHECK(not algorithm_argument<std::hash<int>>);
        XSTD_CONSTEXPR_CHECK(not algorithm_argument<std::string>);
        XSTD_CONSTEXPR_CHECK(not algorithm_argument<xstd::hash<int>>);
}

// No integer converts to a seed by accident.
BOOST_AUTO_TEST_CASE(SeedsAreExplicit)
{
        XSTD_CONSTEXPR_CHECK((std::is_constructible_v<xstd::hash<int>, std::uint64_t>));
        XSTD_CONSTEXPR_CHECK((not std::is_convertible_v<std::uint64_t, xstd::hash<int>>));
        XSTD_CONSTEXPR_CHECK((std::is_constructible_v<xstd::hash<int>, unsigned char const*, std::size_t>));
}

// [unord.hash]/3: noexcept as std::hash is for a standard key under Hash2's algorithms, an optional as its value's is.
BOOST_AUTO_TEST_CASE(TheCallIsNoexceptWhereStdHashsIs)
{
        XSTD_CONSTEXPR_CHECK(noexcept(std::declval<xstd::hash<int> const&>()(0)));
        XSTD_CONSTEXPR_CHECK(noexcept(std::declval<xstd::hash<std::string> const&>()(std::declval<std::string const&>())));
        XSTD_CONSTEXPR_CHECK(noexcept(std::declval<xstd::hash<std::bitset<8>, boost::hash2::siphash_64> const&>()(std::bitset<8>())));
        XSTD_CONSTEXPR_CHECK(noexcept(std::declval<xstd::hash<std::filesystem::path> const&>()(std::filesystem::path())));
        XSTD_CONSTEXPR_CHECK(noexcept(std::declval<xstd::hash<std::optional<int>> const&>()(std::declval<std::optional<int> const&>())));
}

// A key's own hash_append may throw, and a noexcept call would turn that into std::terminate.
BOOST_AUTO_TEST_CASE(TheCallIsNotNoexceptWhereStdHashsIsNot)
{
        XSTD_CONSTEXPR_CHECK(not noexcept(std::declval<xstd::hash<std::vector<int>> const&>()(std::vector<int>())));
        XSTD_CONSTEXPR_CHECK(not noexcept(std::declval<xstd::hash<std::pair<int, int>> const&>()(std::pair<int, int>())));
        XSTD_CONSTEXPR_CHECK(not noexcept(std::declval<xstd::hash<std::unique_ptr<int>> const&>()(std::unique_ptr<int>())));
        XSTD_CONSTEXPR_CHECK(not noexcept(std::declval<xstd::hash<std::chrono::seconds> const&>()(std::chrono::seconds())));
}

// Each key type hashes as Hash2 appends it, under the seeded algorithm.
BOOST_AUTO_TEST_CASE_TEMPLATE(HashesItsKeyAsHash2Does, H, test::algorithms)
{
        auto const word    = std::string("word");
        auto const numbers = std::vector<int>{1, 2, 3};
        auto const entry   = std::pair<int, std::string>(42, "word");

        BOOST_CHECK_EQUAL((xstd::hash<int, H>(seed)(42)), hash2_of(H(seed), 42));
        BOOST_CHECK_EQUAL((xstd::hash<double, H>(seed)(2.5)), hash2_of(H(seed), 2.5));
        BOOST_CHECK_EQUAL((xstd::hash<std::string, H>(seed)(word)), hash2_of(H(seed), word));
        BOOST_CHECK_EQUAL((xstd::hash<std::vector<int>, H>(seed)(numbers)), hash2_of(H(seed), numbers));
        BOOST_CHECK_EQUAL((xstd::hash<std::pair<int, std::string>, H>(seed)(entry)), hash2_of(H(seed), entry));
}

// Hash2 declares hash_append void for every key, so a constraint on the call admits std::weak_ptr, which it rejects.
BOOST_AUTO_TEST_CASE(HashAppendIsDeclaredForKeysHash2CannotHash)
{
        XSTD_CONSTEXPR_CHECK((std::same_as<decltype(boost::hash2::hash_append(std::declval<boost::hash2::xxhash_64&>(), {}, std::declval<std::weak_ptr<int> const&>())), void>));
}

// 42 converts to the key's type first, so it hashes as the std::int64_t{42} a table holds, not as four bytes.
BOOST_AUTO_TEST_CASE(ConvertsEachArgumentToItsKeyType)
{
        auto const h = xstd::hash<std::int64_t>();

        BOOST_CHECK_EQUAL(h(42), h(std::int64_t{42}));
        BOOST_CHECK_EQUAL(h(42), hash2_of(boost::hash2::xxhash_64(), std::int64_t{42}));
        BOOST_CHECK_NE(h(42), hash2_of(boost::hash2::xxhash_64(), 42));
}

// Only a string key is transparent: elsewhere equal values of other types need not append the same bytes.
BOOST_AUTO_TEST_CASE(IsTransparentOverStringsOnly)
{
        XSTD_CONSTEXPR_CHECK(transparent<std::equal_to<>>);
        XSTD_CONSTEXPR_CHECK(transparent<xstd::hash<std::string>>);
        XSTD_CONSTEXPR_CHECK(not transparent<xstd::hash<std::string_view>>);
        XSTD_CONSTEXPR_CHECK(not transparent<xstd::hash<std::int64_t>>);
}

// A string, its view and a C string append the same characters, under every algorithm.
BOOST_AUTO_TEST_CASE_TEMPLATE(AStringHashesAsItsViewAndItsCString, H, test::algorithms)
{
        auto const h      = xstd::hash<std::string, H>(seed);
        auto const string = std::string("piece");

        BOOST_CHECK_EQUAL(h(std::string_view(string)), h(string));
        BOOST_CHECK_EQUAL(h("piece"), h(string));
        BOOST_CHECK_EQUAL(h(string), hash2_of(H(seed), string));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(EqualValuesHashEqual, H, test::algorithms)
{
        auto const literal = std::string("the quick brown fox");
        auto built         = std::string("the quick");
        built += " brown fox";

        for (auto const& h : {xstd::hash<std::string, H>(), xstd::hash<std::string, H>(seed), xstd::hash<std::string, H>(key.data(), key.size())}) {
                BOOST_CHECK_EQUAL(h(literal), h(built));
        }
}

// The prototype is copied, not consumed: a second call, or a copy of the hash, starts where the first did.
BOOST_AUTO_TEST_CASE_TEMPLATE(EachCallStartsFromThePrototype, H, test::algorithms)
{
        auto const h    = xstd::hash<int, H>(seed);
        auto const copy = h;

        BOOST_CHECK_EQUAL(h(42), h(42));
        BOOST_CHECK_EQUAL(copy(42), h(42));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(SeedsChangeTheResult, H, test::algorithms)
{
        auto const unseeded_hash = xstd::hash<int, H>();
        auto const seeded_hash   = xstd::hash<int, H>(seed);
        auto const reseeded_hash = xstd::hash<int, H>(other_seed);
        auto const keyed_hash    = xstd::hash<int, H>(key.data(), key.size());

        auto const unseeded = unseeded_hash(42);
        auto const seeded   = seeded_hash(42);
        auto const keyed    = keyed_hash(42);

        BOOST_CHECK_NE(seeded, unseeded);
        BOOST_CHECK_NE(seeded, reseeded_hash(42));
        BOOST_CHECK_NE(keyed, unseeded);
        BOOST_CHECK_NE(keyed, seeded);
}

// Each constructor is a constant expression; the call is not, as Hash2 1.92 has no constexpr get_integral_result.
BOOST_AUTO_TEST_CASE_TEMPLATE(IsConstructibleInConstantExpressions, H, test::constexpr_algorithms)
{
        [[maybe_unused]] constexpr auto unseeded = xstd::hash<int, H>();
        [[maybe_unused]] constexpr auto seeded   = xstd::hash<int, H>(seed);
        [[maybe_unused]] constexpr auto keyed    = xstd::hash<int, H>(key.data(), key.size());

        BOOST_CHECK(true); // silence Boost.Test's "test case did not check any assertions"
}

// Hash2's advice for keys an adversary chooses: SipHash, seeded per container.
BOOST_AUTO_TEST_CASE(SeedsAStdUnorderedSet)
{
        using siphasher = xstd::hash<std::string, boost::hash2::siphash_64>;

        auto s = std::unordered_set<std::string, siphasher>(0, siphasher(seed));
        s.insert({"one", "two", "three"});

        auto const two      = std::string("two");
        auto const used     = s.hash_function();
        auto const seeded   = siphasher(seed);
        auto const unseeded = siphasher();

        BOOST_CHECK_EQUAL(s.size(), std::size_t{3});
        BOOST_CHECK(s.contains("two"));
        BOOST_CHECK(not s.contains("four"));
        BOOST_CHECK_EQUAL(used(two), seeded(two));
        BOOST_CHECK_NE(used(two), unseeded(two));
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

// -0.0 equals 0.0, and x87's long double holds six bytes of padding that a hash of its bytes would read.
BOOST_AUTO_TEST_CASE(HashesEveryFloatingPointType)
{
        check_equal_and_apart({0.0L, -0.0L}, 1.0L);
#ifdef __STDCPP_FLOAT16_T__
        check_equal_and_apart({std::float16_t{0.0F}, std::float16_t{-0.0F}}, std::float16_t{1.0F});
#endif
#ifdef __STDCPP_FLOAT128_T__
        check_equal_and_apart({std::float128_t{0.0}, std::float128_t{-0.0}}, std::float128_t{1.0});
#endif
}

#ifdef __SIZEOF_INT128__
// Hash2 rejects a 128-bit integer where is_integral admits it, as libc++ and -std=gnu++ do, and where it does not.
BOOST_AUTO_TEST_CASE(HashesA128BitInteger)
{
        __extension__ using uint128 = unsigned __int128;
        check_equal_and_apart({uint128{1} << 70U, uint128{1} << 70U}, uint128{1} << 71U);
        XSTD_CONSTEXPR_CHECK(noexcept(std::declval<xstd::hash<uint128> const&>()(uint128{})));
}
#endif

BOOST_AUTO_TEST_CASE(HashesABitset)
{
        check_equal_and_apart({std::bitset<100>("1"), std::bitset<100>(1)}, std::bitset<100>("10"));
        check_equal_and_apart({std::bitset<100>().set(99), std::bitset<100>().set(99)}, std::bitset<100>().set(98));
        BOOST_CHECK_EQUAL(xstd::hash<std::bitset<0>>()(std::bitset<0>()), xstd::hash<std::bitset<0>>()(std::bitset<0>()));
}

// An error compares by its category and value, so the same value in another category is another error.
BOOST_AUTO_TEST_CASE(HashesAnErrorByItsCategoryAndValue)
{
        check_equal_and_apart({std::error_code(5, std::generic_category()), std::error_code(5, std::generic_category())}, std::error_code(5, std::system_category()));
        check_equal_and_apart({std::error_condition(5, std::generic_category()), std::error_condition(5, std::generic_category())}, std::error_condition(6, std::generic_category()));
}

// [util.smartptr.hash]: a smart pointer hashes as the pointer it holds.
BOOST_AUTO_TEST_CASE(HashesASmartPointerAsItsPointer)
{
        auto const unique = std::make_unique<int>(42);
        auto const shared = std::make_shared<int>(42);

        BOOST_CHECK_EQUAL(xstd::hash<std::unique_ptr<int>>()(unique), xstd::hash<int*>()(unique.get()));
        BOOST_CHECK_EQUAL(xstd::hash<std::shared_ptr<int>>()(shared), xstd::hash<int*>()(shared.get()));
        BOOST_CHECK_NE(xstd::hash<std::shared_ptr<int>>()(shared), xstd::hash<std::shared_ptr<int>>()(nullptr));
}

BOOST_AUTO_TEST_CASE(HashesTheIdentityOfATypeAThreadAndACoroutine)
{
        check_equal_and_apart({std::type_index(typeid(int)), std::type_index(typeid(int const))}, std::type_index(typeid(long)));
        check_equal_and_apart({std::this_thread::get_id(), std::this_thread::get_id()}, std::thread::id());
        check_equal_and_apart({std::coroutine_handle<>(std::noop_coroutine()), std::coroutine_handle<>(std::noop_coroutine())}, std::coroutine_handle<>());
}

// A path compares by its elements, so a doubled separator is the same path and a trailing one another.
BOOST_AUTO_TEST_CASE(HashesAPathByItsElements)
{
        check_equal_and_apart({std::filesystem::path("a//b"), std::filesystem::path("a/b")}, std::filesystem::path("a/b/"));
        check_equal_and_apart({std::filesystem::path("/a"), std::filesystem::path("/a")}, std::filesystem::path("a"));
}

// Sunday is both weekday 0 and weekday 7, which compare equal.
BOOST_AUTO_TEST_CASE(HashesTimesAndDates)
{
        using namespace std::chrono;
        check_equal_and_apart({seconds(60), seconds(60)}, seconds(61));
        check_equal_and_apart({sys_seconds(seconds(60)), sys_seconds(seconds(60))}, sys_seconds(seconds(61)));
        check_equal_and_apart({weekday(0), weekday(7)}, weekday(1));
        check_equal_and_apart({year_month_day(2026y / 10 / 23), year_month_day(2026y / October / 23d)}, year_month_day(2026y / 10 / 24));
        check_equal_and_apart({2026y / October / Friday[last], 2026y / October / Friday[last]}, 2026y / October / Sunday[last]);
}

#ifdef __cpp_lib_stacktrace
BOOST_AUTO_TEST_CASE(HashesAStacktraceEntry)
{
        BOOST_CHECK_EQUAL(xstd::hash<std::stacktrace_entry>()(std::stacktrace_entry()), xstd::hash<std::stacktrace_entry>()(std::stacktrace_entry()));
}
#endif

#ifdef __cpp_lib_text_encoding
// A registered encoding compares by its MIB, whatever alias names it, and an unregistered one by its name.
BOOST_AUTO_TEST_CASE(HashesATextEncoding)
{
        check_equal_and_apart({std::text_encoding("UTF-8"), std::text_encoding("utf8")}, std::text_encoding("UTF-16"));
        check_equal_and_apart({std::text_encoding("x-my-charset"), std::text_encoding("XMYCHARSET")}, std::text_encoding("x-your-charset"));
}
#endif

// [optional.hash]: an engaged optional hashes as its value, and an empty one apart from every engaged one.
BOOST_AUTO_TEST_CASE(HashesAnEngagedOptionalAsItsValue)
{
        auto const h = xstd::hash<std::optional<int>>(seed);

        BOOST_CHECK_EQUAL(h(42), xstd::hash<int>(seed)(42));
        BOOST_CHECK_EQUAL((xstd::hash<std::optional<int const>>(seed)(42)), xstd::hash<int>(seed)(42));
        BOOST_CHECK_NE(h(std::nullopt), h(0));
}

#ifdef __cpp_lib_indirect
// [indirect.hash]: an indirect hashes as its value.
BOOST_AUTO_TEST_CASE(HashesAnIndirectAsItsValue)
{
        BOOST_CHECK_EQUAL(xstd::hash<std::indirect<int>>(seed)(std::indirect<int>(42)), xstd::hash<int>(seed)(42));
}
#endif

// Inside another key, an empty optional still appends a marker, so swapping two elements changes the hash.
BOOST_AUTO_TEST_CASE(HashesStandardKeysInsideOtherKeys)
{
        using bits = std::optional<std::bitset<8>>;
        check_equal_and_apart({std::vector<bits>{std::bitset<8>(1), std::nullopt}, std::vector<bits>{std::bitset<8>(1), std::nullopt}}, std::vector<bits>{std::nullopt, std::bitset<8>(1)});
        check_equal_and_apart({std::pair(std::error_code(), std::chrono::seconds(1)), std::pair(std::error_code(), std::chrono::seconds(1))}, std::pair(std::error_code(), std::chrono::seconds(2)));
}

#ifdef TEST_HAS_BOOST_UNORDERED

BOOST_AUTO_TEST_CASE(SeedsABoostUnorderedFlatSet)
{
        using siphasher = xstd::hash<std::string, boost::hash2::siphash_64>;

        auto s = boost::unordered_flat_set<std::string, siphasher>(0, siphasher(seed));
        s.insert({"one", "two", "three"});

        auto const two      = std::string("two");
        auto const used     = s.hash_function();
        auto const seeded   = siphasher(seed);
        auto const unseeded = siphasher();

        BOOST_CHECK_EQUAL(s.size(), std::size_t{3});
        BOOST_CHECK(s.contains("two"));
        BOOST_CHECK(not s.contains("four"));
        BOOST_CHECK_EQUAL(used(two), seeded(two));
        BOOST_CHECK_NE(used(two), unseeded(two));
}

// Boost's open-addressing tables take a group from a hash's high bits, which FNV-1a leaves alike, and mix it first.
BOOST_AUTO_TEST_CASE(TaggedIdsSpreadUnderFnv1aWhereABoostUnorderedFlatSetMixesTheirHash)
{
        using fnv1a = xstd::hash<std::uint64_t, boost::hash2::fnv1a_64>;

        auto const hash = fnv1a();
        if constexpr (std::numeric_limits<std::size_t>::digits >= 64) {
                auto const top_byte = [&](std::uint64_t id) -> std::size_t { return hash(id) >> ((sizeof(std::size_t) - 1U) * 8U); };
                for (auto const id : tagged_ids) {
                        BOOST_CHECK_EQUAL(top_byte(id), top_byte(tagged_ids[0]));
                }
        }
        BOOST_CHECK(not boost::hash_is_avalanching<fnv1a>::value);

        auto table = boost::unordered_flat_set<std::uint64_t, fnv1a>();
        table.reserve(1024);
        table.insert(tagged_ids.begin(), tagged_ids.end());
        auto const average_with_one_spill = 1.0 + (1.0 / static_cast<double>(tagged_ids.size()));
        BOOST_CHECK_LT(table.get_stats().insertion.probe_length.average, average_with_one_spill);
}

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
