//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_FUNCTIONAL_HASHER_HPP
#define XSTD_MISC_FUNCTIONAL_HASHER_HPP

#include <xstd/misc/concepts/hash_algorithm.hpp>                   // hash_algorithm
#include <xstd/misc/type_traits/hash_algorithm_is_avalanching.hpp> // hash_algorithm_is_avalanching_v
#include <boost/hash2/get_integral_result.hpp>                     // get_integral_result
#include <boost/hash2/hash_append.hpp>                             // hash_append
#include <boost/hash2/xxhash.hpp>                                  // xxhash_64
#include <cstddef>                                                 // size_t
#include <cstdint>                                                 // uint64_t
#include <type_traits>                                             // bool_constant

namespace xstd {

// xxHash avalanches at any key length; FNV-1a never lets the last byte's high bits reach the low result bits.
template<hash_algorithm H = boost::hash2::xxhash_64>
class hasher
{
        H m_prototype{};

        template<class T>
        [[nodiscard]] constexpr auto appended(T const& v) const
                -> H
        {
                auto h = m_prototype;
                boost::hash2::hash_append(h, {}, v);
                return h;
        }

public:
        // NOLINTNEXTLINE(readability-redundant-typename): clang-cl's -Wc++20-compat rejects the bare dependent name.
        using digest_type = typename H::result_type;

        // Read by boost::hash_is_avalanching: where true, Boost.Unordered's open-addressing tables skip their post-mix.
        using is_avalanching = std::bool_constant<hash_algorithm_is_avalanching_v<H>>;

        // No is_transparent: equal values of different types need not append the same bytes, nor hash equal.

        [[nodiscard]] hasher() = default;

        // MSVC checks C4459 as it instantiates the template, so any namespace-scope seed a consumer declares trips it.
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4459)
#endif
        [[nodiscard]] constexpr explicit hasher(std::uint64_t seed)
                : m_prototype(seed)
        {}
#ifdef _MSC_VER
#pragma warning(pop)
#endif

        [[nodiscard]] constexpr hasher(unsigned char const* p, std::size_t n)
                : m_prototype(p, n)
        {}

        // Neither constrained nor noexcept: Hash2 declares hash_append void for every T, and it runs the user's code.
        template<class T>
        [[nodiscard]] constexpr auto operator()(T const& v) const
                -> std::size_t
        {
                auto h = appended(v);
                return boost::hash2::get_integral_result<std::size_t>(h);
        }

        // The algorithm's whole result, as wide on every target, where the call operator folds it to std::size_t.
        template<class T>
        [[nodiscard]] constexpr auto digest(T const& v) const
                -> digest_type
        {
                return appended(v).result();
        }
};

} // namespace xstd

#endif // XSTD_MISC_FUNCTIONAL_HASHER_HPP
