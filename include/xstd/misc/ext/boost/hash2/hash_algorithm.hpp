//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_EXT_BOOST_HASH2_HASH_ALGORITHM_HPP
#define XSTD_MISC_EXT_BOOST_HASH2_HASH_ALGORITHM_HPP

#include <boost/hash2/has_constant_size.hpp> // has_constant_size
#include <concepts>                          // constructible_from, same_as, semiregular, unsigned_integral
#include <cstddef>                           // size_t
#include <cstdint>                           // uint64_t
#include <ranges>                            // contiguous_range, range_value_t
#include <type_traits>                       // remove_cv_t

namespace xstd {

// Boost.Hash2's documented requirements, the byte seed taken in the form its legacy algorithms share with the rest.
template<class H>
concept hash_algorithm =
        std::semiregular<H> and
        std::constructible_from<H, std::uint64_t> and
        std::constructible_from<H, unsigned char const*, std::size_t> and
        requires (H& h, void const* p, std::size_t n) {
                typename H::result_type;
                h.update(p, n);
                { h.result() } -> std::same_as<typename H::result_type>;
        } and
        // std::unsigned_integral admits bool, a one-bit result that Hash2 does not take.
        ((std::unsigned_integral<typename H::result_type> and not std::same_as<typename H::result_type, bool>) or
         (boost::hash2::has_constant_size<typename H::result_type>::value and std::ranges::contiguous_range<typename H::result_type> and std::same_as<std::ranges::range_value_t<typename H::result_type>, unsigned char>)) and
        // Hash2 documents block_size as std::size_t, so an int one, the spelling that comes to hand, is turned away.
        (not requires { H::block_size; } or std::same_as<std::remove_cv_t<decltype(H::block_size)>, std::size_t>);

} // namespace xstd

#endif // XSTD_MISC_EXT_BOOST_HASH2_HASH_ALGORITHM_HPP
