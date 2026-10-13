//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_EXT_BOOST_HASH2_DIGESTER_HPP
#define XSTD_MISC_EXT_BOOST_HASH2_DIGESTER_HPP

#include <xstd/misc/ext/boost/hash2/hash_algorithm.hpp> // hash_algorithm
#include <boost/hash2/hash_append.hpp>                  // hash_append
#include <boost/hash2/xxh3.hpp>                         // xxh3_128
#include <cstddef>                                      // size_t
#include <cstdint>                                      // uint64_t

namespace xstd {

// A key's fingerprint: the algorithm's whole result on every target, where a hasher narrows it to std::size_t.
template<hash_algorithm H = boost::hash2::xxh3_128>
class digester
{
        H m_prototype{};

public:
        // NOLINTNEXTLINE(readability-redundant-typename): clang-cl's -Wc++20-compat rejects the bare dependent name.
        using result_type = typename H::result_type;

        [[nodiscard]] digester() = default;

        // Not seed, which MSVC's C4459 flags here wherever a consumer declares a seed at namespace scope.
        [[nodiscard]] constexpr explicit digester(std::uint64_t s)
                : m_prototype(s)
        {}

        [[nodiscard]] constexpr digester(unsigned char const* p, std::size_t n)
                : m_prototype(p, n)
        {}

        // Neither constrained nor noexcept: Hash2 declares hash_append void for every T, and it runs the user's code.
        template<class T>
        [[nodiscard]] constexpr auto operator()(T const& v) const
                -> result_type
        {
                auto h = m_prototype;
                boost::hash2::hash_append(h, {}, v);
                return h.result();
        }
};

} // namespace xstd

#endif // XSTD_MISC_EXT_BOOST_HASH2_DIGESTER_HPP
