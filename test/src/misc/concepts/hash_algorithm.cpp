//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/concepts/hash_algorithm.hpp> // hash_algorithm
#include <test/constexpr_check.hpp>              // XSTD_CONSTEXPR_CHECK
#include <test/hash2_algorithms.hpp>             // algorithms
#include <boost/hash2/digest.hpp>                // digest
#include <boost/hash2/fnv1a.hpp>                 // fnv1a_32
#include <boost/hash2/legacy/murmur3.hpp>        // murmur3_128
#include <boost/hash2/md5.hpp>                   // md5_128
#include <boost/test/unit_test.hpp>              // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK
#include <array>                                 // array
#include <concepts>                              // same_as, unsigned_integral
#include <cstddef>                               // size_t
#include <cstdint>                               // uint32_t, uint64_t
#include <span>                                  // span
#include <utility>                               // declval

namespace {

template<class H>
concept has_block_size = requires { H::block_size; };

constexpr auto seed       = std::uint64_t{0x9e37'79b9'7f4a'7c15};
constexpr auto other_seed = std::uint64_t{0xc2b2'ae3d'27d4'eb4f};
constexpr auto key        = std::array<unsigned char, 16>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
constexpr auto other_key  = std::array<unsigned char, 16>{16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};

// Long enough to cross every block size in Hash2, and the 240 bytes past which XXH3 takes its long-input path.
constexpr auto message = []() -> std::array<unsigned char, 300> {
        auto bytes = std::array<unsigned char, 300>{};
        for (auto i = std::size_t{0}; i < bytes.size(); ++i) {
                bytes[i] = static_cast<unsigned char>((i * 31) + 7);
        }
        return bytes;
}();

// Either side of each length at which some algorithm in Hash2 changes how it consumes its input.
constexpr auto lengths = std::array<std::size_t, 17>{0, 1, 3, 4, 8, 9, 16, 17, 63, 64, 65, 127, 128, 129, 240, 241, 300};

// The digest of the message's first n bytes, passed in one call.
template<class H>
auto digest_of(H h, std::size_t n)
{
        h.update(message.data(), n);
        return h.result();
}

} // namespace

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(Concepts)
BOOST_AUTO_TEST_SUITE(HashAlgorithm)

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

// Hash2's documented contract: the way a message is split into update calls does not affect its digest.
BOOST_AUTO_TEST_CASE_TEMPLATE(UpdateIsBlindToHowTheMessageIsSplit, H, test::algorithms)
{
        for (auto const n : lengths) {
                auto const bytes = std::span(message).first(n);
                auto const whole = digest_of(H(seed), n);
                for (auto split = std::size_t{0}; split <= n; ++split) {
                        auto h = H(seed);
                        h.update(bytes.data(), split);
                        h.update(bytes.subspan(split).data(), n - split);
                        BOOST_CHECK(h.result() == whole);
                }
                auto bytewise = H(seed);
                for (auto const& byte : bytes) {
                        bytewise.update(&byte, 1);
                }
                BOOST_CHECK(bytewise.result() == whole);
        }
}

// Hash2's documented contract: a copy is equivalent to the original, which is what a hasher's prototype relies on.
BOOST_AUTO_TEST_CASE_TEMPLATE(ACopyIsEquivalentToTheOriginal, H, test::algorithms)
{
        auto const head = std::span(message).first(100);
        auto const tail = std::span(message).subspan(100);
        auto original   = H(seed);
        original.update(head.data(), head.size());
        auto constructed = original;
        auto assigned    = H(other_seed);
        assigned         = original;

        original.update(tail.data(), tail.size());
        constructed.update(tail.data(), tail.size());
        assigned.update(tail.data(), tail.size());
        auto const digest = original.result();
        BOOST_CHECK(constructed.result() == digest);
        BOOST_CHECK(assigned.result() == digest);
}

// Hash2's documented contract: seed 0 is default construction, and distinct seeds initialize distinct states.
BOOST_AUTO_TEST_CASE_TEMPLATE(AnIntegerSeedInitializesTheStateItNames, H, test::algorithms)
{
        constexpr auto n = message.size();
        BOOST_CHECK(digest_of(H(seed), n) == digest_of(H(seed), n));
        BOOST_CHECK(digest_of(H(std::uint64_t{0}), n) == digest_of(H(), n));
        BOOST_CHECK(digest_of(H(seed), n) != digest_of(H(), n));
        BOOST_CHECK(digest_of(H(seed), n) != digest_of(H(other_seed), n));
}

// Hash2's documented contract: an empty byte seed is default construction, and distinct ones seed distinct states.
BOOST_AUTO_TEST_CASE_TEMPLATE(AByteSeedInitializesTheStateItNames, H, test::algorithms)
{
        constexpr auto n = message.size();
        BOOST_CHECK(digest_of(H(key.data(), key.size()), n) == digest_of(H(key.data(), key.size()), n));
        BOOST_CHECK(digest_of(H(key.data(), 0), n) == digest_of(H(), n));
        BOOST_CHECK(digest_of(H(key.data(), key.size()), n) != digest_of(H(), n));
        BOOST_CHECK(digest_of(H(key.data(), key.size()), n) != digest_of(H(other_key.data(), other_key.size()), n));
}

// Hash2's documented contract: each further result continues a pseudorandom sequence that an equal state repeats.
BOOST_AUTO_TEST_CASE_TEMPLATE(ResultCanBeCalledAgain, H, test::algorithms)
{
        auto h = H(seed);
        h.update(message.data(), message.size());
        auto twin = h;

        auto const first  = h.result();
        auto const second = h.result();
        BOOST_CHECK(second != first);
        BOOST_CHECK(twin.result() == first);
        BOOST_CHECK(twin.result() == second);
}

// get_integral_result folds an array-like result into std::size_t from its first 8 bytes, which each one in Hash2 has.
BOOST_AUTO_TEST_CASE_TEMPLATE(EveryArrayLikeResultHasAtLeastEightBytes, H, test::algorithms)
{
        using result = decltype(std::declval<H&>().result());
        XSTD_CONSTEXPR_CHECK(std::unsigned_integral<result> or sizeof(result) >= 8);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
