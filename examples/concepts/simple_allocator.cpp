//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/concepts/simple_allocator.hpp> // simple_allocator
#include <array>                                   // array
#include <cstddef>                                 // byte
#include <iostream>                                // boolalpha, cout
#include <memory>                                  // allocator
#include <memory_resource>                         // memory_resource, monotonic_buffer_resource, polymorphic_allocator

// Stores a value in storage from any allocator, reads it back and returns the storage.
template<xstd::simple_allocator Alloc>
[[nodiscard]] auto round_trip(Alloc alloc, typename Alloc::value_type value)
        -> typename Alloc::value_type
{
        auto* const storage = alloc.allocate(1);
        *storage            = value;
        auto const read     = *storage;
        alloc.deallocate(storage, 1);
        return read;
}

auto main() -> int
{
        auto buffer   = std::array<std::byte, 256>();
        auto resource = std::pmr::monotonic_buffer_resource(buffer.data(), buffer.size());

        std::cout << round_trip(std::allocator<int>(), 1) << '\n';
        std::cout << round_trip(std::pmr::polymorphic_allocator<int>(&resource), 2) << '\n';

        // A memory resource hands out bytes, not elements, so it is no allocator.
        std::cout << std::boolalpha;
        std::cout << "simple_allocator<std::allocator<int>> is " << xstd::simple_allocator<std::allocator<int>> << '\n';
        std::cout << "simple_allocator<std::pmr::polymorphic_allocator<int>> is " << xstd::simple_allocator<std::pmr::polymorphic_allocator<int>> << '\n';
        std::cout << "simple_allocator<std::pmr::memory_resource*> is " << xstd::simple_allocator<std::pmr::memory_resource*> << '\n';
}
