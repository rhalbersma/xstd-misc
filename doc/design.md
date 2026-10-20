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
  but no runtime library; see [`hash`](#hash) for why Hash2 is a dependency
  rather than an opt-in.

## API shape

### Traits and concepts

The type utilities intentionally remain narrow:

- `is_specialization_of` and `specialization_of` recognize specializations of
  class templates whose parameters are all types, under [P2098](https://github.com/cplusplus/papers/issues/812)'s spelling. Only that
  shape is offered. The kinds are part of a template's type and no one template
  template parameter binds them all, so every other shape needs its own name, and
  the one consumer that had a template with a value parameter is better served by a
  two-line trait of its own than by a family of suffixed names it uses one of. The
  trait is the exact question; the concept strips a `const`, because an adaptor over
  a const owner names `Container const`.
- `simple_allocator` and `container_compatible_range` are the standard's
  exposition-only *simple-allocator* ([[allocator.requirements.general]/99](https://eel.is/c++draft/allocator.requirements.general#99)) and
  *container-compatible-range* ([[container.intro.reqmts]/2](https://eel.is/c++draft/container.intro.reqmts#2)), spelled out word for word
  so that a container outside the standard library can constrain its allocator
  arguments and `from_range` constructors exactly as the standard's containers are
  specified to. Each header cites the working draft and paragraph it copies, and
  `tools/check-draft-wording.py` checks the copy against that draft, token for token.
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
[P1682R1](https://github.com/cplusplus/papers/issues/460) and carried `std::to_underlying` through
WG21 for [C++23](https://wg21.link/N4950). [P1682](https://github.com/cplusplus/papers/issues/460)'s acknowledgements record those
roles. The xstd overload complements it: given an enum value wrapped in
`std::integral_constant`, it returns an `integral_constant` of the underlying
type, preserving the value at the type level.

A second overload takes a plain enum, so `xstd::` is one spelling over both
forms rather than a name a caller has to remember to switch away from for the
unwrapped case. It is declared as the standard declares `std::to_underlying`, with
no constraint of its own: `underlying_type_t<Enum>` in its return type already makes
any other type a substitution failure. The wrapped overload deduces its return type,
so it writes its constraint.

It performs the cast rather than calling `std::to_underlying`, which would put
the library's baseline at [C++23](https://wg21.link/N4950) for one function. The
cast is not an approximation of that function: its Returns element,
[[utility.underlying]/1](https://eel.is/c++draft/utility.underlying#1), is `static_cast<underlying_type_t<T>>(value)` and nothing
more, so this is the standard's own wording rather than a reimplementation of it.
`tools/check-draft-wording.py` holds the cast to that paragraph of the working
draft the header cites. Selecting between
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

### `hash`

`xstd::hash<T, H>` is the hasher the
[Boost.Hash2 documentation](https://www.boost.org/doc/libs/latest/libs/hash2/)
sketches for an unordered container: it holds a prototype of the algorithm `H`, seeded
once, and copies it on every call, so that `hash_append` of the key starts from the same
state each time and a `const` call operator mutates nothing. Hash2 separates *what* a type
hashes, its `tag_invoke` hook, from *which algorithm* hashes it; this is the piece that
lets a user choose the algorithm per container, and seed it per container. Its call returns
`std::size_t`, as the standard's [*Cpp17Hash*](https://eel.is/c++draft/hash.requirements) requirements and [Boost.Unordered](https://www.boost.org/doc/libs/latest/libs/unordered/) ask, through
Hash2's [`get_integral_result`](https://www.boost.org/doc/libs/latest/libs/hash2/doc/html/hash2.html#ref_get_integral_result). It descends from [N3980](https://wg21.link/n3980)'s `uhash`, which
takes its key per call, default-constructs its algorithm on every one and returns the whole
result. The paper sets `uhash` beside `std::hash` rather than in it, as this library sets `hash`:
an algorithm parameter on `std::hash` would change the type of every `std::hash<Key>`, and so the
ABI of every unordered container over one, and a specialization per key computes its value
itself, so no algorithm could reach the key's bytes through it.

**What the copy costs.** Copying `xxhash_64`'s 80-byte prototype takes about 1 ns of an 11–18 ns call, and starting
from a default-constructed or freshly seeded algorithm costs as much as the whole call, so the copy is the cheapest
start a seeded call has. `xxh3_128`'s 560-byte state makes its copy a third or more of the call, and still the
cheapest start. [The measurements](measurements.md#the-prototype-copy) give the method and the figures.

**Its successors.** Two later papers took the problem elsewhere. [P0029R0](https://wg21.link/P0029R0)
unifies it with [N3333](https://wg21.link/n3333): a type's `hash_value` takes the algorithm's state by value and returns it
through `hash_combine`, the algorithm gets that broad interface rather than the bytes `uhash`'s algorithms take, and
`std::hash`'s primary template calls `hash_value`, so a type becomes hashable in every container
without naming a hasher. It permits other algorithms but expects most users never to need one, warns that
the choice may prove a foot-gun, and asks that the default be randomized at program startup; it states
that `hash_value`'s contract is not strong enough for fingerprinting or serialization. Abseil's
`absl::Hash` has that shape, an `AbslHashValue(H, T const&)` returning `H::combine(...)`, and its
values change between runs. This library follows [N3980](https://wg21.link/n3980) and Hash2 on both counts the paper argues
against. The algorithm is the user's, named and seeded per container, because containers differ in
what they need: a secretly seeded SipHash where an adversary chooses the keys, and xxHash where
none does. And the bytes `hash_append` writes are fixed by the flavor, which is what lets `digest`
persist. A library outside the standard cannot extend `std::hash`, so a container names
`xstd::hash`, the explicit choice that paper observes users are reluctant to make.
[P0814R2](https://github.com/cplusplus/papers/issues/253) proposes `std::hash_combine(args...)`,
Boost's `hash_combine` as a standard function: it hashes each argument through `std::hash` to a
`size_t` and mixes the results, and promises the same value only within one execution. That
reduces each member to a `size_t` before the next begins, the repeated step [P0029R0](https://wg21.link/P0029R0) names as what
makes composing `std::hash` inefficient; `hash_append` feeds every member to one algorithm state
instead, so the library has no `hash_combine`.

**Where it lives.** Under `functional/`, the directory mirroring `<functional>`, which holds
`std::hash`, beside [`digest`](#digest); the concept goes under `concepts/` and the avalanching
trait under `type_traits/`, as every other concept and trait here does. Each header holds one
name, so a consumer that needs only the concept or the trait does not pull in xxHash and XXH3.
Boost.Hash2 is therefore a dependency of the library rather than of an opt-in adapter: the
CMake target links `Boost::hash2`, the installed package config calls `find_dependency` for it,
and the vcpkg manifest lists it. It is header-only, so that costs an include path and no
runtime library, and xstd-bits, the library's main consumer, requires it already. An adapter
directory such as xstd-ints' `ext/boost/` would keep it optional, but would fence off from
`<xstd/misc.hpp>` the one facility here whose dependency every consumer has. `xstd::hash`,
`xstd::digest` and `xstd::hash_algorithm` name nothing in xstd-ints or xstd-bits, which share
the namespace.

**A key type.** The key is a parameter of the class, as in `std::hash<T>`, so a reader who knows
the standard's finds it where they expect it, and the call takes a `T const&`. Every argument
therefore converts to the key type before it is hashed: `hash<std::int64_t>` hashes `42` as the
8 bytes of the key a table holds. `uhash` takes its key per call instead, and so hashes `42` as the
4 bytes of an `int`, and misses the key. A container always has a key type, and generic code
knows it wherever it hashes, so a key-agnostic hasher would only keep that pitfall within reach.
The seeded prototype, its copy and its fold sit in `detail::hasher<H>`, a private base of both
`hash` and `digest`, which inherit its three constructors and their `explicit`.

**Transparent over strings only.** Heterogeneous lookup hashes the argument it is given, so every
argument a transparent hash accepts must append the bytes the key would for an equal value.
`hash_append` hashes a value in the bytes of its own type: `42` appends four bytes and
`std::int64_t{42}` eight, so they compare equal and hash apart, and a transparent hash over the
integers would miss a key stored as the other. Over a `std::basic_string` the call takes its
`std::basic_string_view` instead: a string, a view and a C string all convert to it, and Hash2
appends a string and its view alike, so they hash equal under every algorithm, as the tests
check. That specialization declares `is_transparent`, and a container pairing it with
`std::equal_to<>` finds a string key by a view or a literal without building a string. Every
other key converts a lookup argument to the key type first, as it would for `std::hash`.

**The call is unconstrained.** A requires-expression that `hash_append(h, {}, v)` is
well-formed would read as a constraint and check nothing: Hash2 declares `hash_append` with
a `void` return and no constraint for every `T`, and rejects a key it cannot hash inside its
body, where overload resolution among its own `detail` functions fails. Spelling those
`detail` functions in a constraint would bind this header to Hash2's internals, and
reproducing their conditions would fork Hash2's dispatch, which a hash built on Hash2 must
not. A key Hash2 cannot hash is therefore rejected where Hash2 rejects it, at the call's
instantiation; the constraint belongs in Hash2, and the call operator can take it up once
`hash_append` has it.

**`hash_algorithm`.** The algorithm parameter is constrained by a public concept, because a
user has a reason to name it: to check that an algorithm they wrote conforms, and to
constrain their own templates on one. It states Boost.Hash2 1.92's [documented requirements](https://www.boost.org/doc/libs/latest/libs/hash2/doc/html/hash2.html#hashing_bytes_hash_algorithm_requirements)
for a hash algorithm, and only those, since a concept stricter or looser than Hash2's own
contract would mislead the user it answers:

- `std::semiregular`, for the default constructor, copy construction and copy assignment;
- a constructor from a `std::uint64_t` seed;
- a constructor from a byte seed `(unsigned char const*, std::size_t)`;
- `update(void const*, std::size_t)` on a non-`const` object;
- `result()` on a non-`const` object, returning `result_type`;
- `result_type` an unsigned integer type other than `bool`, or an array-like type of
  `unsigned char` whose size is fixed at compile time, as Hash2's [`has_constant_size`](https://www.boost.org/doc/libs/latest/libs/hash2/doc/html/hash2.html#ref_has_constant_size) reports;
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
Hash2 runs. Since every algorithm has both seeded constructors, so do every `hash` and `digest`, with
no constraint of its own; the `std::uint64_t` one is `explicit`, as Hash2's are, so an
integer never becomes one by conversion.

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
tag in the high four bits of the byte `hash_append` writes last, the ID's top four bits on a
little-endian target and bits 4 to 7 on a big-endian one, into a table asked for eight buckets,
so both targets hash the same bytes: under a 64-bit FNV-1a all of
them share one bucket where the table masks, and on a 32-bit target `get_integral_result`'s
multiply-high fold to `std::size_t` spreads them again. A `boost::unordered_flat_set` takes a
group from a hash's high bits instead, and FNV-1a leaves those alike for the same IDs too: the
last byte meets a single multiply, which carries its high bits only part of the way up. The
table mixes FNV-1a's hash before taking the group, and no insertion then probes past its first. That it hides on the most common
toolchain is what makes it easy to miss. FNV-1a also has input and output bit pairs that never flip together, where
xxHash is at the sampling noise ([measurements](measurements.md#avalanche)). It is faster only on keys of up to 16
bytes, and several times slower from 64, while xxHash consumes the input a word per lane with a constant
finalization, so its cost through `hash` is nearly flat up to 64 bytes ([measurements](measurements.md#the-cost-of-a-call)).
There is no second alias for short
keys: a choice between a fast default and a safe one is the misuse this rules out, and a
caller who wants FNV-1a names `boost::hash2::fnv1a_64`. One algorithm on every platform
also makes the value one function of the key at each width of `size_t`: the 64-bit digest
itself where `size_t` holds it, and `get_integral_result`'s multiply-high fold of that digest
where it is 32 bits, which the tests pin with literal values. A 32-bit CPU emulates xxHash's
64-bit multiplies, and still pays no more for them than for `xxhash_32`'s
([measurements](measurements.md#on-a-32-bit-target)).

**A second algorithm.** `hash<T, H64, H32>` runs `H32` where `std::size_t` is at most 32 bits and
`H64` elsewhere, and names the one that runs `hash_type`, after the hasher in
[Hash2's documentation](https://www.boost.org/doc/libs/latest/libs/hash2/doc/html/hash2.html#example_use_with_unordered_containers).
It serves a family whose two members differ in kind rather than in speed, such as `siphash_64` and
`siphash_32`, HalfSipHash, which is keyed for a 32-bit word. `H32` defaults to `H64`, so one
algorithm named alone runs on every target, and the default pair is `xxhash_64` twice, which the
measurement above leaves no reason to split. The choice follows the width of `size_t` rather than,
as in Hash2's example, whether `H64`'s result matches it, so the first slot runs wherever `size_t`
is 64 bits even for an algorithm wider than that, such as `xxh3_128`. A requires-clause asks that `H32`'s result
be no wider than `H64`'s: two names read as a pair are easily given in the other order, and would
then run the wider algorithm where `size_t` is the narrower. `Flavor` comes after both, so code
that names one spells the algorithm twice, as `hash<T, H, H, Flavor>`.

The default is not SipHash either: a default-constructed hasher is unseeded, and keyed
resistance without a key buys nothing. Hash2's own advice for keys an adversary chooses is
`siphash_64` with a seed drawn per container, which is a choice the caller makes and
spells out.

**Flooding.** The seed is what defeats an attacker who knows the hash function. Keys found offline to share a slot
slow insertion into a `boost::unordered_flat_map` and a `std::unordered_map` by one to two orders of magnitude under an
unseeded or publicly seeded hash, whatever the algorithm, and a secret seed per container leaves both tables as they
are with random keys, under SipHash and under xxHash ([measurements](measurements.md#hash-flooding)). SipHash's half
of the advice answers what that search does not attempt: collisions found independently of the seed, as Aumasson and
Bernstein found for MurmurHash and CityHash in 2012.

**xxHash or SipHash.** The two halves of that advice trade speed for what the seed protects. In
[SMHasher3](https://gitlab.com/fwojcik/smhasher3/-/tree/main/results), XXH-64 fails nine of its 250 tests, all of them
seed tests, and passes every test that hashes keys under one seed, while SipHash-2-4 passes every test it runs, at
about three times the cost on short keys ([measurements](measurements.md#xxhash-and-siphash-in-smhasher3)).

- **What that means here:** a container hashes every key under its one seed, so none of XXH-64's failures reaches a
  table. They show that its seed is an offset rather than a key. Nothing in it resists an attacker who learns hashes
  and works back towards the seed, or who looks for collisions that hold under every seed, and SipHash is built to
  resist both.
- **SipHash's key:** SipHash takes a 128-bit key, and Hash2's `siphash_64(std::uint64_t)` fills only its low half,
  which leaves an attacker 64 bits to guess. Seeded from 16 bytes, as `hash<T, siphash_64>(p, 16)`, it takes the whole
  key, as the reference does.

So the default stays `xxhash_64`: one seed per container never meets its failures, and on short
keys it takes a third of SipHash's time. A container whose keys come from outside the program, such
as request headers or user names, names `siphash_64` and seeds it with 16 bytes from a random
device, as the README's example does.

**`is_avalanching`.** Boost.Unordered's open-addressing containers [mix every hash value again](https://www.boost.org/doc/libs/latest/libs/unordered/doc/html/unordered/hash_quality.html)
before using it, unless `boost::hash_is_avalanching` reports the hasher avalanching, which it
reads from a nested `is_avalanching`. `hash<T, H>` declares it from
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

The tests check the trait against the Strict Avalanche Criterion, measured through the hasher's whole path for every
Hash2 algorithm, and SMHasher3's Avalanche test agrees with it for all 28
([measurements](measurements.md#avalanche)). Once Hash2 publishes the property, the hasher reads it instead.

**`constexpr`, `noexcept`.** All three constructors and the call operator are `constexpr`.
The constructors are constant expressions for every algorithm outside `legacy/`, which the
tests assert. The call operator is not yet one for any algorithm: Hash2's
`get_integral_result` is not `constexpr` as of 1.92.
The specifier costs nothing in a template and takes effect as soon as Hash2's does;
reimplementing the fold here to evaluate it now would fork Hash2's mapping, which a hasher
built on Hash2 must not. Nothing is `noexcept`: Hash2 declares no exception specification
on its algorithms, and the `hash_append` of a key is the user's own code, so a written
guarantee would be a promise about code this header does not see.

### `digest`

`xstd::digest<T, H>` runs the same seeded prototype over the same `hash_append` as
`hash<T, H>`, and returns the algorithm's whole `result_type` where `hash` folds it to
`std::size_t`. The fold is a function of the target, the 64-bit digest itself where `size_t`
holds it and a multiply-high fold of it where `size_t` is 32 bits, so a value that stands in for a
key across targets, runs or builds, such as a transposition table's or a stored one, is the
digest rather than the hash. It cannot serve an unordered container, whose hash must return
`std::size_t`, and so it declares neither `is_avalanching` nor `is_transparent`; over a
`std::basic_string` its call still takes the view, so a literal digests without a string being
built.

It is a class of its own rather than a member of `hash` because the two want different defaults.
A table hashes on every lookup and needs only `size_t`'s width; a digest is computed once per key
and its width is the point, as wide as makes two keys colliding never the likeliest bug at any
table size. The default is therefore `boost::hash2::xxh3_128`, which
[xxHash](https://github.com/Cyan4973/xxHash) recommends where a 128-bit hash is required, while
`hash` keeps `xxhash_64`. Through these classes `xxh3_128` takes three to five times as long per call on x86-64
([measurements](measurements.md#the-cost-of-a-call)), much of it the copy of its 560-byte state. That is a price a
digest pays once and a table would pay on every lookup.
The width buys the birthday bound: n keys under a b-bit digest collide with probability about
n²/2^(b+1), so 2³² keys collide with probability 2⁻⁶⁵ at 128 bits and 0.39 at 64.
A digest that persists still does better to name its algorithm, so that no change of default
could invalidate what was stored.

**Flavor.** Both classes take Hash2's [flavor](https://www.boost.org/doc/libs/latest/libs/hash2/doc/html/hash2.html#ref_flavor) as a third parameter, the byte order in which
`hash_append` writes integers and the type in which it writes a range's size. `digest` defaults to
`little_endian_flavor`, so a persisted digest is the same on a big-endian target as on a
little-endian one; there it writes the native bytes, at no cost, so every digest the tests pin
keeps its value. `hash` defaults to `default_flavor`, the native order, because a table's hash
never leaves the process and the native order keeps Hash2's contiguous fast path on every target.

### Formatting a digest

`<xstd/misc/format/digest.hpp>` specializes `std::formatter` for `boost::hash2::digest<N>`, the
result of XXH3-128 and every other algorithm with an array-like result, so that `std::format` and
`std::print` write it as `boost::hash2::to_string` does: two lowercase hex digits per byte, in byte
order, with `X` for uppercase. Any other specification is a `std::format_error`, and so a compile
error in a checked format string: a width or a base is the habit from integers, and a digest is
none. [[namespace.std]](https://eel.is/c++draft/namespace.std) admits the specialization because it depends on a type the standard library
does not define; Hash2 1.92 ships no formatter of its own, and one that a later release ships
would collide with this one at compile time rather than replace it in silence.

## Requirements and evolution

xstd-misc requires a conforming [C++20](https://wg21.link/N4861) compiler, and the
CMake project CMake 3.30 or later, exporting the header-only `xstd::misc` target.
Consumers build neither the tests nor their dependencies. New facilities should
stay small, portable and motivated by current practice; one that enters the
standard library can be retired, and the baseline can advance once a later
standard is a practical default. A facility that grows a domain of its own belongs in a
library named for it rather than here.
