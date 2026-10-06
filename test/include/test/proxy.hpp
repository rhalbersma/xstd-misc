//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef TEST_PROXY_HPP
#define TEST_PROXY_HPP

#include <cstddef> // ptrdiff_t, size_t
#include <span>    // span

namespace test {

enum class shade : unsigned char {
        light = 1,
        dark  = 2,
};

template<class T>
class element_iterator;

// An element proxy closed under & and *, as a packed container's is: the address is the iterator it came from.
template<class T>
class element_reference
{
        std::span<T const> m_range;
        std::size_t m_idx;

public:
        [[nodiscard]] constexpr element_reference(std::span<T const> range, std::size_t idx) noexcept
                : m_range(range)
                , m_idx(idx)
        {}

        [[nodiscard]] constexpr explicit(false) operator T() const noexcept
        {
                return m_range[m_idx];
        }

        [[nodiscard]] constexpr auto operator&() const noexcept
                -> element_iterator<T>
        {
                return element_iterator<T>(m_range, m_idx);
        }
};

template<class T>
class element_iterator
{
        std::span<T const> m_range;
        std::size_t m_idx{};

public:
        using value_type      = T;
        using difference_type = std::ptrdiff_t;

        [[nodiscard]] element_iterator() = default;

        [[nodiscard]] constexpr element_iterator(std::span<T const> range, std::size_t idx) noexcept
                : m_range(range)
                , m_idx(idx)
        {}

        [[nodiscard]] friend constexpr auto operator==(element_iterator lhs, element_iterator rhs) noexcept
                -> bool
        {
                return lhs.m_idx == rhs.m_idx;
        }

        [[nodiscard]] constexpr auto operator*() const noexcept
                -> element_reference<T>
        {
                return element_reference<T>(m_range, m_idx);
        }

        constexpr auto operator++() noexcept
                -> element_iterator&
        {
                ++m_idx;
                return *this;
        }

        constexpr auto operator++(int) noexcept
                -> element_iterator
        {
                auto nrv = *this;
                ++*this;
                return nrv;
        }
};

// Converts like a proxy but keeps the built-in address, so nothing leads back from it to an iterator.
struct wrapper
{
        shade value;

        [[nodiscard]] constexpr explicit(false) operator shade() const noexcept
        {
                return value;
        }
};

// Its address is an iterator, but one that dereferences to the value rather than back to the proxy.
struct one_way_reference
{
        shade const* ptr;

        [[nodiscard]] constexpr explicit(false) operator shade() const noexcept
        {
                return *ptr;
        }

        [[nodiscard]] constexpr auto operator&() const noexcept
                -> shade const*
        {
                return ptr;
        }
};

} // namespace test

#endif // TEST_PROXY_HPP
