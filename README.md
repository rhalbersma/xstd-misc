# Miscellaneous extensions to the C++ Standard Library

> ‘The time has come,’ the Walrus said,  
> ‘To talk of many things:  
> Of shoes—and ships—and sealing-wax—  
> Of cabbages—and kings—  
> And why the sea is boiling hot—  
> And whether pigs have wings.’
>
> — Lewis Carroll, *Through the Looking-Glass* (1871), "The Walrus and the Carpenter"

[![Project Status: Active](https://www.repostatus.org/badges/latest/active.svg)](https://www.repostatus.org/#active)
[![Language](https://img.shields.io/badge/language-C++-blue.svg)](https://isocpp.org/)
[![Standard](https://img.shields.io/badge/c%2B%2B-20-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
[![License](https://img.shields.io/badge/license-Boost-blue.svg)](https://opensource.org/licenses/BSL-1.0)
[![GCC](https://github.com/rhalbersma/xstd-misc/actions/workflows/gcc.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/gcc.yml)
[![MinGW](https://github.com/rhalbersma/xstd-misc/actions/workflows/mingw.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/mingw.yml)
[![Clang](https://github.com/rhalbersma/xstd-misc/actions/workflows/clang.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/clang.yml)
[![Clang-libc++](https://github.com/rhalbersma/xstd-misc/actions/workflows/clang-libc%2B%2B.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/clang-libc%2B%2B.yml)
[![Apple Clang](https://github.com/rhalbersma/xstd-misc/actions/workflows/apple-clang.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/apple-clang.yml)
[![Clang-CL](https://github.com/rhalbersma/xstd-misc/actions/workflows/clang-cl.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/clang-cl.yml)
[![MSVC](https://github.com/rhalbersma/xstd-misc/actions/workflows/msvc.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/msvc.yml)
[![Emscripten](https://github.com/rhalbersma/xstd-misc/actions/workflows/emscripten.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/emscripten.yml)
[![Coverage](https://codecov.io/gh/rhalbersma/xstd-misc/branch/main/graph/badge.svg)](https://codecov.io/gh/rhalbersma/xstd-misc)
[![Consumption](https://github.com/rhalbersma/xstd-misc/actions/workflows/consumption.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/consumption.yml)
[![Sanitizers](https://github.com/rhalbersma/xstd-misc/actions/workflows/sanitizers.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/sanitizers.yml)
[![Clang-Tidy](https://github.com/rhalbersma/xstd-misc/actions/workflows/clang-tidy.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/clang-tidy.yml)
[![MSVC-Analyze](https://github.com/rhalbersma/xstd-misc/actions/workflows/msvc-analyze.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/msvc-analyze.yml)
[![CodeQL](https://github.com/rhalbersma/xstd-misc/actions/workflows/codeql.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/codeql.yml)
[![OpenSSF Scorecard](https://api.scorecard.dev/projects/github.com/rhalbersma/xstd-misc/badge)](https://scorecard.dev/viewer/?uri=github.com/rhalbersma/xstd-misc)
[![Documentation](https://github.com/rhalbersma/xstd-misc/actions/workflows/pages.yml/badge.svg)](https://rhalbersma.github.io/xstd-misc/)

xstd-misc is a header-only collection of small extensions to the C++ standard library: what the xstd family needs
beyond integers, which [xstd-ints](https://github.com/rhalbersma/xstd-ints) holds, and bit containers, which
[xstd-bits](https://github.com/rhalbersma/xstd-bits) holds. Each facility lives under the standard header it extends.
[Boost.Hash2](https://www.boost.org/doc/libs/latest/libs/hash2/) is the one dependency. The library relies on the
[C++20](https://wg21.link/N4861) standard and targets the draft [C++29](https://wg21.link/std) standard library, and
every public API is in namespace `xstd`.

## Headers

| Header | Description |
| :----- | :---------- |
| `<xstd/misc.hpp>` | Every header below, through one include. |
| `<xstd/misc/concepts.hpp>` | The concepts below, through one include. |
| `<xstd/misc/concepts/container_compatible_range.hpp>` | The standard's exposition-only *container-compatible-range*, word for word. |
| `<xstd/misc/concepts/hash_algorithm.hpp>` | A Boost.Hash2 hash algorithm, to its documented requirements. |
| `<xstd/misc/concepts/proxy_iterator.hpp>` | An iterator whose reference is a proxy reference. |
| `<xstd/misc/concepts/proxy_reference.hpp>` | A proxy whose address is an iterator dereferencing back to it, and which writes through when `const`. |
| `<xstd/misc/concepts/proxy_semi_iterator.hpp>` | An iterator whose reference is a proxy for its value type. |
| `<xstd/misc/concepts/proxy_semi_reference.hpp>` | A class that converts implicitly to a value type the caller names. |
| `<xstd/misc/concepts/simple_allocator.hpp>` | The standard's exposition-only *simple-allocator*, word for word. |
| `<xstd/misc/concepts/specialization_of.hpp>` | A specialization of a class template whose parameters are all types, `const` or not. |
| `<xstd/misc/format.hpp>` | The formatter below, through one include. |
| `<xstd/misc/format/digest.hpp>` | A `std::formatter` that writes a Boost.Hash2 digest as hex. |
| `<xstd/misc/functional.hpp>` | `hash` and `digest`, through one include. |
| `<xstd/misc/functional/digest.hpp>` | A key's digest at its algorithm's own width, from a seeded Boost.Hash2 algorithm, XXH3-128 by default. |
| `<xstd/misc/functional/hash.hpp>` | An unordered container's hash, from a seeded Boost.Hash2 algorithm, xxHash64 by default, transparent over strings. |
| `<xstd/misc/type_traits.hpp>` | The traits below, through one include. |
| `<xstd/misc/type_traits/conditional_data_member.hpp>` | A data member's type where a condition holds, and a tagged empty type where it does not. |
| `<xstd/misc/type_traits/empty_base_type.hpp>` | A tagged empty type that keeps the place of a base class that is not there. |
| `<xstd/misc/type_traits/empty_member_type.hpp>` | A tagged empty type that keeps the place of a data member that is not there. |
| `<xstd/misc/type_traits/hash_algorithm_is_avalanching.hpp>` | Whether a hash algorithm avalanches, as Boost.Unordered asks: true for every Boost.Hash2 algorithm but FNV-1a. |
| `<xstd/misc/type_traits/is_specialization_of.hpp>` | Whether a type is a specialization of a class template whose parameters are all types. |
| `<xstd/misc/type_traits/no_unique_address.hpp>` | `XSTD_NO_UNIQUE_ADDRESS`, a portable spelling of `[[no_unique_address]]`. |
| `<xstd/misc/utility.hpp>` | `to_underlying`, through one include. |
| `<xstd/misc/utility/to_underlying.hpp>` | `std::to_underlying`, extended to an enumerator in `std::integral_constant` and to a proxy reference to one. |

[The reference](doc/reference.md) specifies each entity, and [the design notes](doc/design.md) give the rationale and
the measurements behind it. [CONTRIBUTING.md](CONTRIBUTING.md) builds the library itself.

## Usage

An unordered container whose keys an adversary may choose takes SipHash, seeded per container, as
[Hash2 advises](https://www.boost.org/doc/libs/latest/libs/hash2/doc/html/hash2.html#hashing_bytes_choosing_a_hash_algorithm):

```cpp
#include <xstd/misc/functional.hpp>
#include <boost/hash2/siphash.hpp>
#include <cstdint>
#include <string>
#include <unordered_set>

using siphasher = xstd::hash<std::string, boost::hash2::siphash_64>;

auto const seed = std::uint64_t{/* drawn at random, per container */};
auto names      = std::unordered_set<std::string, siphasher>(0, siphasher(seed));
```

## Requirements


- A conforming [C++20](https://wg21.link/N4861) compiler
- CMake 3.30 or later when using the supplied CMake project
- [Boost.Hash2](https://www.boost.org/doc/libs/latest/libs/hash2/) 1.88 or later, header-only,
  which the `xstd::misc` target links and its installed package finds

CI covers the toolchains in the table below, and no older ones.

## Installation


The usual approach is CMake's `FetchContent`:

```cmake
include(FetchContent)
FetchContent_Declare(
    xstd-misc
    GIT_REPOSITORY https://github.com/rhalbersma/xstd-misc.git
    GIT_TAG main # Prefer a release tag for reproducible builds.
)
FetchContent_MakeAvailable(xstd-misc)

target_link_libraries(my_target PRIVATE xstd::misc)
```

`find_package(xstd-misc CONFIG REQUIRED)` and `add_subdirectory(external/xstd-misc)` provide
the same `xstd::misc` target. Each finds Boost.Hash2 with `find_package(boost_hash2 CONFIG)`, so it
must be installed where CMake looks, as vcpkg's `boost-hash2` port or Boost 1.88+ puts it.

To build and install the headers alone, without the test suite and its [Boost.Test](https://www.boost.org/doc/libs/latest/libs/test/)
dependency, configure with tests off:

```sh
cmake --preset no-tests   # or: cmake -S . -B build -DBUILD_TESTING=OFF
cmake --install build/no-tests --prefix /usr/local
```

Tests are built only when xstd-misc is the top-level project, so a `FetchContent` or
`add_subdirectory` consumer never needs Boost.Test.

## Continuous Integration


We continuously test the stable, qualification, and development branches of the
major [C++20](https://wg21.link/N4861) toolchains (compilers and standard
libraries) in both Debug and Release mode:

| Platform | Compiler   | Standard Library | Stable                    | Qualification             | Development                    | Status |
| :------- | :--------- | :--------------- | :------------------------ | :------------------------ | :----------------------------- | :----- |
| Linux    | GCC        | libstdc++        | 15                        | 16                        | 17-SVN                         | [![GCC](https://github.com/rhalbersma/xstd-misc/actions/workflows/gcc.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/gcc.yml) |
| Windows  | MinGW      | libstdc++        | 15                        | 16                        | —                              | [![MinGW](https://github.com/rhalbersma/xstd-misc/actions/workflows/mingw.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/mingw.yml) |
| Linux    | Clang      | libstdc++        | 22 (libstdc++ 15)         | 23 (libstdc++ 16)         | 24-SVN (libstdc++ 17-SVN)      | [![Clang](https://github.com/rhalbersma/xstd-misc/actions/workflows/clang.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/clang.yml) |
| Linux    | Clang      | libc++           | 22                        | 23                        | 24-SVN                         | [![Clang-libc++](https://github.com/rhalbersma/xstd-misc/actions/workflows/clang-libc%2B%2B.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/clang-libc%2B%2B.yml) |
| macOS    | Apple Clang | libc++          | 17.0.0 (Xcode 16.4)       | 21.0.0 (Xcode 26.6)       | —                              | [![Apple Clang](https://github.com/rhalbersma/xstd-misc/actions/workflows/apple-clang.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/apple-clang.yml) |
| Windows  | Clang-CL   | MSVC             | 19.1.5 (VS 2022)          | 20.1.8 (VS 2026)          | 20.1.8 (VS 2026-Preview)       | [![Clang-CL](https://github.com/rhalbersma/xstd-misc/actions/workflows/clang-cl.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/clang-cl.yml) |
| Windows  | MSVC       | MSVC             | 2022 (17.11+)             | 2026                      | 2026-Preview                   | [![MSVC](https://github.com/rhalbersma/xstd-misc/actions/workflows/msvc.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/msvc.yml) |
| Web      | Emscripten | libc++           | 6.0.12                    | —                         | —                              | [![Emscripten](https://github.com/rhalbersma/xstd-misc/actions/workflows/emscripten.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/emscripten.yml) |

The `Web` row runs one Emscripten release on the stable rung, in Debug: wasm32 under node,
where `size_t` is 32 bits but 64-bit integers keep their 8-byte alignment. The GCC workflow
adds a second 32-bit leg, i686 with GCC `-m32`, where `size_t` is 32 bits and there is no
`__int128`. The MSVC workflow adds a third, x86 with VS 2026 in Debug, where `size_t` is 32
bits and 64-bit members keep their 8-byte alignment under MSVC's own ABI.

## Acknowledgements


We acknowledge Walter E. Brown for discussing the idea for `std::to_underlying`
and JeanHeyd Meneide for drafting it into an actual proposal and shepherding
[P1682](https://github.com/cplusplus/papers/issues/460) through the C++ standardization process.

## License


<pre>
         Copyright Rein Halbersma 2014-2026.
Distributed under the <a href="http://www.boost.org/users/license.html">Boost Software License, Version 1.0</a>.
   (See accompanying file LICENSE_1_0.txt or copy at
         <a href="http://www.boost.org/LICENSE_1_0.txt">http://www.boost.org/LICENSE_1_0.txt</a>)
</pre>
