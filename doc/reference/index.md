# Reference

Each public entity of xstd-misc is specified here the way the standard specifies the library: a synopsis per header,
then for each entity the elements of [[structure.specifications]](https://eel.is/c++draft/structure.specifications) that apply. The baseline is
[C++20](https://wg21.link/N4861), and a name with no namespace is in `std`.

The synopses leave out `[[nodiscard]]`, as the standard's own have since [P2422R1](https://github.com/cplusplus/papers/issues/1744), which made
it a recommended practice rather than part of a library's specification. The code carries it on every function that
returns a value and on every constructor, so discarding any result here draws a warning.

## Relation to the standard library

Each entity is one of five kinds:

- **Copied:** the standard's own exposition-only wording, word for word, under a name users can spell.
- **Extends:** a standard facility under its own name, giving the standard's answer wherever the standard gives one,
  with overloads or specializations for more types.
- **Generalizes:** the job of a standard facility, done for more types through an interface of its own.
- **Spells:** a portable spelling of a standard attribute.
- **Proposed** or **new:** no counterpart in the standard. A proposed entity follows a WG21 paper; a new one follows
  none.

| Entity | Kind | Counterpart |
| :-- | :-- | :-- |
| `container_compatible_range` | Copied | *container-compatible-range*, [[container.intro.reqmts]/2](https://eel.is/c++draft/container.intro.reqmts#2) |
| `simple_allocator` | Copied | *simple-allocator*, [[allocator.requirements.general]/99](https://eel.is/c++draft/allocator.requirements.general#99) |
| `to_underlying` | Extends | `to_underlying`, [[utility.underlying]](https://eel.is/c++draft/utility.underlying), from [P1682R1](https://github.com/cplusplus/papers/issues/460): an enumeration forwards to it, and the overloads add an enumerator kept in `integral_constant` and a proxy reference to an enumeration |
| `formatter<boost::hash2::digest<N>>` | Extends | `formatter`, [[format.formatter.spec]](https://eel.is/c++draft/format.formatter.spec): a specialization for Boost.Hash2's digest, as [[namespace.std]](https://eel.is/c++draft/namespace.std) admits for a type the standard library does not declare |
| `hash` | Generalizes | `hash`, [[unord.hash]](https://eel.is/c++draft/unord.hash), and the [*Cpp17Hash*](https://eel.is/c++draft/hash.requirements) requirements it meets: any key with a `hash_append`, under a Boost.Hash2 algorithm the caller names and seeds, after [N3980](https://wg21.link/n3980)'s `uhash` |
| `XSTD_NO_UNIQUE_ADDRESS` | Spells | `[[no_unique_address]]`, [[dcl.attr.nouniqueaddr]](https://eel.is/c++draft/dcl.attr.nouniqueaddr), as `msvc::no_unique_address` where the MSVC-compatible frontend reads the standard one as layout-neutral |
| `is_specialization_of`, `specialization_of` | Proposed | `is_specialization_of`, in [P2098R1](https://github.com/cplusplus/papers/issues/812)'s spelling, for class templates whose parameters are all types |
| `digest` | Proposed | [N3980](https://wg21.link/n3980)'s `uhash`, which returns the algorithm's whole result as `digest` does, here with a seed |
| `proxy_semi_reference`, `proxy_semi_iterator`, `proxy_reference`, `proxy_iterator` | New | none: the standard names no proxy concept; these are built on `indirectly_readable` and `indirectly_writable`, [[iterator.concepts]](https://eel.is/c++draft/iterator.concepts), and [P2321](https://github.com/cplusplus/papers/issues/1003)'s const-assignable proxies |
| `hash_algorithm` | New | none in the standard: Boost.Hash2's documented requirements on a hash algorithm, as a concept |
| `hash_algorithm_is_avalanching` | New | none in the standard: Boost.Unordered's `hash_is_avalanching` asks the same of a hasher, and this asks it of an algorithm |
| `empty_base_type`, `empty_member_type`, `conditional_data_member` | New | none |

## Prior work

What each entity builds on in C++, and, for an entity that introduces a name or a concept of its own, its counterpart in another language where that counterpart shares its purpose. Papers are linked through wg21.link.

- **`container_compatible_range`:** introduced as an exposition-only concept by [P1206R7](https://wg21.link/p1206r7),
  *Conversions from ranges to containers* (Corentin Jabot, Eric Niebler, Casey Carter), for `ranges::to` and the
  containers' `from_range` constructors.
- **`to_underlying`:** [P1682R1](https://wg21.link/p1682r1), *`std::to_underlying` for enumerations* (JeanHeyd
  Meneide), from an idea of Walter E. Brown's.
- **`is_specialization_of`, `specialization_of`:** [P2098R1](https://wg21.link/p2098r1), *Proposing
  `std::is_specialization_of`* (Walter E. Brown, Bob Steagall), whose spelling and all-types scope these follow.
- **`XSTD_NO_UNIQUE_ADDRESS`, `empty_base_type`, `empty_member_type`, `conditional_data_member`:**
  `[[no_unique_address]]` comes from [P0840R2](https://wg21.link/p0840r2), *Language support for empty objects*
  (Richard Smith). It replaced the empty-base optimization that
  [`boost::compressed_pair`](https://www.boost.org/doc/libs/latest/libs/utility/doc/html/utility/utilities/compressed_pair.html)
  and the standard libraries' own compressed pairs rely on. MSVC documents its own spelling under
  [attributes](https://learn.microsoft.com/en-us/cpp/cpp/attributes).
- **The proxy concepts:** [P0022R2](https://wg21.link/p0022r2), *Proxy Iterators for the Ranges Extensions* (Eric
  Niebler), and his [To Be or Not to Be (an Iterator)](https://ericniebler.com/2015/01/28/to-be-or-not-to-be-an-iterator/),
  set out the problem `vector<bool>` poses. [P2321R2](https://wg21.link/p2321r2), *zip* (Tim Song), made its reference
  assignable through `const`, which `proxy_reference` asks of a writable proxy.
- **`hash_algorithm`:**
  - **C++:** Boost.Hash2's documented requirements on a hash algorithm, which descend from [N3980](https://wg21.link/n3980).
  - **Rust** has the same separation in its [`Hasher`](https://doc.rust-lang.org/std/hash/trait.Hasher.html) trait:
    what a type hashes is its own, and the algorithm is a parameter.
- **`hash`:**
  - [N3980](https://wg21.link/n3980), *Types Don't Know #* (Howard Hinnant, Vinnie Falco, John Bytheway), whose
    `uhash` this follows.
  - [P0029R0](https://wg21.link/p0029r0), *A Unified Proposal for Composable Hashing* (Geoff Romer, Chandler
    Carruth), and [Abseil's `absl::Hash`](https://abseil.io/docs/cpp/guides/hash), which take the algorithm away from
    the user instead.
- **`hash_algorithm_is_avalanching`:** Boost.Unordered's
  [`hash_is_avalanching`](https://www.boost.org/doc/libs/latest/libs/unordered/doc/html/unordered/reference/hash_traits.html),
  which asks it of a hasher.
- **`digest`, and its formatter:**
  - **C++:** [N3980](https://wg21.link/n3980)'s `uhash` returns the algorithm's whole result, as `digest` does.
    Boost.Hash2's `to_string` writes it as hex, as the formatter does.
  - **Python's** [`hashlib`](https://docs.python.org/3/library/hashlib.html) `digest()` is the same fixed-width
    fingerprint at the algorithm's own width.

## Headers

Each group of headers has a page, and each header a page under it:

- [`<xstd/misc/concepts.hpp>`](concepts/index.md)
- [`<xstd/misc/type_traits.hpp>`](type_traits/index.md)
- [`<xstd/misc/functional.hpp>`](functional/index.md)
- [`<xstd/misc/format.hpp>`](format/index.md)
- [`<xstd/misc/utility.hpp>`](utility/index.md)
