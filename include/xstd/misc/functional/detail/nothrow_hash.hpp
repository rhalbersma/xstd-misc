//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_FUNCTIONAL_DETAIL_NOTHROW_HASH_HPP
#define XSTD_MISC_FUNCTIONAL_DETAIL_NOTHROW_HASH_HPP

#include <xstd/misc/functional/detail/standard_keys.hpp>         // int128, uint128
#include <xstd/misc/type_traits/detail/hash2_algorithms_fwd.hpp> // blake2b_512, blake2s_256, hmac, md5_128, murmur3_128, murmur3_32, ripemd_128, ripemd_160, sha1_160, sha2_224, sha2_256, sha2_384, sha2_512, sha2_512_224, sha2_512_256, sha3_224, sha3_256, sha3_384, sha3_512, shake_128, shake_256, siphash_32, siphash_64, spooky2_128, xxh3_128, xxhash_32, xxhash_64
#include <bitset>                                                // bitset
#include <chrono>                                                // day, leap_second, month, weekday, year, year_month_day, ...
#include <coroutine>                                             // coroutine_handle
#include <cstddef>                                               // nullptr_t, size_t
#include <filesystem>                                            // path
#include <memory>                                                // shared_ptr
#include <string>                                                // basic_string, char_traits
#include <string_view>                                           // basic_string_view
#include <system_error>                                          // error_code, error_condition
#include <thread>                                                // thread
#include <type_traits>                                           // bool_constant, false_type, is_arithmetic_v, is_enum_v, is_pointer_v, is_same_v, true_type
#include <typeindex>                                             // type_index
#include <variant>                                               // monostate
#include <vector>                                                // vector

#if __has_include(<stacktrace>)
#include <stacktrace> // basic_stacktrace, stacktrace_entry
#endif

#if __has_include(<text_encoding>)
#include <text_encoding> // text_encoding
#endif

