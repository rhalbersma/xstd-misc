//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_FUNCTIONAL_HASH_HPP
#define XSTD_MISC_FUNCTIONAL_HASH_HPP

#include <xstd/misc/concepts/hash_algorithm.hpp>        // hash_algorithm
#include <xstd/misc/functional/detail/hasher.hpp>       // hasher, is_nothrow_hash_v
#include <xstd/misc/functional/detail/value_hasher.hpp> // value_hasher
#include <boost/hash2/flavor.hpp>                       // default_flavor
#include <boost/hash2/xxhash.hpp>                       // xxhash_64
#include <concepts>                                     // convertible_to
#include <cstddef>                                      // size_t
#include <memory>                                       // indirect
#include <optional>                                     // optional
#include <string>                                       // basic_string
#include <string_view>                                  // basic_string_view
#include <type_traits>                                  // is_nothrow_convertible_v, remove_const_t

namespace xstd {

// xxHash passes SMHasher3's Avalanche test at 3 to 128 bytes; FNV-1a's last byte's high bits never reach the low bits.
template<class T, hash_algorithm H64 = boost::hash2::xxhash_64, hash_algorithm H32 = H64, class Flavor = boost::hash2::default_flavor>
        requires (sizeof(typename H32::result_type) <= sizeof(typename H64::result_type)) // A pair given in the wrong order would run the wider algorithm where std::size_t is narrower.
class hash : detail::hasher<detail::native_hash_algorithm_t<H64, H32>, Flavor>
{
public:
        using hash_type = detail::native_hash_algorithm_t<H64, H32>;

        using detail::hasher<hash_type, Flavor>::hasher;
        // Declared rather than using-declared: MSVC then denies access to its members through the private base.
        // NOLINTNEXTLINE(readability-redundant-typename): clang-cl's -Wc++20-compat rejects the bare dependent name.
        using is_avalanching = typename detail::hasher<hash_type, Flavor>::is_avalanching;

        [[nodiscard]] constexpr auto operator()(T const& v) const noexcept(detail::is_nothrow_hash_v<hash_type, T>)
                -> std::size_t
        {
                return detail::hasher<hash_type, Flavor>::operator()(v);
        }
};

// A string, its view and a C string append the same characters, so a lookup by any of them finds the key.
template<class CharT, class Traits, class Allocator, hash_algorithm H64, hash_algorithm H32, class Flavor>
class hash<std::basic_string<CharT, Traits, Allocator>, H64, H32, Flavor> : detail::hasher<detail::native_hash_algorithm_t<H64, H32>, Flavor>
{
public:
        using hash_type = detail::native_hash_algorithm_t<H64, H32>;

        using detail::hasher<hash_type, Flavor>::hasher;
        // Declared rather than using-declared: MSVC then denies access to its members through the private base.
        // NOLINTNEXTLINE(readability-redundant-typename): clang-cl's -Wc++20-compat rejects the bare dependent name.
        using is_avalanching = typename detail::hasher<hash_type, Flavor>::is_avalanching;
        using is_transparent = void;

        // What converts to the key alone, as std::hash's parameter accepts, would need a second conversion to reach a view.
        [[nodiscard]] constexpr auto operator()(std::basic_string<CharT, Traits, Allocator> const& v) const noexcept(detail::is_nothrow_hash_v<hash_type, std::basic_string_view<CharT, Traits>>)
                -> std::size_t
        {
                return detail::hasher<hash_type, Flavor>::operator()(std::basic_string_view<CharT, Traits>(v));
        }

        template<class K>
                requires std::convertible_to<K const&, std::basic_string_view<CharT, Traits>>
        [[nodiscard]] constexpr auto operator()(K const& v) const noexcept(detail::is_nothrow_hash_v<hash_type, std::basic_string_view<CharT, Traits>> && std::is_nothrow_convertible_v<K const&, std::basic_string_view<CharT, Traits>>)
                -> std::size_t
        {
                return detail::hasher<hash_type, Flavor>::operator()(std::basic_string_view<CharT, Traits>(v));
        }
};

// [optional.hash]: an engaged optional hashes as its value does, and an empty one as no bytes, which no value appends.
template<class T, hash_algorithm H64, hash_algorithm H32, class Flavor>
        requires (sizeof(typename H32::result_type) <= sizeof(typename H64::result_type))
class hash<std::optional<T>, H64, H32, Flavor> : detail::value_hasher<hash<std::remove_const_t<T>, H64, H32, Flavor>, detail::native_hash_algorithm_t<H64, H32>, Flavor>
{
        using value_hasher = detail::value_hasher<hash<std::remove_const_t<T>, H64, H32, Flavor>, detail::native_hash_algorithm_t<H64, H32>, Flavor>;

public:
        using hash_type = detail::native_hash_algorithm_t<H64, H32>;

        using value_hasher::value_hasher;
        // Declared rather than using-declared: MSVC then denies access to its members through the private base.
        // NOLINTNEXTLINE(readability-redundant-typename): clang-cl's -Wc++20-compat rejects the bare dependent name.
        using is_avalanching = typename value_hasher::is_avalanching;

        [[nodiscard]] constexpr auto operator()(std::optional<T> const& v) const noexcept(value_hasher::template is_nothrow<std::remove_const_t<T>>)
                -> std::size_t
        {
                return value_hasher::operator()(v ? std::addressof(static_cast<std::remove_const_t<T> const&>(*v)) : nullptr);
        }
};

#if defined(__cpp_lib_indirect)
// [indirect.hash]: an indirect hashes as its value does, and a valueless one as no bytes, which no value appends.
template<class T, class Allocator, hash_algorithm H64, hash_algorithm H32, class Flavor>
        requires (sizeof(typename H32::result_type) <= sizeof(typename H64::result_type))
class hash<std::indirect<T, Allocator>, H64, H32, Flavor> : detail::value_hasher<hash<T, H64, H32, Flavor>, detail::native_hash_algorithm_t<H64, H32>, Flavor>
{
        using value_hasher = detail::value_hasher<hash<T, H64, H32, Flavor>, detail::native_hash_algorithm_t<H64, H32>, Flavor>;

public:
        using hash_type = detail::native_hash_algorithm_t<H64, H32>;

        using value_hasher::value_hasher;
        // Declared rather than using-declared: MSVC then denies access to its members through the private base.
        // NOLINTNEXTLINE(readability-redundant-typename): clang-cl's -Wc++20-compat rejects the bare dependent name.
        using is_avalanching = typename value_hasher::is_avalanching;

        [[nodiscard]] constexpr auto operator()(std::indirect<T, Allocator> const& v) const noexcept(value_hasher::template is_nothrow<T>)
                -> std::size_t
        {
                return value_hasher::operator()(v.valueless_after_move() ? nullptr : std::addressof(*v));
        }
};
#endif

} // namespace xstd

#endif // XSTD_MISC_FUNCTIONAL_HASH_HPP
