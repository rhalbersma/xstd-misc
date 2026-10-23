# `<xstd/misc/functional/hash.hpp>`

## `xstd::hash`

```cpp
// (1)
template<class T, hash_algorithm H64 = boost::hash2::xxhash_64, hash_algorithm H32 = H64,
         class Flavor = boost::hash2::default_flavor>
  requires (sizeof(typename H32::result_type) <= sizeof(typename H64::result_type))
class hash;

// (2)
template<class CharT, class Traits, class Allocator, hash_algorithm H64, hash_algorithm H32, class Flavor>
class hash<basic_string<CharT, Traits, Allocator>, H64, H32, Flavor>;

// (3)
template<class T, hash_algorithm H64, hash_algorithm H32, class Flavor>
class hash<optional<T>, H64, H32, Flavor>;

// (4)
template<class T, class Allocator, hash_algorithm H64, hash_algorithm H32, class Flavor>
class hash<indirect<T, Allocator>, H64, H32, Flavor>;
```

An unordered container's hash for keys of type `T`, from a seeded Boost.Hash2 algorithm. The class holds a prototype
of `hash_type`, seeded once; each call copies the prototype, appends the key and folds the result to `size_t`.

1. The primary template.
2. The string specialization, transparent over `basic_string_view<CharT, Traits>`. A string, its view and a C string
   hash the same, and a set of strings finds a key by its view, or by a literal, without building a string. Only a
   string key is transparent: equal values of other types need not append the same bytes.
