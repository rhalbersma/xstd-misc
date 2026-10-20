//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_FORMAT_DIGEST_HPP
#define XSTD_MISC_FORMAT_DIGEST_HPP

#include <xstd/misc/format/detail/hex_spec.hpp> // hex_spec
#include <boost/hash2/digest.hpp>               // digest
#include <cstddef>                              // size_t
#include <format>                               // formatter

namespace std {

// Lowercase hex in byte order, as boost::hash2::to_string prints, or uppercase with X.
template<std::size_t N>
// NOLINTNEXTLINE(bugprone-std-namespace-modification): [namespace.std] admits specializations on program-defined types.
struct formatter<boost::hash2::digest<N>> : xstd::detail::hex_spec
{
        template<class FormatContext>
        [[nodiscard]] auto format(boost::hash2::digest<N> const& d, FormatContext& ctx) const
                -> decltype(ctx.out())
        {
                auto const hex = digits();
                auto out       = ctx.out();
                for (auto const octet : d) {
                        auto const value = std::size_t{octet};
                        *out++           = hex[value / hex.size()];
                        *out++           = hex[value % hex.size()];
                }
                return out;
        }
};

} // namespace std

#endif // XSTD_MISC_FORMAT_DIGEST_HPP
