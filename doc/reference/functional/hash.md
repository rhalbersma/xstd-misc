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
```

An unordered container's hash for keys of type `T`, from a seeded Boost.Hash2 algorithm. The class holds a prototype
of `hash_type`, seeded once; each call copies the prototype, appends the key and folds the result to `size_t`.

1. The primary template.
2. The string specialization, transparent over `basic_string_view<CharT, Traits>`. A string, its view and a C string
   hash the same, and a set of strings finds a key by its view, or by a literal, without building a string. Only a
   string key is transparent: equal values of other types need not append the same bytes.

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

The string specialization takes `basic_string_view<CharT, Traits>` in place of `const T&`.

- *Effects:* Copies the prototype to `h` and calls `boost::hash2::hash_append(h, Flavor(), v)`. The prototype is
  unchanged.
- *Returns:* `boost::hash2::get_integral_result<size_t>(h)`.
- *Throws:* Any exception the key's `hash_append` throws. It is not `noexcept`.
- *Remarks:* It is unconstrained: Hash2 declares `hash_append` for every type, so a constraint would admit keys it
  then rejects, such as `thread::id`.
- *Remarks:* As of Boost.Hash2 1.92 a call is not a constant expression, since
  [`get_integral_result`](https://www.boost.org/doc/libs/latest/libs/hash2/doc/html/hash2.html#ref_get_integral_result) is not `constexpr`.

### Notes

- Equal values hash equal.
- It serves as the hash of `unordered_set` and `boost::unordered_flat_set`, seeded per container.
- Under FNV-1a, keys that differ only in their high bits share a bucket wherever a table masks the hash, and spread
  in a `boost::unordered_flat_set`, which mixes it first.

### Example

Ported from the example of [`std::hash`](https://en.cppreference.com/w/cpp/utility/hash) on cppreference.com, at
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
