//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_FORMAT_DIGEST_HPP
#define XSTD_MISC_FORMAT_DIGEST_HPP

#include <boost/hash2/digest.hpp> // digest
#include <cstddef>                // size_t
#include <format>                 // format_error, format_parse_context, formatter
#include <iterator>               // next
#include <string_view>            // string_view

namespace std {

// Lowercase hex in byte order, as boost::hash2::to_string prints, or uppercase with X.
template<std::size_t N>
// NOLINTNEXTLINE(bugprone-std-namespace-modification): [namespace.std] admits specializations on program-defined types.
struct formatter<boost::hash2::digest<N>>
{
        bool m_uppercase = false;

        [[nodiscard]] constexpr auto parse(std::format_parse_context& ctx)
                -> std::format_parse_context::iterator
        {
                std::format_parse_context::iterator it = ctx.begin();
                if (it != ctx.end() and (*it == 'x' or *it == 'X')) {
                        m_uppercase = *it == 'X';
                        it          = std::next(it);
                }
                if (it != ctx.end() and *it != '}') {
                        throw std::format_error("a digest takes x, X or no format specification");
                }
                return it;
        }

        template<class FormatContext>
        [[nodiscard]] auto format(boost::hash2::digest<N> const& d, FormatContext& ctx) const
                -> decltype(ctx.out())
        {
                auto const digits = std::string_view(m_uppercase ? "0123456789ABCDEF" : "0123456789abcdef");
                auto out          = ctx.out();
                for (auto const octet : d) {
                        *out++ = digits[static_cast<unsigned>(octet) >> 4U];
                        *out++ = digits[static_cast<unsigned>(octet) & 0x0FU];
                }
                return out;
        }
};

} // namespace std

#endif // XSTD_MISC_FORMAT_DIGEST_HPP
