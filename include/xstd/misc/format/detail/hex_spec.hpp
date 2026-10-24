//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_FORMAT_DETAIL_HEX_SPEC_HPP
#define XSTD_MISC_FORMAT_DETAIL_HEX_SPEC_HPP

#include <xstd/misc/format/detail/throw_format_error.hpp> // throw_format_error
#include <format>                                         // format_parse_context
#include <iterator>                                       // next
#include <string_view>                                    // string_view

namespace xstd::detail {

// The one specification a hex formatter takes: x or nothing for lowercase digits, X for uppercase ones.
class hex_spec
{
        bool m_uppercase = false;

public:
        [[nodiscard]] constexpr auto parse(std::format_parse_context& ctx)
                -> std::format_parse_context::iterator
        {
                std::format_parse_context::iterator it = ctx.begin();
                if (it != ctx.end() and (*it == 'x' or *it == 'X')) {
                        m_uppercase = *it == 'X';
                        it          = std::next(it);
                }
                if (it != ctx.end() and *it != '}') {
                        throw_format_error("a digest takes x, X or no format specification");
                }
                return it;
        }

        [[nodiscard]] constexpr auto digits() const noexcept
                -> std::string_view
        {
                return m_uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
        }
};

} // namespace xstd::detail

#endif // XSTD_MISC_FORMAT_DETAIL_HEX_SPEC_HPP