3. The optional specialization. An engaged optional hashes as `hash<remove_const_t<T>, H64, H32, Flavor>` hashes its
   value, as [[optional.hash]](https://eel.is/c++draft/optional.hash) requires, and an empty one as no bytes at all,
   which no key appends. It is seeded as that hash is, and `is_avalanching` is that hash's.
4. The indirect specialization, where the standard library declares `indirect`. An `indirect` hashes as
   `hash<T, H64, H32, Flavor>` hashes its value, as [[indirect.hash]](https://eel.is/c++draft/indirect.hash) requires,
   and a valueless one as no bytes at all.

A key's own hash is its Boost.Hash2 `tag_invoke`, which every key containing it follows at any depth. A
specialization of `hash` for a key is followed only by the optional and indirect specializations.

### Keys

Every key Boost.Hash2's `hash_append` hashes, and every key the standard specializes `std::hash` for. Hash2 1.92
hashes the arithmetic, enumeration and pointer types, `nullptr_t`, strings and their views, ranges, tuple-likes,
`optional`, `variant` and `monostate`. For the other standard keys, the header appends the state each one's
`operator==` reads:

| Key | Appends |
| :-- | :------ |
| a floating-point type of neither 4 nor 8 bytes, such as x87's `long double`, `float16_t` or `float128_t` | its value bits, with `-0` as `+0` and without padding |
| `__int128`, `unsigned __int128`, where the compiler has them and the key is not inside another | its low and high 64 bits |
| `bitset<N>` | its bits, 64 to a word |
| `error_code`, `error_condition` | the address of its category, then its value |
| `unique_ptr<T, D>`, `shared_ptr<T>` | `get()` |
| `type_index` | `name()` |
| `thread::id` | its object representation |
| `coroutine_handle<P>` | `address()` |
| `filesystem::path` | its root name, whether it has a root directory, then each element of its relative path |
| `stacktrace_entry` | `native_handle()` |
| `text_encoding` | `mib()`, then for `id::other` its name, lowercased, without non-alphanumerics or `0`s |
| `indirect<T, Allocator>` | whether it is valueless, then its value |
| `chrono::duration`, `chrono::time_point` | `count()`, `time_since_epoch()` |
| `chrono::day`, `month`, `year`, `weekday` and the calendar types built from them | each field `operator==` compares |
| `chrono::zoned_time`, `chrono::leap_second` | its time zone and its time, its date |

Equal keys therefore hash equal, and unequal keys append different bytes. A key whose state is an address, such as
a smart pointer or an error's category, hashes differently from one run to the next.

### Template parameters

| Parameter | Meaning |
| :-------- | :------ |
| `T` | The key type. An argument converts to `T` before it is appended, so `42` hashes as the `int64_t` a table holds. |
| `H64` | The algorithm where `size_t` is wider than 32 bits. Constrained to model `hash_algorithm`, so a second key type in its place is rejected. Defaults to `xxhash_64`. |
| `H32` | The algorithm where `size_t` is at most 32 bits, constrained as `H64` is. Defaults to `H64`, so one algorithm named alone runs on every target. |
| `Flavor` | The [flavor](https://www.boost.org/doc/libs/latest/libs/hash2/doc/html/hash2.html#hashing_objects_flavor) Hash2 appends with. Defaults to `default_flavor`, which appends in the target's own byte order. |

*Remarks:* A pair whose 32-bit algorithm is named first, such as `xxhash_32, xxhash_64`, is rejected by the
requires-clause.

### Member types

| Type | Definition |
| :--- | :--------- |
| `hash_type` | `conditional_t<sizeof(size_t) <= sizeof(uint32_t), H32, H64>`, the algorithm that runs |
| `is_avalanching` | `bool_constant<hash_algorithm_is_avalanching_v<hash_type>>`, which [`boost::hash_is_avalanching`](https://www.boost.org/doc/libs/latest/libs/container_hash/doc/html/hash.html#ref_hash_is_avalanchinghash) reads |
| `is_transparent` | (2) only |

### Member functions

| Function | Purpose |
| :------- | :------ |
| [(constructor)](#constructor) | value-initializes or seeds the prototype |
| [`operator()`](#operator) | hashes a key |

#### (constructor)

```cpp
hash() = default;                                       // (1)
constexpr explicit hash(uint64_t seed);                 // (2)
constexpr hash(const unsigned char* p, size_t n);       // (3)
```

1. Value-initializes the prototype.
2. Initializes it from `seed`.
3. Initializes it from the `n` bytes at `p`.

*Remarks:* Each is a constant expression.

#### `operator()`

```cpp
constexpr auto operator()(const T& v) const -> size_t;
```

The string specialization has two in its place: one taking `const basic_string<CharT, Traits, Allocator>&`, which
accepts whatever converts to a string, as `std::hash`'s parameter does, and one taking any `const K&` that converts
to `basic_string_view<CharT, Traits>`, such as a view or a C string, which hashes as the view. The optional
specialization takes `const optional<T>&` and the indirect one `const indirect<T, Allocator>&`.

- *Effects:* Copies the prototype to `h` and calls `boost::hash2::hash_append(h, Flavor(), v)`. The prototype is
  unchanged.
- *Returns:* `boost::hash2::get_integral_result<size_t>(h)`.
- *Throws:* Any exception the key's `hash_append` throws.
- *Remarks:* It is `noexcept` where [[unord.hash]](https://eel.is/c++draft/unord.hash)/3 makes `std::hash<T>`'s call
  `noexcept`, and `hash_type` is one of Hash2's own algorithms. That covers the arithmetic, enumeration and pointer
  types, `nullptr_t`, strings and their views, and each standard key the [Keys](#keys) table lists but `indirect`,
  `unique_ptr`, `duration`, `time_point` and `zoned_time`. The optional and indirect specializations are `noexcept`
  where the hash of their value is.
- *Remarks:* It is unconstrained: Hash2 declares `hash_append` for every type, so a constraint would admit keys it
  then rejects, such as `weak_ptr`.
- *Remarks:* As of Boost.Hash2 1.92 a call is not a constant expression, since
  [`get_integral_result`](https://www.boost.org/doc/libs/latest/libs/hash2/doc/html/hash2.html#ref_get_integral_result) is not `constexpr`.

### Notes

- Equal values hash equal, and unequal values append different bytes.
- It serves as the hash of `unordered_set` and `boost::unordered_flat_set`, seeded per container.
- Under FNV-1a, keys that differ only in their high bits share a bucket wherever a table masks the hash, and spread
  in a `boost::unordered_flat_set`, which mixes it first.

### Example

Adapted from the example of [`std::hash`](https://en.cppreference.com/w/cpp/utility/hash) on cppreference.com, at
[revision 179088](https://en.cppreference.com/mwiki/index.php?title=cpp/utility/hash&oldid=179088), by cppreference.com
contributors. This example is under [CC-BY-SA 3.0 and the GFDL](../../../examples/cppreference/LICENSE), as
cppreference.com's content is; the rest of this page is under the [Boost Software License](../../../LICENSE_1_0.txt).

```cpp
--8<-- "examples/cppreference/hash.cpp"
```

Output:

```text
--8<-- "examples/cppreference/hash.out"
```

### See also

| Entity | Relation |
| :----- | :------- |
| [`digest`](digest.md) | a key's digest at its algorithm's own width, from the same seeded prototype |
| [`hash_algorithm`](../concepts/hash_algorithm.md) | what `H64` and `H32` model |
| [`hash_algorithm_is_avalanching`](../type_traits/hash_algorithm_is_avalanching.md) | what `is_avalanching` reads |
| [`std::hash`](https://en.cppreference.com/w/cpp/utility/hash) | the standard's hash, whose role this class generalizes |
