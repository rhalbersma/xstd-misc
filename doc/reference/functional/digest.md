# `<xstd/misc/functional/digest.hpp>`

## `xstd::digest`

```cpp
// (1)
template<class T, hash_algorithm H = boost::hash2::xxh3_128, class Flavor = boost::hash2::little_endian_flavor>
class digest;

// (2)
template<class CharT, class Traits, class Allocator, hash_algorithm H, class Flavor>
class digest<basic_string<CharT, Traits, Allocator>, H, Flavor>;
```

A key's digest at its algorithm's own width, from a seeded Boost.Hash2 algorithm. The class holds a prototype of `H`,
seeded once; each call copies the prototype, appends the key and returns the algorithm's whole result.

1. The primary template.
2. The string specialization, taking `basic_string_view<CharT, Traits>`. A string, its view and a C string digest the
   same.

### Template parameters

| Parameter | Meaning |
| :-------- | :------ |
| `T` | The key type. An argument converts to `T` before it is appended. |
| `H` | The algorithm. Constrained to model `hash_algorithm`. Defaults to `xxh3_128`. |
| `Flavor` | The [flavor](https://www.boost.org/doc/libs/latest/libs/hash2/doc/html/hash2.html#hashing_objects_flavor) Hash2 appends with. Defaults to `little_endian_flavor`, so a digest is the same on every target. |

### Member types

| Type | Definition |
| :--- | :--------- |
| `result_type` | `typename H::result_type` |

### Member functions

| Function | Purpose |
| :------- | :------ |
| [(constructor)](#constructor) | value-initializes or seeds the prototype |
| [`operator()`](#operator) | digests a key |

#### (constructor)

```cpp
digest() = default;                                     // (1)
constexpr explicit digest(uint64_t seed);               // (2)
constexpr digest(const unsigned char* p, size_t n);     // (3)
```

As for [`hash`](hash.md#constructor).

#### `operator()`

```cpp
constexpr auto operator()(const T& v) const -> result_type;
```

The string specialization takes `basic_string_view<CharT, Traits>` in place of `const T&`.

- *Returns:* `h.result()`, where `h` is a copy of the prototype after `boost::hash2::hash_append(h, Flavor(), v)`.
- *Throws:* Any exception the key's `hash_append` throws. It is not `noexcept`.

### Notes

- A `digest` does not model `hash_algorithm`: it takes a key rather than updates.
- The digests of fixed-width keys under the default algorithm match the reference xxHash implementation's.
- Where the digest is as wide as `size_t`, the hash is the digest unchanged.

### Example

```cpp
--8<-- "examples/functional/digest.cpp"
```

Output:

```text
--8<-- "examples/functional/digest.out"
```

### See also

| Entity | Relation |
| :----- | :------- |
| [`hash`](hash.md) | the same seeded prototype, folded to `size_t` for a container |
| [`formatter<boost::hash2::digest<N>>`](../format/digest.md) | writes a digest as hex |
| [`hash_algorithm`](../concepts/hash_algorithm.md) | what `H` models |
