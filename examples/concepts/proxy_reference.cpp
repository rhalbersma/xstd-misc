//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/concepts/proxy_reference.hpp> // proxy_reference
#include <cstddef>                                // ptrdiff_t
#include <iostream>                               // boolalpha, cout
#include <iterator>                               // iter_value_t
#include <vector>                                 // vector

enum class shade : unsigned char
{
        light,
        dark
};

class shade_iterator;

// One shade, kept as one bit of a word, and read and written through this proxy.
class shade_reference
{
        unsigned* m_word;
        unsigned m_bit;

public:
        [[nodiscard]] constexpr shade_reference(unsigned* word, unsigned bit) noexcept
                : m_word(word)
                , m_bit(bit)
        {}

        [[nodiscard]] constexpr explicit(false) operator shade() const noexcept
        {
                return static_cast<shade>((*m_word >> m_bit) & 1U);
        }

        // Writes through a const proxy, as a const language reference writes to what it refers to.
        constexpr auto operator=(shade value) const noexcept
                -> shade_reference const&
        {
                *m_word = (*m_word & ~(1U << m_bit)) | (static_cast<unsigned>(value) << m_bit);
                return *this;
        }

        // The address of a shade is an iterator that yields this proxy back.
        [[nodiscard]] constexpr auto operator&() const noexcept
                -> shade_iterator;
};

class shade_iterator
{
        unsigned* m_word;
        unsigned m_bit;

public:
        using value_type      = shade;
        using difference_type = std::ptrdiff_t;

        [[nodiscard]] constexpr shade_iterator(unsigned* word, unsigned bit) noexcept
                : m_word(word)
                , m_bit(bit)
        {}

        [[nodiscard]] constexpr auto operator*() const noexcept
                -> shade_reference
        {
                return shade_reference(m_word, m_bit);
        }
};

constexpr auto shade_reference::operator&() const noexcept
        -> shade_iterator
{
        return shade_iterator(m_word, m_bit);
}

[[nodiscard]] constexpr auto name(shade s) noexcept
        -> char const*
{
        return s == shade::dark ? "dark" : "light";
}

// Resets what a language reference refers to.
template<class T>
auto reset(T& value) -> void
{
        value = T();
}

// Resets what a proxy reference stands for: taken by value, it still writes through, and its address names the value.
template<xstd::proxy_reference R>
auto reset(R ref) -> void
{
        ref = std::iter_value_t<decltype(&ref)>();
}

auto main() -> int
{
        auto plain = shade::dark;
        auto word  = 0b11U;

        reset(plain);
        reset(*shade_iterator(&word, 1));
        std::cout << "plain is " << name(plain) << ", the packed shades are " << name(*shade_iterator(&word, 0)) << " and " << name(*shade_iterator(&word, 1)) << '\n';

        std::cout << std::boolalpha;
        std::cout << "proxy_reference<shade_reference> is " << xstd::proxy_reference<shade_reference> << '\n';
        std::cout << "proxy_reference<shade&> is " << xstd::proxy_reference<shade&> << '\n';
        std::cout << "proxy_reference<vector<bool>::const_reference> is " << xstd::proxy_reference<std::vector<bool>::const_reference> << '\n';
}
