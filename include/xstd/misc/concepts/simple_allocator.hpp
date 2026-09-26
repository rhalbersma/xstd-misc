//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_CONCEPTS_SIMPLE_ALLOCATOR_HPP
#define XSTD_MISC_CONCEPTS_SIMPLE_ALLOCATOR_HPP

#include <concepts> // copy_constructible, equality_comparable, same_as
#include <cstddef>  // size_t

namespace xstd {

// [allocator.requirements.general]/99's exposition-only simple-allocator, word for word.
template<class Alloc>
concept simple_allocator =
        requires (Alloc alloc, std::size_t n) {
                { *alloc.allocate(n) } -> std::same_as<typename Alloc::value_type&>;
                { alloc.deallocate(alloc.allocate(n), n) };
        } and std::copy_constructible<Alloc> and std::equality_comparable<Alloc>;

} // namespace xstd

#endif // XSTD_MISC_CONCEPTS_SIMPLE_ALLOCATOR_HPP