namespace xstd::detail {

// Hash2's own algorithms, none of which throws from its constructors, update or result, though none says noexcept.
template<class H>
struct is_hash2_algorithm : std::false_type
{};

template<class H>
struct is_hash2_algorithm<boost::hash2::hmac<H>> : is_hash2_algorithm<H>
{};

template<class H>
inline constexpr auto is_hash2_algorithm_v = is_hash2_algorithm<H>::value;

#define XSTD_MISC_DETAIL_HASH2_ALGORITHM(H) \
        template<> \
        struct is_hash2_algorithm<boost::hash2::H> : std::true_type \
        {}

XSTD_MISC_DETAIL_HASH2_ALGORITHM(blake2b_512);
XSTD_MISC_DETAIL_HASH2_ALGORITHM(blake2s_256);
XSTD_MISC_DETAIL_HASH2_ALGORITHM(fnv1a_32);
XSTD_MISC_DETAIL_HASH2_ALGORITHM(fnv1a_64);
XSTD_MISC_DETAIL_HASH2_ALGORITHM(md5_128);
XSTD_MISC_DETAIL_HASH2_ALGORITHM(murmur3_128);
XSTD_MISC_DETAIL_HASH2_ALGORITHM(murmur3_32);
XSTD_MISC_DETAIL_HASH2_ALGORITHM(ripemd_128);
XSTD_MISC_DETAIL_HASH2_ALGORITHM(ripemd_160);
XSTD_MISC_DETAIL_HASH2_ALGORITHM(sha1_160);
XSTD_MISC_DETAIL_HASH2_ALGORITHM(sha2_224);
XSTD_MISC_DETAIL_HASH2_ALGORITHM(sha2_256);
XSTD_MISC_DETAIL_HASH2_ALGORITHM(sha2_384);
XSTD_MISC_DETAIL_HASH2_ALGORITHM(sha2_512);
XSTD_MISC_DETAIL_HASH2_ALGORITHM(sha2_512_224);
XSTD_MISC_DETAIL_HASH2_ALGORITHM(sha2_512_256);
XSTD_MISC_DETAIL_HASH2_ALGORITHM(sha3_224);
XSTD_MISC_DETAIL_HASH2_ALGORITHM(sha3_256);
XSTD_MISC_DETAIL_HASH2_ALGORITHM(sha3_384);
XSTD_MISC_DETAIL_HASH2_ALGORITHM(sha3_512);
XSTD_MISC_DETAIL_HASH2_ALGORITHM(shake_128);
XSTD_MISC_DETAIL_HASH2_ALGORITHM(shake_256);
XSTD_MISC_DETAIL_HASH2_ALGORITHM(siphash_32);
XSTD_MISC_DETAIL_HASH2_ALGORITHM(siphash_64);
XSTD_MISC_DETAIL_HASH2_ALGORITHM(spooky2_128);
XSTD_MISC_DETAIL_HASH2_ALGORITHM(xxh3_128);
XSTD_MISC_DETAIL_HASH2_ALGORITHM(xxhash_32);
XSTD_MISC_DETAIL_HASH2_ALGORITHM(xxhash_64);

#undef XSTD_MISC_DETAIL_HASH2_ALGORITHM

// [unord.hash]/3: the keys whose std::hash is noexcept, which leaves out those whose clause says otherwise.
template<class T>
struct is_nothrow_key : std::bool_constant<std::is_arithmetic_v<T> || std::is_enum_v<T> || std::is_pointer_v<T> || std::is_same_v<T, std::nullptr_t>>
{};

template<class T>
inline constexpr auto is_nothrow_key_v = is_nothrow_key<T>::value;

template<class CharT, class Allocator>
struct is_nothrow_key<std::basic_string<CharT, std::char_traits<CharT>, Allocator>> : is_nothrow_key<CharT>
{};

template<class CharT>
struct is_nothrow_key<std::basic_string_view<CharT, std::char_traits<CharT>>> : is_nothrow_key<CharT>
{};

template<class Allocator>
struct is_nothrow_key<std::vector<bool, Allocator>> : std::true_type
{};

template<std::size_t N>
struct is_nothrow_key<std::bitset<N>> : std::true_type
{};

template<class T>
struct is_nothrow_key<std::shared_ptr<T>> : std::true_type
{};

template<class Promise>
struct is_nothrow_key<std::coroutine_handle<Promise>> : std::true_type
{};

#define XSTD_MISC_DETAIL_NOTHROW_KEY(...) \
        template<> \
        struct is_nothrow_key<__VA_ARGS__> : std::true_type \
        {}

#if defined(__SIZEOF_INT128__)
XSTD_MISC_DETAIL_NOTHROW_KEY(int128);
XSTD_MISC_DETAIL_NOTHROW_KEY(uint128);
#endif
XSTD_MISC_DETAIL_NOTHROW_KEY(std::monostate);
XSTD_MISC_DETAIL_NOTHROW_KEY(std::error_code);
XSTD_MISC_DETAIL_NOTHROW_KEY(std::error_condition);
XSTD_MISC_DETAIL_NOTHROW_KEY(std::type_index);
XSTD_MISC_DETAIL_NOTHROW_KEY(std::thread::id);
XSTD_MISC_DETAIL_NOTHROW_KEY(std::filesystem::path);
XSTD_MISC_DETAIL_NOTHROW_KEY(std::chrono::day);
XSTD_MISC_DETAIL_NOTHROW_KEY(std::chrono::month);
XSTD_MISC_DETAIL_NOTHROW_KEY(std::chrono::year);
XSTD_MISC_DETAIL_NOTHROW_KEY(std::chrono::weekday);
XSTD_MISC_DETAIL_NOTHROW_KEY(std::chrono::weekday_indexed);
XSTD_MISC_DETAIL_NOTHROW_KEY(std::chrono::weekday_last);
XSTD_MISC_DETAIL_NOTHROW_KEY(std::chrono::month_day);
XSTD_MISC_DETAIL_NOTHROW_KEY(std::chrono::month_day_last);
XSTD_MISC_DETAIL_NOTHROW_KEY(std::chrono::month_weekday);
XSTD_MISC_DETAIL_NOTHROW_KEY(std::chrono::month_weekday_last);
XSTD_MISC_DETAIL_NOTHROW_KEY(std::chrono::year_month);
XSTD_MISC_DETAIL_NOTHROW_KEY(std::chrono::year_month_day);
XSTD_MISC_DETAIL_NOTHROW_KEY(std::chrono::year_month_day_last);
XSTD_MISC_DETAIL_NOTHROW_KEY(std::chrono::year_month_weekday);
XSTD_MISC_DETAIL_NOTHROW_KEY(std::chrono::year_month_weekday_last);
#if defined(XSTD_MISC_DETAIL_HAS_TIME_ZONES)
XSTD_MISC_DETAIL_NOTHROW_KEY(std::chrono::leap_second);
#endif
#if defined(__cpp_lib_stacktrace)
XSTD_MISC_DETAIL_NOTHROW_KEY(std::stacktrace_entry);
#endif
#if defined(__cpp_lib_text_encoding)
XSTD_MISC_DETAIL_NOTHROW_KEY(std::text_encoding);
#endif

#undef XSTD_MISC_DETAIL_NOTHROW_KEY

#if defined(__cpp_lib_stacktrace)
template<class Allocator>
struct is_nothrow_key<std::basic_stacktrace<Allocator>> : std::true_type
{};
#endif

// A key whose std::hash is noexcept, hashed by one of Hash2's algorithms, which nothing in between can make throw.
template<class H, class T>
inline constexpr auto is_nothrow_hash_v = is_hash2_algorithm_v<H> && is_nothrow_key_v<T>;

} // namespace xstd::detail

#endif // XSTD_MISC_FUNCTIONAL_DETAIL_NOTHROW_HASH_HPP
