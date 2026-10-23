//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_FUNCTIONAL_DETAIL_VALUE_HASHER_HPP
#define XSTD_MISC_FUNCTIONAL_DETAIL_VALUE_HASHER_HPP

#include <xstd/misc/concepts/hash_algorithm.hpp>  // hash_algorithm
#include <xstd/misc/functional/detail/hasher.hpp> // hasher
#include <cstddef>                                // size_t
#include <cstdint>                                // uint64_t
#include <type_traits>                            // bool_constant, false_type, is_constructible_v, is_invocable_v, is_nothrow_copy_constructible_v, is_nothrow_invocable_v

namespace xstd::detail {

template<class ValueHash>
struct value_hash_is_avalanching : std::false_type
{};

template<class ValueHash>
        requires requires { ValueHash::is_avalanching::value; }
struct value_hash_is_avalanching<ValueHash> : std::bool_constant<ValueHash::is_avalanching::value>
{};

// A wrapper hashes what it holds by that value's own hash, and holding nothing as no bytes, from one seed for both.
template<class ValueHash, hash_algorithm H, class Flavor>
class value_hasher : hasher<H, Flavor>
{
        ValueHash m_value_hash{};

public:
        using is_avalanching = value_hash_is_avalanching<ValueHash>;

        [[nodiscard]] value_hasher() = default;

        [[nodiscard]] constexpr explicit value_hasher(std::uint64_t seed)
                requires std::is_constructible_v<ValueHash, std::uint64_t>
                : hasher<H, Flavor>(seed)
                , m_value_hash(seed)
        {}

        [[nodiscard]] constexpr value_hasher(unsigned char const* p, std::size_t n)
                requires std::is_constructible_v<ValueHash, unsigned char const*, std::size_t>
                : hasher<H, Flavor>(p, n)
                , m_value_hash(p, n)
        {}

        template<class Value>
        static constexpr auto is_nothrow = std::is_nothrow_invocable_v<ValueHash&, Value const&> and (std::is_invocable_v<ValueHash const&, Value const&> or std::is_nothrow_copy_constructible_v<ValueHash>);

        // A value's hash whose call operator is not const is called on a copy, as std::hash's own is on a temporary.
        template<class Value>
        [[nodiscard]] constexpr auto operator()(Value const* v) const noexcept(is_nothrow<Value>)
                -> std::size_t
        {
                if (v == nullptr) {
                        return hasher<H, Flavor>::operator()();
                }
                if constexpr (std::is_invocable_v<ValueHash const&, Value const&>) {
                        return m_value_hash(*v);
                } else {
                        auto value_hash = m_value_hash;
                        return value_hash(*v);
                }
        }
};

} // namespace xstd::detail

#endif // XSTD_MISC_FUNCTIONAL_DETAIL_VALUE_HASHER_HPP
