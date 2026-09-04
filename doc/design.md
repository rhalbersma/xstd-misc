# Design notes

## Purpose

xstd-misc is a header-only collection of small C++ standard-library extensions
that belong to no one domain. There is no single proposal behind it: it collects
facilities suggested by existing practice that can be implemented portably, with
a standard-library interface. What is here is what the domain libraries kept
reaching for and none of them owns. The baseline is [C++23](https://wg21.link/N4950),
and may move to C++26 once that is common across the tested toolchains. Consumers
need no third-party dependencies.

## Principles

- **Prefer `constexpr`.** Value-oriented functions are usable during constant
  evaluation unless the standard library operation they delegate to prevents it.
- **Keep metaprogramming small.** `specialization_of`, `empty_type` and
  `conditional_data_member_t` solve local problems without a framework.
- **Stay a leaf.** No header here includes `<xstd/ints/...>` or `<xstd/bits/...>`.
  A library that everything may depend on can depend on nothing, or the split that
  gave each of them a repository would make a chain of them. `test/CMakeLists.txt`
  enforces this over every public header rather than trusting it.
- **Stay modular and dependency-free.** Linking `xstd::misc` adds include paths
  and the [C++23](https://wg21.link/N4950) requirement, but no runtime library or
  transitive package.

## API shape

### Traits and concepts

The type utilities intentionally remain narrow:

- `is_specialization_of` and `specialization_of` recognize specializations of
  class templates whose parameters are types.
- `empty_type` and `conditional_data_member_t` support optional
  `[[no_unique_address]]` storage. Its tag defaults to `void`, so `empty_type<>`
  also serves the uses with nothing to keep distinct.
- `to_underlying` forwards a plain enum and preserves one wrapped in
  `std::integral_constant`.

A concept spelling is provided when the standard library has an analogous
concept; otherwise the trait is the interface. Where a trait stands beside a
concept, the `is` is what marks which is which: `is_specialization_of` is the
trait and `specialization_of` the constraint, as `std::is_integral` stands beside
`std::integral`.

### Conditional storage

`empty_type` carries a tag because two empty members of the same type in one
layout are not required to share an address, and a class with two absent members
would otherwise be paying for one of them. The tag is a type the enclosing class
names, so nothing about the members' order or number is a layout question.

Its members exist for the class holding it rather than for its own sake: a
variadic constructor lets an enclosing class construct the member without a
value to construct it from, constrained so it never hijacks copy or move
construction, and a defaulted `operator<=>` lets that class default its own
comparisons over a member that has nothing to compare.

`XSTD_NO_UNIQUE_ADDRESS` is a macro rather than a portable attribute because
there is no portable spelling: MSVC keeps `[[no_unique_address]]` layout-neutral
for ABI reasons and puts the semantics behind `[[msvc::no_unique_address]]`. The
macro expands to the token, not to the brackets, so it reads as an attribute at
the point of use.

### `to_underlying`

The [original 2016 sketch](ideas.md#1-convenient-underlying-types-for-scoped-enums)
motivated `to_underlying` with scoped enums used as named tuple and array
indices. Rein Halbersma developed the idea and initial usage evidence with
Walter E. Brown; JeanHeyd Meneide then authored
[P1682R1](https://wg21.link/P1682R1) and carried `std::to_underlying` through
WG21 for [C++23](https://wg21.link/N4950). P1682's acknowledgements record those
roles. The xstd overload complements it: given an enum value wrapped in
`std::integral_constant`, it returns an `integral_constant` of the underlying
type, preserving the value at the type level.

A second overload forwards a plain enum to `std::to_underlying`, so `xstd::` is
one spelling over both forms rather than a name a caller has to remember to
switch away from for the unwrapped case. It is an overload rather than a
using-declaration because the constraint is then written where it applies, as
the wrapped one writes its own.

## Boost and include-cleaner

`misc-include-cleaner` is not asked about `boost/.*` at all, in the root
`.clang-tidy` and so in the test tree that inherits it. No Boost library ships
IWYU pragmas -- Boost.Hana has none across 450 headers -- and Boost.Test's macros
expand through private implementation headers, so the check reports that nothing
provides the names the tests write. The library itself includes no Boost; this is
about the test tree, and about any project that lints its own sources while using
Boost. No library can supply the line on their behalf: the option belongs to the
linter, and CMake carries no usage requirement that could propagate one.

## Requirements and evolution

xstd-misc requires a conforming [C++23](https://wg21.link/N4950) compiler, and the
CMake project CMake 3.28 or later, exporting the header-only `xstd::misc` target.
Consumers build neither the tests nor their dependencies. New facilities should
stay small, portable and motivated by current practice; one that enters the
standard library can be retired, and the baseline can advance to C++26 once that
is a practical default. A facility that grows a domain of its own belongs in a
library named for it rather than here.

## CI policy

The matrix follows the channel model used by
[apt.llvm.org](https://apt.llvm.org/): **stable** is the established release,
**qualification** the newest release still being qualified, and **development**
the current development branch. These are tested across the three main compiler
families, the three main standard libraries and the three main desktop platforms,
with MinGW, Apple Clang and Clang-CL filling out the remaining combinations.
Development jobs are required where a usable development compiler exists; the
README records the versions currently assigned to each channel. Building the
tests requires the dependencies documented in
[CONTRIBUTING.md](../CONTRIBUTING.md).
