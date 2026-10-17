# Design notes

## Purpose

xstd-misc is a header-only collection of small C++ standard-library extensions
that belong to no one domain. There is no single proposal behind it: it collects
facilities suggested by existing practice that can be implemented portably, with
a standard-library interface. What is here is what the domain libraries kept
reaching for and none of them owns. The baseline is [C++20](https://wg21.link/N4861),
and may move once a later standard is common across the tested toolchains. Consumers
need one third-party library, Boost.Hash2, which the hashing facilities are built on.

## Principles

- **Prefer `constexpr`.** Value-oriented functions are usable during constant
  evaluation unless the standard library operation they delegate to prevents it.
- **Keep metaprogramming small.** `specialization_of`, the two empty types and
  `conditional_data_member_t` solve local problems without a framework.
- **Stay a leaf.** No header here includes `<xstd/ints/...>` or `<xstd/bits/...>`.
  A library that everything may depend on can depend on nothing, or the split that
  gave each of them a repository would make a chain of them. `test/CMakeLists.txt`
  enforces this over every public header rather than trusting it.
- **Stay modular and header-only.** Linking `xstd::misc` adds include paths, the
  [C++20](https://wg21.link/N4861) requirement and the header-only `Boost::hash2`,
  but no runtime library; see [`hasher`](#hasher) for why Hash2 is a dependency
  rather than an opt-in.

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
omitting the `typename` before it relies on [P0634R3](https://wg21.link/P0634R3),
which Clang implemented only in 16, with no feature-test macro to branch on.
Writing the `typename` instead works everywhere, but
`readability-redundant-typename` then flags it while the C++20-compat warning
flags its absence; `conditional_t` is the spelling neither tool has an opinion
about. The two cannot drift: `conditional_t` is
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
lets a user choose the algorithm per container, and seed it per container. It descends from
[N3980](https://wg21.link/n3980)'s `uhash`, which default-constructs its algorithm on every call
and returns the whole result, as `digest` does; `hasher` adds the seeded prototype and the fold to
`std::size_t`. The paper sets `uhash` beside `std::hash` rather than in it, as this library sets
`hasher`: an algorithm parameter on `std::hash` would change the type of every
`std::hash<Key>`, and so the ABI of every unordered container over one, and a specialization per
key computes its value itself, so no algorithm could reach the key's bytes through it.

**Where it lives.** Under `functional/`, the directory mirroring `<functional>`, which holds
`std::hash`; the concept goes under `concepts/` and the avalanching trait
under `type_traits/`, as every other concept and trait here does. Each header holds one name,
so a consumer that needs only the concept or the trait does not pull in xxHash and XXH3.
Boost.Hash2 is therefore a dependency of the library rather than of an opt-in adapter: the
CMake target links `Boost::hash2`, the installed package config calls `find_dependency` for it,
and the vcpkg manifest lists it. It is header-only, so that costs an include path and no
runtime library, and xstd-bits, the library's main consumer, requires it already. An adapter
directory such as xstd-ints' `ext/boost/` would keep it optional, but would fence off from
`<xstd/misc.hpp>` the one facility here whose dependency every consumer has. `xstd::hasher` and
`xstd::hash_algorithm` name nothing in xstd-ints or xstd-bits, which share the namespace.

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
`std::hash`. Where a lookup should not build a key, [`hash`](#hash) names the key type and can be
transparent over the arguments that hash as it does.

**`digest`, a member.** The call operator folds the algorithm's result to `std::size_t`, which
a container needs and a stored digest must not have: its width would change with the target.
`digest` returns the result whole, as `digest_type`, from the same seeded prototype and the same
`hash_append`, so a table's hash of a key is the fold of that key's digest. A second class for
it would repeat the three constructors and the constraint to differ in one line, and would let
the two be seeded apart. It shares the hasher's default, xxHash64; a digest that persists names
its algorithm, so that no change of default could invalidate what was stored.

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
own -- then sees keys differing only in those bits collide. Which tables do depends on the
standard library: libstdc++ rounds a bucket count up to a prime and Boost.Unordered mixes the
hash again, which hide it, while MSVC's STL always masks a power-of-two count and libc++ masks
one whenever it is asked for a power of two. The tests insert sixteen IDs differing only in a
tag in their top four bits into a table asked for eight buckets: under a 64-bit FNV-1a all of
them share one bucket where the table masks, and on a 32-bit target `get_integral_result`'s
multiply-high fold to `std::size_t` spreads them again. That it hides on the most common
toolchain is what makes it easy to miss. Measured over 24-byte keys, FNV-1a has input/output bit pairs that never flip
together, where xxHash is at the sampling noise. FNV-1a is faster on a key of about a word,
by a few nanoseconds, and slower once keys reach a few words; xxHash consumes the input a word per lane with a constant finalization, so its
worst case is a fixed overhead on a key of a few bytes. There is no second alias for short
keys: a choice between a fast default and a safe one is the misuse this rules out, and a
caller who wants FNV-1a names `boost::hash2::fnv1a_64`. One algorithm on every platform
also makes the value one function of the key at each width of `size_t`: the 64-bit digest
itself where `size_t` holds it, and `get_integral_result`'s multiply-high fold of that digest
where it is 32 bits, which the tests pin with literal values. The cost is that a 32-bit CPU
emulates xxHash's 64-bit multiplies. The default is not SipHash either: a default-constructed hasher is unseeded, and keyed
resistance without a key buys nothing. Hash2's own advice for keys an adversary chooses is
`siphash_64` with a seed drawn per container, which is a choice the caller makes and
spells out.

**`is_avalanching`.** Boost.Unordered's open-addressing containers mix every hash value again
before using it, unless `boost::hash_is_avalanching` reports the hasher avalanching, which it
reads from a nested `is_avalanching`. `hasher<H>` declares it from
`hash_algorithm_is_avalanching<H>`, a public trait, because Hash2 records no such property on
its algorithms and a user with an algorithm of their own has a reason to state it. The trait is
true for every Hash2 algorithm but FNV-1a, and false for anything it is not told about. It errs
that way because a table that trusts a biased hash places keys by bits that ignore part of them,
while one that mixes an avalanching hash again pays only a multiply per lookup.

Every translation unit that sees the hasher sees every answer. The trait's header names each
Hash2 algorithm through a forward declaration rather than its header, so the answers cost no
include, and an answer cannot depend on which headers a translation unit happened to include.
That matters beyond speed: an explicit specialization that one translation unit sees and another
does not is ill-formed, no diagnostic required, and two translation units that disagree on a
hasher's `is_avalanching` inline two different probe sequences into one container, so a lookup
compiled in one misses an element inserted by the other. Answers given per algorithm header,
opted into by including ours instead of Hash2's, would have put that one include away from every
user. The forward declarations match Hash2's own, `class` in `boost::hash2` for each algorithm
and `template<class H> class hmac`, from 1.88 to 1.92, so a change there fails to compile rather
than going unnoticed. HMAC answers as the algorithm it keys does. An algorithm outside Hash2 gets
its answer from a specialization its author writes beside its definition, as with `std::hash`.

The tests measure the Strict Avalanche Criterion (Webster and Tavares, CRYPTO '85) through the
hasher's whole path: the seeded prototype, `hash_append` of a random 64-bit key, and the fold to
`std::size_t`, and they check the trait against it for every Hash2 algorithm. For every key bit
and hash bit, the rate at which flipping the one flips the other must lie within Hoeffding's
bound of ½, union-bounded over every pair at a false-failure probability of 10⁻⁶. The algorithms
a table runs are measured over 4096 keys, where the bound is about 0.053; the cryptographic ones
avalanche by design and cost far more per hash, so 256 keys check them coarsely, against a bound
of about 0.21. Measured over 4096 keys, every algorithm but FNV-1a stays near 0.03, the sampling
noise of an ideal hash, at either width of `std::size_t`; FNV-1a comes close to 0.5, some hash bit
almost never flipping with some key bit. In a `boost::unordered_flat_set` of 16384 sequential,
strided or common-prefix keys, xxHash, XXH3 and SipHash have the same probe lengths without the
mix as with it, while FNV-1a declared avalanching lengthens insertion probes by five to eight
percent. That second measurement is evidence rather than a test: probes in an
open-addressing table are not independent, so no threshold on it is calibrated. SMHasher3 gives
the fuller verdict, and once Hash2 publishes the property the hasher reads it instead.

**`constexpr`, `noexcept`.** All three constructors and the call operator are `constexpr`.
The constructors are constant expressions for every algorithm outside `legacy/`, which the
tests assert. The call operator is not yet one for any algorithm: Hash2's
`get_integral_result` is not `constexpr` as of 1.92.
The specifier costs nothing in a template and takes effect as soon as Hash2's does;
reimplementing the fold here to evaluate it now would fork Hash2's mapping, which a hasher
built on Hash2 must not. Nothing is `noexcept`: Hash2 declares no exception specification
on its algorithms, and the `hash_append` of a key is the user's own code, so a written
guarantee would be a promise about code this header does not see.

### `hash`

`xstd::hash<T, H>` is `hasher<H>` at one key type, spelled as `std::hash<T>` is, so a reader who
knows the standard's finds the key where they expect it. Its call operator and `digest` take a
`T const&`, so every argument converts to the key type before it is hashed: `hash<std::int64_t>`
hashes `42` as the 8 bytes of the key a table holds, where `hasher` hashes the 4 bytes of an
`int`. It derives privately from `hasher<H>`, inheriting the three constructors and their
`explicit`, and exposing `digest_type` and `is_avalanching`; it is not a `hasher`, so it does not
convert to one.

The key type is what makes transparency sound. A heterogeneous lookup hashes the argument it is
given, so every argument a transparent hash accepts must append the bytes the key would for an
equal value. Over a `std::basic_string` the call takes its `std::basic_string_view`: a string, a
view and a C string all convert to it, and Hash2 appends a string and its view alike, so they
hash equal under every algorithm, as the tests check. That specialization declares
`is_transparent`, and a container pairing it with `std::equal_to<>` finds a string key by a view
or a literal without building a string. No other key is transparent: a `hasher` that did not
know its key could not be, and a key whose equal values span types Hash2 appends differently,
such as the integers, would miss.

## Boost and include-cleaner

`misc-include-cleaner` is not asked about `boost/.*` at all, in the root
`.clang-tidy` and so in the test tree that inherits it. No Boost library ships
IWYU pragmas -- Boost.Hana has none across 450 headers -- and Boost.Test's macros
expand through private implementation headers, so the check reports that nothing
provides the names the tests write. Of Boost, the library includes only Hash2; this is
about the test tree, about the hashing headers, and about any project that lints its own sources while using
Boost. No library can supply the line on their behalf: the option belongs to the
linter, and CMake carries no usage requirement that could propagate one.

## Requirements and evolution

xstd-misc requires a conforming [C++20](https://wg21.link/N4861) compiler, and the
CMake project CMake 3.30 or later, exporting the header-only `xstd::misc` target.
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
