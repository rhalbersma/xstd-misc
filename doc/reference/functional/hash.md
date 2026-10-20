# `<xstd/misc/functional/hash.hpp>`

## `hash`

```cpp
template<class T, hash_algorithm H64, hash_algorithm H32, class Flavor>
class hash {
public:
  using hash_type      = conditional_t<sizeof(size_t) <= sizeof(uint32_t), H32, H64>;
  using is_avalanching = bool_constant<hash_algorithm_is_avalanching_v<hash_type>>;

  hash() = default;
  constexpr explicit hash(uint64_t seed);
  constexpr hash(const unsigned char* p, size_t n);

  constexpr auto operator()(const T& v) const -> size_t;
};
```

The class holds a prototype of `hash_type`.

- `H64` and `H32` are constrained to model `hash_algorithm`, so a second key type in either place is rejected.
- `H64` defaults to `xxhash_64`, `H32` to `H64` and `Flavor` to `default_flavor`, which appends in the target's own
  byte order.
- `hash_type` is `H32` where `size_t` is at most 32 bits, and `H64` otherwise. One algorithm named alone runs on
  every target.
- *Remarks:* A pair whose 32-bit algorithm is named first, such as `xxhash_32, xxhash_64`, is rejected.
- `is_avalanching` is what [`boost::hash_is_avalanching`](https://www.boost.org/doc/libs/latest/libs/container_hash/doc/html/hash.html#ref_hash_is_avalanchinghash) reads.
- *Constructors, Effects:* Value-initializes the prototype, or initializes it from `seed`, or from the `n` bytes at
  `p`. Each is a constant expression, and the integer one is `explicit`.
- `operator()`:
  - *Effects:* Copies the prototype to `h` and calls `boost::hash2::hash_append(h, Flavor(), v)`. The prototype is
    unchanged.
  - *Returns:* `boost::hash2::get_integral_result<size_t>(h)`.
  - An argument converts to `T` before it is appended, so `42` hashes as the `int64_t` a table holds.
  - *Throws:* Any exception the key's `hash_append` throws. It is not `noexcept`.
  - *Remarks:* It is unconstrained: Hash2 declares `hash_append` for every type, so a constraint would admit keys it
    then rejects, such as `thread::id`.
  - *Remarks:* As of Boost.Hash2 1.92 a call is not a constant expression, since
    [`get_integral_result`](https://www.boost.org/doc/libs/latest/libs/hash2/doc/html/hash2.html#ref_get_integral_result) is not `constexpr`.
- Equal values hash equal.
- Under FNV-1a, keys that differ only in their high bits share a bucket wherever a table masks the hash, and spread
  in a `boost::unordered_flat_set`, which mixes it first.
- It serves as the hash of `unordered_set` and `boost::unordered_flat_set`, seeded per container.

## `hash<basic_string<CharT, Traits, Allocator>, H64, H32, Flavor>`

As the primary template, with `is_transparent` declared and `operator()` taking `basic_string_view<CharT, Traits>`.

- Only a string key is transparent: equal values of other types need not append the same bytes.
- A string, its view and a C string hash the same.
- A set of strings finds a key by its view, or by a literal, without building a string.
