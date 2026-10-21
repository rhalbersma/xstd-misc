//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/concepts/proxy_iterator.hpp>      // proxy_iterator
#include <xstd/misc/concepts/proxy_semi_iterator.hpp> // proxy_semi_iterator
#include <array>                                      // array
#include <cstddef>                                    // ptrdiff_t
#include <iostream>                                   // boolalpha, cout
#include <type_traits>                                // is_pointer_v

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

// The address of the element an iterator points to: for a pointer and for a proxy iterator, an iterator to it again.
template<class I>
        requires std::is_pointer_v<I> or xstd::proxy_iterator<I>
[[nodiscard]] constexpr auto address_of_element(I it)
{
        return &*it;
}

auto main() -> int
{
        auto plain = std::array<shade, 2>{shade::light, shade::light};
        auto word  = 0b00U;

        *address_of_element(&plain[1])                = shade::dark;
        *address_of_element(shade_iterator(&word, 1)) = shade::dark;
        std::cout << "plain: " << name(plain[0]) << ' ' << name(plain[1]) << '\n';
        std::cout << "packed: " << name(*shade_iterator(&word, 0)) << ' ' << name(*shade_iterator(&word, 1)) << '\n';

        std::cout << std::boolalpha;
        std::cout << "proxy_iterator<shade_iterator> is " << xstd::proxy_iterator<shade_iterator> << '\n';
        std::cout << "proxy_semi_iterator<shade_iterator> is " << xstd::proxy_semi_iterator<shade_iterator> << '\n';
        std::cout << "proxy_iterator<shade*> is " << xstd::proxy_iterator<shade*> << '\n';
}
