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

xstd-misc is a header-only collection of small extensions to the C++ standard
library: what the xstd family needs beyond integers, which
[xstd-ints](https://github.com/rhalbersma/xstd-ints) holds, and bit containers, which
[xstd-bits](https://github.com/rhalbersma/xstd-bits) holds. Each lives under the standard header it extends, so the
layout mirrors `<concepts>`, `<format>`, `<functional>`, `<type_traits>` and
`<utility>`. It adds two pairs
of a concept and a trait for recognizing a specialization of a class template --
one naming the template, one naming an example of it -- tagged empty
types that keep a place in a class layout, a data member present only when a
condition holds, a portable spelling of `[[no_unique_address]]`, four
concepts recognizing proxy iterators and proxy references, and `to_underlying`
over a plain enum, one wrapped in `std::integral_constant`, and a proxy for one.
Under `<functional>`, it adds `hash`, the hash function object of an unordered container, which
runs a [Boost.Hash2](https://www.boost.org/doc/libs/release/libs/hash2/) algorithm of the
caller's choosing, seeded per container, and is transparent over strings; and `digest`, which
runs one for a key's digest at the algorithm's own width. Beside them, `hash_algorithm` is the concept that
algorithm satisfies, and `hash_algorithm_is_avalanching` tells Boost.Unordered whether
the algorithm's hash can be used without mixing it again.

Boost.Hash2 is the one dependency, and the CMake target and the installed package bring it
along. It relies on the [C++20](https://wg21.link/N4861) standard and
targets the draft [C++29](https://wg21.link/std) standard library. All public
APIs are in namespace `xstd`.

## Usage


`to_underlying` is one spelling over both forms of an enum value: it forwards a
plain one to the standard's, and preserves a wrapped one at the type level.

```cpp
#include <xstd/misc/utility.hpp>
#include <type_traits>

enum class color : unsigned { red = 1 };

static_assert(xstd::to_underlying(color::red) == 1u);
static_assert(decltype(xstd::to_underlying(std::integral_constant<color, color::red>()))::value == 1u);
```

A proxy stands in for a value it converts to, as a packed container's element
does, and that conversion is one a deduced parameter never considers. Four
concepts recognize proxies, in two strengths:

| Concept | Recognizes |
|---|---|
| `proxy_semi_reference<R, V>` | a class `R` that converts implicitly to the `V` the caller names |
| `proxy_semi_iterator<I>` | an iterator whose reference is a semi-reference for its value type |
| `proxy_reference<R>` | a proxy closed under `&` and `*`, and written through when `const`: its address is a semi-iterator dereferencing back to it, which names its value |
| `proxy_iterator<I>` | a semi-iterator whose reference is a proxy reference |

`to_underlying` reads through a proxy reference to an enumeration:

```cpp
#include <xstd/misc/concepts.hpp>
#include <xstd/misc/utility.hpp>
#include <ranges>
#include <vector>

static_assert(xstd::proxy_semi_iterator<std::vector<bool>::iterator>);
static_assert(xstd::proxy_semi_reference<std::vector<bool>::reference, bool>);

template<std::ranges::input_range R>
auto sum_of_underlying(R const& packed_enums)
{
    auto sum = 0;
    for (auto element : packed_enums) {
        sum += xstd::to_underlying(element); // element is the container's proxy, not the enum
    }
    return sum;
}
```

Every library's `std::vector<bool>::reference` and `std::bitset<N>::reference` is
a semi-reference for `bool`, and `std::vector<bool>`'s iterator and `zip_view`'s
are semi-iterators. Whether the bit references are also proxy references depends
on the library: libc++ 22 gives them an `&` that leads to an iterator, and
libstdc++ gives them none. The concepts otherwise ask what the standard asks:
a semi-iterator is `std::indirectly_readable`, and a proxy reference's address is
also `std::indirectly_writable`, which [P2321](https://wg21.link/p2321) makes
`std::vector<bool>`'s iterator by assignment through a `const` proxy. A read-only
proxy, such as libc++'s `const_reference` or a set's element, is therefore a
semi-reference only: convert it first, as
`xstd::to_underlying(static_cast<E>(key))`. The round trip through `&` is the one
clause the standard has no counterpart for.

A concept cannot name itself, even through another, so the four are layered one
way, each over those before it. That a proxy reference's address is a proxy
iterator follows rather than being required: it is a semi-iterator whose
reference is the proxy reference itself, which is a semi-reference for the value
that address names.

Only the iterators order by their constraints: an overload on `proxy_iterator`
is taken over one on `proxy_semi_iterator`. A semi-reference is for a value the
caller names, while a proxy reference reads its value from its address, so an
overload on `proxy_semi_reference<R, bool>` and one on `proxy_reference<R>` are
ambiguous for a type that is both. Set the weaker one aside with
`requires xstd::proxy_semi_reference<R, bool> and (not xstd::proxy_reference<R>)`.

`empty_member_type` and `conditional_data_member_t` give a member that is present only
under a condition, and `XSTD_NO_UNIQUE_ADDRESS` costs it nothing when it is not.
The tag keeps two absent members distinct, which their layout would otherwise
collapse:

```cpp
#include <xstd/misc/type_traits.hpp>
#include <memory>

template<bool Stateful, class Allocator = std::allocator<int>>
struct storage {
    [[XSTD_NO_UNIQUE_ADDRESS]] xstd::conditional_data_member_t<Stateful, Allocator, struct alloc> allocator;
    int value;
};

static_assert(sizeof(storage<false>) == sizeof(int));
```

Use `XSTD_NO_UNIQUE_ADDRESS` inside an attribute-specifier. It expands to
`msvc::no_unique_address` with the MSVC-compatible frontend, which keeps the
standard spelling layout-neutral, and to `no_unique_address` elsewhere.

`is_specialization_of` answers whether a type is a specialization of a class
template whose parameters are all types, and `specialization_of` is the same
question where a constraint is what a caller writes:

```cpp
#include <xstd/misc/concepts.hpp>
#include <xstd/misc/type_traits.hpp>
#include <complex>

static_assert(xstd::is_specialization_of_v<std::complex<double>, std::complex>);
static_assert(xstd::specialization_of<std::complex<double>, std::complex>);
```

Only the all-types shape is offered, the one p2098 names. A template with a value parameter,
such as `std::array`, is not matched: the kinds are part of a template's type and no template
template parameter binds them all, so a caller with such a template writes its own two-line
trait over it.

A constrained parameter is no obstacle: an unconstrained template template parameter does not
consider the constraints on its argument, so a `template<std::integral Block>` storage binds
where a `template<class>` one does.

The trait is the exact question and the concept is the one a caller writes, which is why only
the concept sees through a `const`: an adaptor over a const owner names `Container const`, and
no specialization pattern matches that. A reference is a specialization of nothing either way.

```cpp
#include <xstd/misc/concepts.hpp>
#include <vector>

template<xstd::specialization_of<std::vector> Owner>
class view { Owner* owner; };

using mutable_view = view<std::vector<int>>;        // the owner as it is
using const_view   = view<std::vector<int> const>;  // and a view over a const one
```


`simple_allocator` and `container_compatible_range` are the standard's exposition-only
*simple-allocator* ([allocator.requirements.general]) and *container-compatible-range*
([container.intro.reqmts]), word for word, for a container that constrains its allocator
arguments and `from_range` constructors as the standard's do.

```cpp
#include <xstd/misc/concepts.hpp>
#include <memory>
#include <vector>

static_assert(xstd::simple_allocator<std::allocator<int>>);
static_assert(xstd::container_compatible_range<std::vector<int>, long>);
```

`hash<T, H>` hashes a key of type `T` through `hash_append` with the Boost.Hash2 algorithm `H`,
holding a seeded prototype of `H` and copying it on every call, and returns the `std::size_t` an
unordered container asks for. The default is `boost::hash2::xxhash_64` on every platform: it
avalanches at any key length, where FNV-1a leaves the low bits a masked table takes blind to the
high bits of the last byte. For keys an adversary chooses, name SipHash and seed it per container,
as Hash2 advises. Without a secret seed, an attacker who knows the hash searches offline for keys
that share a slot: 2.7·10⁸ hashes find 32,768 that lengthen a `boost::unordered_flat_map`'s
average insertion probe from 1.07 groups to 786, and 7·10⁷ find 8,192 that fill one bucket of a
reserved `std::unordered_map`, under xxHash, FNV-1a and a publicly seeded SipHash alike. A seed
drawn per container leaves both tables as they are with random keys:

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

The constructors are the three every Hash2 algorithm has: none, a `std::uint64_t` seed, and a
byte sequence `(unsigned char const*, std::size_t)`. Each argument converts to `T` before it is
hashed, so `xstd::hash<std::int64_t>()(42)` hashes the 8 bytes of the key a table holds rather
than the 4 of an `int`. Over a `std::basic_string` it is transparent, taking the string's view, so
a string, a `std::string_view` and a C string with the same characters hash equal, and a set
looks a key up by any of them without building a string:

```cpp
#include <xstd/misc/functional.hpp>
#include <functional>
#include <string>
#include <string_view>
#include <unordered_set>

auto names = std::unordered_set<std::string, xstd::hash<std::string>, std::equal_to<>>{"one", "two"};
auto found = names.contains(std::string_view("two"));                // no std::string is built
```

No other key is transparent: equal values of different types, such as `42` and
`std::int64_t{42}`, need not append the same bytes, so a lookup by one would miss the other.

`hash<T, H>` reports itself avalanching, through the nested `is_avalanching` that
`boost::hash_is_avalanching` reads, wherever `xstd::hash_algorithm_is_avalanching<H>` holds, and
Boost.Unordered's open-addressing containers then use its hash as it is. The trait holds for every
Hash2 algorithm but FNV-1a, and every translation unit sees the same answer without including the
algorithm. An algorithm written outside Hash2 is false until its author says otherwise, next to its
definition, and deriving from an avalanching algorithm does not inherit the answer:

```cpp
#include <xstd/misc/type_traits.hpp>
#include <boost/hash2/xxhash.hpp>
#include <type_traits>

struct my_algorithm : boost::hash2::xxhash_64 {
    using xxhash_64::xxhash_64;
};

template<>
struct xstd::hash_algorithm_is_avalanching<my_algorithm> : std::true_type {};

static_assert(xstd::hash_algorithm_is_avalanching_v<my_algorithm>);
```

`digest<T, H>` runs the same seeded prototype over the same `hash_append`, but returns the
algorithm's whole `result_type` where `hash` folds it to `std::size_t`: a digest that stands in
for a key, such as a transposition table's, is as wide on a 32-bit target as on a 64-bit one. It
should be wide enough that two keys colliding is never the likeliest bug, at any table size, so the
default is `boost::hash2::xxh3_128`, whose result is a 16-byte `digest<16>`, as
[xxHash](https://github.com/Cyan4973/xxHash) recommends where a 128-bit hash is required. Its
per-call cost is two to four times xxHash64's on short keys, which a table hashing every lookup
would feel and a digest computed once does not. Name `xxhash_64` for a 64-bit one:

```cpp
#include <xstd/misc/functional.hpp>
#include <boost/hash2/xxhash.hpp>
#include <cstdint>
#include <string>

auto const position = xstd::digest<std::string>(std::uint64_t{/* fixed, so digests persist */});
auto const stored   = position("r1/p1/...");                              // boost::hash2::digest<16>
auto const key64    = xstd::digest<int, boost::hash2::xxhash_64>()(42);   // std::uint64_t everywhere
```

A digest persists across runs, builds and targets, for keys of fixed-width integer types,
strings, and arrays of those. Both classes take a Boost.Hash2 flavor as their third parameter,
which fixes the bytes `hash_append` writes. `digest` defaults to `boost::hash2::little_endian_flavor`,
so an integer appends the same bytes on a big-endian target as on a little-endian one, and a
range's size appends as a `std::uint32_t`; the tests check those bytes' digests against xxHash's
reference implementation, and the flavor is the same in every release from 1.88 to 1.92. `hash`
defaults to `boost::hash2::default_flavor`, the target's own byte order, as a table's hash never
leaves the process. A key of `long`, `std::size_t` or `wchar_t`, whose width differs between
platforms, gives different bytes on each.

A `boost::hash2::digest<N>`, the result of XXH3-128 and every other algorithm with an array-like
result, formats through `std::format` and `std::print` as hex, two digits per byte in byte order,
as `boost::hash2::to_string` writes it, with `{:X}` for uppercase:

```cpp
#include <xstd/misc/format.hpp>
#include <xstd/misc/functional.hpp>
#include <format>
#include <string>

auto const text = std::format("{}", xstd::digest<std::string>()("r1/p1/..."));   // 32 hex digits
```

`H` is any type satisfying `hash_algorithm`, Boost.Hash2's documented requirements as a concept,
so an algorithm written outside Hash2 is checked where it is named:

```cpp
#include <xstd/misc/concepts.hpp>
#include <boost/hash2/siphash.hpp>

static_assert(xstd::hash_algorithm<boost::hash2::siphash_64>);
static_assert(not xstd::hash_algorithm<int>);
```

See [the design notes](doc/design.md) for rationale, and
[CONTRIBUTING.md](CONTRIBUTING.md) to build the library itself.

## Headers


| Header | Additions | Description | Reference |
| :----- | :-------- | :---------- | :-------- |
| `<xstd/misc/concepts/specialization_of.hpp>` | `specialization_of` | Constraint form of `is_specialization_of`, seeing through a `const` owner | [p2098r1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p2098r1.pdf) (relationship documented) |
| `<xstd/misc/type_traits/is_specialization_of.hpp>` | `is_specialization_of` | Is a type a specialization of a class template? | [p2098r1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p2098r1.pdf) (relationship documented) |
| `<xstd/misc/type_traits/no_unique_address.hpp>` | `XSTD_NO_UNIQUE_ADDRESS` | Portable spelling of `no_unique_address` | none |
| `<xstd/misc/type_traits/empty_base_type.hpp>` | `empty_base_type` | A tagged empty type for a base class that is not there | none |
| `<xstd/misc/type_traits/empty_member_type.hpp>` | `empty_member_type` | A tagged empty type for a data member that is not there | none |
| `<xstd/misc/type_traits/conditional_data_member.hpp>` | `conditional_data_member` | A conditionally present member | none |
| `<xstd/misc/utility/to_underlying.hpp>` | `to_underlying` | `std::to_underlying`, plus an `std::integral_constant` overload | [p1682r1](https://wg21.link/p1682r1) (`std::to_underlying`) |
| `<xstd/misc/concepts/hash_algorithm.hpp>` | `hash_algorithm` | A Boost.Hash2 hash algorithm, to its documented requirements | [Boost.Hash2](https://www.boost.org/doc/libs/release/libs/hash2/) |
| `<xstd/misc/type_traits/hash_algorithm_is_avalanching.hpp>` | `hash_algorithm_is_avalanching` | Whether a hash algorithm avalanches: true for every Boost.Hash2 algorithm but FNV-1a, and specializable for one's own | [Boost.Unordered](https://www.boost.org/doc/libs/release/libs/unordered/) (`hash_is_avalanching`) |
| `<xstd/misc/format/digest.hpp>` | `formatter<boost::hash2::digest<N>>` | Formats a Boost.Hash2 digest as hex | [Boost.Hash2](https://www.boost.org/doc/libs/release/libs/hash2/) |
| `<xstd/misc/format.hpp>` | `formatter<boost::hash2::digest<N>>` | The one above, through one include | none |
| `<xstd/misc/functional/digest.hpp>` | `digest` | A key's digest at the algorithm's own width, from a seeded Boost.Hash2 algorithm, XXH3-128 by default | [Boost.Hash2](https://www.boost.org/doc/libs/release/libs/hash2/) |
| `<xstd/misc/functional/hash.hpp>` | `hash` | A key's hash for an unordered container, from a seeded Boost.Hash2 algorithm, xxHash64 by default, transparent over `std::basic_string` | [Boost.Hash2](https://www.boost.org/doc/libs/release/libs/hash2/), [N3980](https://wg21.link/n3980) |
| `<xstd/misc/functional.hpp>` | `digest` <br> `hash` | The two above, through one include | [Boost.Hash2](https://www.boost.org/doc/libs/release/libs/hash2/) |

Each directory has an umbrella exporting what is under it -- `<xstd/misc/concepts.hpp>`,
`<xstd/misc/format.hpp>`, `<xstd/misc/functional.hpp>`, `<xstd/misc/type_traits.hpp>`,
`<xstd/misc/utility.hpp>` -- and `<xstd/misc.hpp>` exports the five, so one include brings the whole surface.

## Requirements


- A conforming [C++20](https://wg21.link/N4861) compiler
- CMake 3.30 or later when using the supplied CMake project
- [Boost.Hash2](https://www.boost.org/doc/libs/release/libs/hash2/) 1.88 or later, header-only,
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

To build and install the headers alone, without the test suite and its Boost.Test
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
[P1682](https://wg21.link/p1682) through the C++ standardization process.

## License


<pre>
         Copyright Rein Halbersma 2014-2026.
Distributed under the <a href="http://www.boost.org/users/license.html">Boost Software License, Version 1.0</a>.
   (See accompanying file LICENSE_1_0.txt or copy at
         <a href="http://www.boost.org/LICENSE_1_0.txt">http://www.boost.org/LICENSE_1_0.txt</a>)
</pre>
