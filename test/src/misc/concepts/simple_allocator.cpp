//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/concepts/simple_allocator.hpp> // simple_allocator
#include <boost/test/unit_test.hpp>                // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE, BOOST_CHECK
#include <cstddef>                                 // size_t
#include <functional>                              // less
#include <memory>                                  // allocator
#include <memory_resource>                         // polymorphic_allocator

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(Concepts)
BOOST_AUTO_TEST_SUITE(SimpleAllocator)

// Allocates, but its allocation cannot be handed back: no deallocate.
struct allocate_only
{
        using value_type = int;

        [[nodiscard, maybe_unused]] auto allocate(std::size_t) -> int*;
        [[nodiscard, maybe_unused]] friend auto operator==(allocate_only, allocate_only) -> bool = default;
};

// The standard's own allocators are simple allocators, a comparator and a plain type are not.
BOOST_AUTO_TEST_CASE(TheStandardAllocatorsModelIt)
{
        static_assert(xstd::simple_allocator<std::allocator<int>>);
        static_assert(xstd::simple_allocator<std::pmr::polymorphic_allocator<int>>);
        static_assert(not xstd::simple_allocator<std::less<int>>);
        static_assert(not xstd::simple_allocator<int>);
        static_assert(not xstd::simple_allocator<allocate_only>);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
