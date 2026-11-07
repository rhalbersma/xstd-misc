# Changelog

Each release of xstd-misc, newest first. A version is a tag `vX.Y.Z` matching `project(VERSION)` in
[CMakeLists.txt](CMakeLists.txt); an entry is written from the pull requests merged since the previous tag, grouped
by what a user of the library meets. The numbers are this repository's pull requests.

## 0.1.0 (2026-11-07)

The first release: small extensions to the C++ standard library that the xstd family needs beyond integers and bit
containers, each under the standard header it extends, in namespace `xstd`. It needs a C++20 compiler, CMake 3.30 or
later for the supplied CMake project, and Boost.Hash2 from Boost 1.89 (#23, #100).

### `<xstd/misc/concepts.hpp>`

- `proxy_reference` and `proxy_iterator`, the shapes a proxy container's reference and iterator take, in semi and full
  strengths, the iterators ordered by subsumption (#32, #33, #70, #72).
- `hash_algorithm`, for a Boost.Hash2 hash algorithm, tested against Hash2's own algorithms and their near-misses
  (#36, #38, #58).
- `specialization_of`, and `container_compatible_range` and `simple_allocator`, copied from the standard's
  exposition-only concepts and checked against the working draft (#26, #74).

### `<xstd/misc/functional.hpp>` and `<xstd/misc/format.hpp>`

- `hash`, an unordered container's hash, and `digest`, a key's digest at the algorithm's own width, each from a seeded
  Boost.Hash2 algorithm (#35, #37, #40, #67, #81).
- `hash` covers every key the standard specializes `std::hash` for, `_BitInt` of every width among them, and rejects a
  string or view whose traits are not `std::char_traits` (#116, #118, #124).
- A `std::formatter` that writes a digest as hex (#67).

### `<xstd/misc/type_traits.hpp>`

- `is_specialization_of`, and `hash_algorithm_is_avalanching` (#56).
- `XSTD_NO_UNIQUE_ADDRESS`, and `empty_base_type`, `empty_member_type` and `conditional_data_member`: types that keep
  the place of a base or member that may be absent (#8).

### `<xstd/misc/utility.hpp>`

- `to_underlying`, extending `std::to_underlying` to an enumerator held in `std::integral_constant` and to a proxy
  reference to one (#32, #79).

### Platforms

GCC 15, 16 and trunk; Clang 22, 23 and trunk, with libstdc++ and with libc++; Apple Clang; MSVC and clang-cl on Visual
Studio 2022 and 2026, x64 and x86; MinGW; Emscripten for wasm32; i686; Android and iOS (#117); and big-endian s390x
under QEMU (#103). Every one also builds under `-fno-exceptions` (#117). The README's matrix gives the exact versions.

### How it is checked

- libc++'s, libstdc++'s and the MSVC STL's own tests of `to_underlying` and of every hash run against xstd (#109, #116,
  #123).
- Each constraint clause is deleted in turn, weekly, and every deletion no test notices is answered (#76, #101, #121).
- Every public entity is specified in standardese, naming the test behind each element, and every performance and
  quality claim gives its measured number (#77, #99).
- Every public header is a leaf over a shared `detail/`, so deleting one breaks no other (#120), and every reference page
  shows a compiled example whose output is checked (#103).
