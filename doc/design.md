# Design notes

## Purpose

xstd-misc is a header-only collection of small C++ standard-library extensions
that belong to no one domain. There is no single proposal behind it: it collects
facilities suggested by existing practice that can be implemented portably, with
a standard-library interface. What is here is what the domain libraries kept
reaching for and none of them owns. The baseline is [C++20](https://wg21.link/N4861),
and may move once a later standard is common across the tested toolchains. Consumers
need no third-party dependencies, unless they include a header under `ext/`, which
adapts the one library it names.

## Principles

- **Prefer `constexpr`.** Value-oriented functions are usable during constant
  evaluation unless the standard library operation they delegate to prevents it.
- **Keep metaprogramming small.** `specialization_of`, the two empty types and
  `conditional_data_member_t` solve local problems without a framework.
- **Stay a leaf.** No header here includes `<xstd/ints/...>` or `<xstd/bits/...>`.
  A library that everything may depend on can depend on nothing, or the split that
  gave each of them a repository would make a chain of them. `test/CMakeLists.txt`
  enforces this over every public header rather than trusting it.
- **Stay modular and dependency-free.** Linking `xstd::misc` adds include paths
  and the [C++20](https://wg21.link/N4861) requirement, but no runtime library or
  transitive package. A header that cannot keep this, because what it adds is an
  adapter over a third-party library, goes under `ext/` and is asked for by name;
  see [`hasher`](#hasher).

## API shape

### Traits and concepts

The type utilities intentionally remain narrow:

- `is_specialization_of` and `specialization_of` recognize specializations of
  class templates whose parameters are all types, under p2098's spelling. Only that
  shape is offered. The kinds are part of a template's type and no one template
  template parameter binds them all, so every other shape needs its own name, and
  the one consumer that had a template with a value parameter is better served by a
  two-line trait of its own than by a family of suffixed names it uses one of. The
  trait is the exact question; the concept strips a `const`, because an adaptor over
  a const owner names `Container const`.
- `simple_allocator` and `container_compatible_range` are the standard's
  exposition-only *simple-allocator* ([allocator.requirements.general]) and
  *container-compatible-range* ([container.intro.reqmts]), spelled out word for word
  so that a container outside the standard library can constrain its allocator
  arguments and `from_range` constructors exactly as the standard's containers are
  specified to.
- `empty_member_type` and `conditional_data_member_t` support optional
  `[[no_unique_address]]` storage, and `empty_base_type` is the same idea for a
  base class. Both tags default to `void`, so `empty_member_type<>` and
  `empty_base_type<>` serve the uses with nothing to keep distinct.
- `to_underlying` forwards a plain enum and preserves one wrapped in
  `std::integral_constant`.

A concept spelling is provided when the standard library has an analogous
concept; otherwise the trait is the interface. Where a trait stands beside a
concept, the `is` is what marks which is which: `is_specialization_of` is the
trait and `specialization_of` the constraint, as `std::is_integral` stands beside
`std::integral`.

The two are not quite one predicate, and the difference is deliberate. A trait
answers exactly: `is_specialization_of_v<std::vector<int> const, std::vector>` is
false, a const-qualified type being no specialization of anything. A constraint is
written for what a caller may name, and an adaptor over a const owner names
`Container const` -- so the concepts strip the const and the traits do not. Only the
const: a reference is not a specialization under either spelling, and nothing else
comes off. xstd-bits draws the same line in its own nominal concept, whose comment
puts it exactly: the const comes off here and nowhere else.

### Conditional storage

`empty_member_type` carries a tag because two empty members of the same type in
one layout are not required to share an address, and a class with two absent
members would otherwise be paying for one of them. The tag is a type the
enclosing class names, so nothing about the members' order or number is a layout
question. `empty_base_type` carries one for a different reason: a class cannot
derive from the same base twice.

The two are separate types because a comparison behaves differently in the two
positions. `empty_member_type` has a defaulted `operator<=>` so that an enclosing
class can default its own comparisons over the member, and that is safe: a
member's associated classes are not the enclosing class's, so the hidden friend
is invisible to it. A base's associated classes ARE the derived class's, so the
same defaulted comparison would be found by ADL for every derived object and
would answer *equal* for any two of them, having only the empty base to compare.
`empty_base_type` therefore carries nothing.

The price is exact: a derived class cannot default its own comparisons over a
base that has none, the defaulted operator being defined as deleted. So
`empty_base_type` serves the incomparable case. A class wanting an empty base and
defaulted comparisons wants a base that carries them, which is a different type
and belongs where it is used.

Its members exist for the class holding it rather than for its own sake: a
variadic constructor lets an enclosing class construct the member without a
value to construct it from, constrained so it never hijacks copy or move
construction, and a defaulted `operator<=>` lets that class default its own
comparisons over a member that has nothing to compare.

`conditional_data_member_t` is spelled with `std::conditional_t` rather than as
`conditional_data_member<...>::type`. The latter is a dependent `::type`, and
omitting the `typename` before it is [P0634R3](https://wg21.link/P0634R3), which
Clang did not implement until 16 -- so that spelling would set the library's
Clang floor at 16 though nothing else in it needs more than 11. There is no
feature-test macro for P0634R3 to branch on. Writing the `typename` instead
would work everywhere, but `readability-redundant-typename` then flags it while
the C++20-compat warning flags its absence; `conditional_t` is the spelling
neither tool has an opinion about. The two cannot drift: `conditional_t` is
specified as `typename conditional<...>::type`.

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

A second overload takes a plain enum, so `xstd::` is one spelling over both
forms rather than a name a caller has to remember to switch away from for the
unwrapped case. It is an overload rather than a using-declaration because the
constraint is then written where it applies, as the wrapped one writes its own.

It performs the cast rather than calling `std::to_underlying`, which would put
the library's baseline at [C++23](https://wg21.link/N4950) for one function. The
cast is not an approximation of that function: P1682R1's Returns clause is
`static_cast<underlying_type_t<T>>(value)` and nothing more, so this is the
standard's own wording rather than a reimplementation of it. Selecting between
the two on `__cpp_lib_to_underlying` was considered and dropped: it would buy
nothing an identity can give, at the price of a header included on some builds
and not others, and a branch only one CI dimension ever compiles.

The wrapped overload calls the plain one rather than repeating the cast. The
call is qualified: unqualified, an enum's own namespace could supply a
`to_underlying` that ADL would prefer.

Writing the cast out puts it where a linter can see it, which delegating did
not. Instantiated for an enum whose underlying type is `bool` and which has no
enumerators, it becomes a cast to `bool`, and
`bugprone-non-zero-enum-to-bool-conversion` reports that as always true; a
value-initialized one converts to `false`, so it is not. The suppression sits on
that line rather than in `.clang-tidy`, so it reaches a consumer linting their
own code -- their configuration is not ours to fix.

### `hasher`

`xstd::hasher<H>` is the hasher the
[Boost.Hash2 documentation](https://www.boost.org/doc/libs/release/libs/hash2/)
sketches for an unordered container: it holds a prototype of the algorithm `H`, seeded
once, and copies it on every call, so that `hash_append` of the key starts from the same
state each time and a `const` call operator mutates nothing. Hash2 separates *what* a type
hashes, its `tag_invoke` hook, from *which algorithm* hashes it; this is the piece that
lets a user choose the algorithm per container, and seed it per container.

**Where it lives.** `<functional>`, which holds `std::hash`, would be the mirrored
directory, but every mirrored directory is exported by `<xstd/misc.hpp>`, and a hasher
over Boost.Hash2 there would make Boost.Hash2 a dependency of every consumer of that
umbrella. It goes under `ext/boost/` instead, the layout xstd-ints uses for its adapters
(after Boost.Hana's `boost/hana/ext/`): one directory per adapted library, one header per
adapted upstream library, an umbrella `<xstd/misc/ext/boost.hpp>` and none above it. Nothing
else includes them, so `xstd::misc` stays dependency-free and the CMake target links nothing;
a consumer who includes `<xstd/misc/ext/boost/hash2.hpp>` links `Boost::hash2` themselves, as
an xstd-ints consumer of `<xstd/ints/ext/boost/int128.hpp>` links `Boost::int128`. The
installed package config therefore calls no `find_dependency` for it. The vcpkg manifest
carries a `hash2` feature for a consumer who wants vcpkg to resolve it, and the `test`
feature installs it with Boost.Unordered for the suite. `xstd::hasher` and
`xstd::hash_algorithm` name nothing in xstd-ints or xstd-bits, which share the namespace. `hash_algorithm` sits in the same header rather than under `concepts/`, which
`<xstd/misc.hpp>` exports: it names Hash2's `has_constant_size`, so it carries the dependency
the hasher does.

**No key type.** The key is a parameter of the call, not of the class, as with the
standard's transparent function objects: one `hasher<H>` serves an `int` key, a string key
and a key of a user's own type, each hashed as `hash_append` hashes it. `std::hash<T>` fixes
its key because a specialization per key is its customization point; Hash2's is
`tag_invoke`, which the call reaches for whatever it is given, so a key type on the class
would only make one hasher type per key for the same seeded algorithm.

**Not transparent.** A hasher without a key type looks like one that could serve
heterogeneous lookup, but it declares no `is_transparent`. Heterogeneous lookup needs equal
values to hash equal across their types, and `hash_append` hashes a value in the bytes of
its own type: `42` appends four bytes and `std::int64_t{42}` eight, so they compare equal
and hash apart, and a lookup by one would miss a key stored as the other. A container over
a `hasher` therefore converts a lookup argument to its key type first, as it would for
`std::hash`.

**The call is unconstrained.** A requires-expression that `hash_append(h, {}, v)` is
well-formed would read as a constraint and check nothing: Hash2 declares `hash_append` with
a `void` return and no constraint for every `T`, and rejects a key it cannot hash inside its
body, where overload resolution among its own `detail` functions fails. Spelling those
`detail` functions in a constraint would bind this header to Hash2's internals, and
reproducing their conditions would fork Hash2's dispatch, which a hasher built on Hash2 must
not. A key Hash2 cannot hash is therefore rejected where Hash2 rejects it, at the call's
instantiation; the constraint belongs in Hash2, and the call operator can take it up once
`hash_append` has it.

**`hash_algorithm`.** The algorithm parameter is constrained by a public concept, because a
user has a reason to name it: to check that an algorithm they wrote conforms, and to
constrain their own templates on one. It states Boost.Hash2 1.92's documented requirements
for a hash algorithm, and only those, since a concept stricter or looser than Hash2's own
contract would mislead the user it answers:

- `std::semiregular`, for the default constructor, copy construction and copy assignment;
- a constructor from a `std::uint64_t` seed;
- a constructor from a byte seed `(unsigned char const*, std::size_t)`;
- `update(void const*, std::size_t)` on a non-`const` object;
- `result()` on a non-`const` object, returning `result_type`;
- `result_type` an unsigned integer type other than `bool`, or an array-like type of
  `unsigned char` whose size is fixed at compile time, as Hash2's `has_constant_size` reports;
- `block_size`, where it is declared at all, of type `std::size_t`.

Three of those lines are drawn where the documentation is ambiguous. The byte seed is
documented as `(void const*, std::size_t)` in the synopsis and as "a seed sequence of
`unsigned char` values" in the prose. The legacy `murmur3_32`, `murmur3_128` and
`spooky2_128` have only the `unsigned char const*` form, which every other algorithm also
has, and a `void const*` constructor accepts an `unsigned char const*` argument, so the
concept asks for that one. `block_size` is optional, required only of an algorithm passed to
`hmac`, so an algorithm without one satisfies the concept, and one that declares it must
declare it as the documentation does. `update`'s return type and the `explicit` on the seed
constructor are left free: Hash2 reads neither, so requiring them would reject an algorithm
Hash2 runs. Since every algorithm has both seeded constructors, so does every `hasher`, with
no constraint of its own; the `std::uint64_t` one is `explicit`, as Hash2's are, so an
integer never becomes a hasher by conversion.

The concept does not ask for what `get_integral_result`, which folds a result into
`std::size_t`, asks of an array-like result beyond Hash2's requirements: at least eight bytes.
No algorithm in Hash2 has a shorter one, and an algorithm that did would satisfy Hash2's
contract and still fail in Hash2's own fold.

**The default algorithm.** The default `H` is `boost::hash2::xxhash_64`, on every platform
and for every key, and the library names no other. Easy to use and hard to misuse rules out
a default that is biased: FNV-1a multiplies after each byte, and a multiply only carries
upward, so the high bits of the last byte never reach the low bits of the result. A table
that takes the low bits as they come -- a power-of-two bucket count with no mixing of its
own -- then sees keys differing only in those bits collide. The standard containers' prime
bucket counts and Boost.Unordered's post-mix hide this, which is what makes it easy to
miss. Measured over 24-byte keys, FNV-1a has input/output bit pairs that never flip
together, where xxHash is at the sampling noise. FNV-1a is faster on a key of about a word,
by a few nanoseconds, and slower once keys reach a few words; xxHash consumes the input a word per lane with a constant finalization, so its
worst case is a fixed overhead on a key of a few bytes. There is no second alias for short
keys: a choice between a fast default and a safe one is the misuse this rules out, and a
caller who wants FNV-1a names `boost::hash2::fnv1a_64`. One algorithm on every platform
also makes the value the same on 32- and 64-bit targets, `get_integral_result` folding it
into a narrower `size_t`; the cost is that a 32-bit CPU emulates xxHash's 64-bit
multiplies. The default is not SipHash either: a default-constructed hasher is unseeded, and keyed
resistance without a key buys nothing. Hash2's own advice for keys an adversary chooses is
`siphash_64` with a seed drawn per container, which is a choice the caller makes and
spells out.

**`constexpr`, `noexcept`.** All three constructors and the call operator are `constexpr`.
The constructors are constant expressions for every algorithm outside `legacy/`, which the
tests assert. The call operator is not yet one for any algorithm: Hash2's
`get_integral_result` is not `constexpr` as of 1.92.
The specifier costs nothing in a template and takes effect as soon as Hash2's does;
reimplementing the fold here to evaluate it now would fork Hash2's mapping, which a hasher
built on Hash2 must not. Nothing is `noexcept`: Hash2 declares no exception specification
on its algorithms, and the `hash_append` of a key is the user's own code, so a written
guarantee would be a promise about code this header does not see.

## Boost and include-cleaner

`misc-include-cleaner` is not asked about `boost/.*` at all, in the root
`.clang-tidy` and so in the test tree that inherits it. No Boost library ships
IWYU pragmas -- Boost.Hana has none across 450 headers -- and Boost.Test's macros
expand through private implementation headers, so the check reports that nothing
provides the names the tests write. Outside `ext/`, the library includes no Boost; this is
about the test tree, about the adapters there, and about any project that lints its own sources while using
Boost. No library can supply the line on their behalf: the option belongs to the
linter, and CMake carries no usage requirement that could propagate one.

## Requirements and evolution

xstd-misc requires a conforming [C++20](https://wg21.link/N4861) compiler, and the
CMake project CMake 3.28 or later, exporting the header-only `xstd::misc` target.
Consumers build neither the tests nor their dependencies. New facilities should
stay small, portable and motivated by current practice; one that enters the
standard library can be retired, and the baseline can advance once a later
standard is a practical default. A facility that grows a domain of its own belongs in a
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
