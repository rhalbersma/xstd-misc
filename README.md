# Miscellaneous extensions to the C++ Standard Library

> ‘The time has come,’ the Walrus said,  
> ‘To talk of many things:  
> Of shoes—and ships—and sealing-wax—  
> Of cabbages—and kings—  
> And why the sea is boiling hot—  
> And whether pigs have wings.’
>
> -- Lewis Carroll, *Through the Looking-Glass* (1871), "The Walrus and the Carpenter"

[![Language](https://img.shields.io/badge/language-C++-blue.svg)](https://isocpp.org/)
[![Standard](https://img.shields.io/badge/c%2B%2B-23-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
[![License](https://img.shields.io/badge/license-Boost-blue.svg)](https://opensource.org/licenses/BSL-1.0)
[![GCC](https://github.com/rhalbersma/xstd-misc/actions/workflows/gcc.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/gcc.yml)
[![MinGW](https://github.com/rhalbersma/xstd-misc/actions/workflows/mingw.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/mingw.yml)
[![Clang](https://github.com/rhalbersma/xstd-misc/actions/workflows/clang.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/clang.yml)
[![Clang-libc++](https://github.com/rhalbersma/xstd-misc/actions/workflows/clang-libc%2B%2B.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/clang-libc%2B%2B.yml)
[![Apple Clang](https://github.com/rhalbersma/xstd-misc/actions/workflows/apple-clang.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/apple-clang.yml)
[![Clang-CL](https://github.com/rhalbersma/xstd-misc/actions/workflows/clang-cl.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/clang-cl.yml)
[![MSVC](https://github.com/rhalbersma/xstd-misc/actions/workflows/msvc.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/msvc.yml)
[![Coverage](https://codecov.io/gh/rhalbersma/xstd-misc/branch/main/graph/badge.svg)](https://codecov.io/gh/rhalbersma/xstd-misc)
[![OpenSSF Scorecard](https://api.scorecard.dev/projects/github.com/rhalbersma/xstd-misc/badge)](https://scorecard.dev/viewer/?uri=github.com/rhalbersma/xstd-misc)

xstd-misc is the assortment: the small, portable, header-only facilities that
belong to no one domain, and so to none of the other xstd libraries. Recognizing
a class template specialization, an empty type that keeps its place in a layout,
a data member that is there only under a condition, the portable spelling of
`[[no_unique_address]]`, and `to_underlying` over both a plain enum and a wrapped
one. It relies on the [C++23](https://wg21.link/N4950) standard and targets the
draft [C++29](https://wg21.link/std) standard library. All public APIs are in
namespace `xstd`.

The library is a leaf: nothing here includes `xstd::ints` or `xstd::bits`, which
is what lets a project take this one alone.

## Requirements

- A conforming [C++23](https://wg21.link/N4950) compiler
- CMake 3.28 or later when using the supplied CMake project
- No third-party runtime or library dependencies

## Add xstd-misc to a project

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
the same `xstd::misc` target.

## Headers

| Header | Additions | Description | Reference |
| :----- | :-------- | :---------- | :-------- |
| `<xstd/misc/concepts/specialization_of.hpp>` | `specialization_of` | Constraint form of `is_specialization_of` | [p2098r1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p2098r1.pdf) (relationship documented) |
| `<xstd/misc/type_traits/is_specialization_of.hpp>` | `is_specialization_of` <br> `is_specialization_of_v` | Is a type a specialization of a type-parameter-only class template? | [p2098r1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p2098r1.pdf) (relationship documented) |
| `<xstd/misc/type_traits/no_unique_address.hpp>` | `XSTD_NO_UNIQUE_ADDRESS` | Portable spelling of `no_unique_address` | none |
| `<xstd/misc/type_traits/empty_type.hpp>` | `empty_type` | A tagged empty type | none |
| `<xstd/misc/type_traits/conditional_data_member.hpp>` | `conditional_data_member` <br> `conditional_data_member_t` | A conditionally present member | none |
| `<xstd/misc/utility/to_underlying.hpp>` | `to_underlying` | `std::to_underlying`, plus an `std::integral_constant` overload | [p1682r1](https://wg21.link/p1682r1) (`std::to_underlying`) |

Each directory has an umbrella exporting what is under it -- `<xstd/misc/concepts.hpp>`,
`<xstd/misc/type_traits.hpp>`, `<xstd/misc/utility.hpp>` -- and `<xstd/misc.hpp>` exports
the three, so one include brings the whole surface.

## Examples

`to_underlying` is one spelling over both forms of an enum value: it forwards a
plain one to the standard's, and preserves a wrapped one at the type level.

```cpp
#include <xstd/misc/utility.hpp>
#include <type_traits>

enum class color : unsigned { red = 1 };

static_assert(xstd::to_underlying(color::red) == 1u);
static_assert(decltype(xstd::to_underlying(std::integral_constant<color, color::red>()))::value == 1u);
```

`empty_type` and `conditional_data_member_t` give a member that is present only
under a condition, and `XSTD_NO_UNIQUE_ADDRESS` costs it nothing when it is not.
The tag keeps two absent members distinct, which their layout would otherwise
collapse:

```cpp
#include <xstd/misc/type_traits.hpp>

template<bool Stateful>
struct storage {
    [[XSTD_NO_UNIQUE_ADDRESS]] xstd::conditional_data_member_t<Stateful, allocator_type, struct alloc> allocator;
    value_type value;
};

static_assert(sizeof(storage<false>) == sizeof(value_type));
```

Use `XSTD_NO_UNIQUE_ADDRESS` inside an attribute-specifier. It expands to
`msvc::no_unique_address` with the MSVC-compatible frontend, which keeps the
standard spelling layout-neutral, and to `no_unique_address` elsewhere.

`is_specialization_of` answers whether a type is a specialization of a class
template whose parameters are types, and `specialization_of` is the same question
where a constraint is what a caller writes:

```cpp
#include <xstd/misc/concepts.hpp>
#include <complex>

static_assert(xstd::is_specialization_of_v<std::complex<double>, std::complex>);
static_assert(xstd::specialization_of<std::complex<double>, std::complex>);
```

See [the design notes](doc/design.md) for rationale, and
[CONTRIBUTING.md](CONTRIBUTING.md) to build the library itself.

## Continuous integration

We continuously test the stable, qualification, and development branches of the
major [C++23](https://wg21.link/N4950) toolchains (compilers and standard
libraries) in both Debug and Release mode:

| Platform | Compiler   | Standard Library | Stable                    | Qualification             | Development                    | CI    |
| :------- | :--------- | :--------------- | :------------------------ | :------------------------ | :----------------------------- | :---- |
| Linux    | GCC        | libstdc++        | 15                        | 16                        | 17-SVN                         | [![GCC](https://github.com/rhalbersma/xstd-misc/actions/workflows/gcc.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/gcc.yml) |
| Windows  | MinGW      | libstdc++        | 15                        | 16                        | —                              | [![MinGW](https://github.com/rhalbersma/xstd-misc/actions/workflows/mingw.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/mingw.yml) |
| Linux    | Clang      | libstdc++        | 22 (libstdc++ 15)         | 23 (libstdc++ 16)         | 24-SVN (libstdc++ 17-SVN)      | [![Clang](https://github.com/rhalbersma/xstd-misc/actions/workflows/clang.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/clang.yml) |
| Linux    | Clang      | libc++           | 22                        | 23                        | 24-SVN                         | [![Clang-libc++](https://github.com/rhalbersma/xstd-misc/actions/workflows/clang-libc%2B%2B.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/clang-libc%2B%2B.yml) |
| macOS    | Apple Clang | libc++          | 17.0.0 (Xcode 16.4)       | 21.0.0 (Xcode 26.6)       | —                              | [![Apple Clang](https://github.com/rhalbersma/xstd-misc/actions/workflows/apple-clang.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/apple-clang.yml) |
| Windows  | Clang-CL   | MSVC             | 19.1.5 (VS 2022)          | 20.1.8 (VS 2026)          | 20.1.8 (VS 2026-Preview)       | [![Clang-CL](https://github.com/rhalbersma/xstd-misc/actions/workflows/clang-cl.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/clang-cl.yml) |
| Windows  | MSVC       | MSVC             | 2022 (17.11+)             | 2026                      | 2026-Preview                   | [![MSVC](https://github.com/rhalbersma/xstd-misc/actions/workflows/msvc.yml/badge.svg)](https://github.com/rhalbersma/xstd-misc/actions/workflows/msvc.yml) |

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
