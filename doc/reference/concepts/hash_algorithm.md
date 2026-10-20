# `<xstd/misc/concepts/hash_algorithm.hpp>`

## `xstd::hash_algorithm`

```cpp
template<class H>
concept hash_algorithm =
  semiregular<H> and
  constructible_from<H, uint64_t> and
  constructible_from<H, const unsigned char*, size_t> and
  requires(H& h, const void* p, size_t n) {
    typename H::result_type;
    h.update(p, n);
    { h.result() } -> same_as<typename H::result_type>;
  } and
  ((unsigned_integral<typename H::result_type> and not same_as<typename H::result_type, bool>) or
   (boost::hash2::has_constant_size<typename H::result_type>::value and
    ranges::contiguous_range<typename H::result_type> and
    same_as<ranges::range_value_t<typename H::result_type>, unsigned char>)) and
  (not requires { H::block_size; } or same_as<remove_cv_t<decltype(H::block_size)>, size_t>);
```

A [Boost.Hash2](https://www.boost.org/doc/libs/latest/libs/hash2/) hash algorithm: seeded by an integer or by bytes,
updated with bytes, and finished with a result that is an unsigned integer or a fixed-size byte array.

### Semantic requirements

A type models it if it meets
[Boost.Hash2's documented requirements](https://www.boost.org/doc/libs/latest/libs/hash2/doc/html/hash2.html#hashing_bytes_hash_algorithm_requirements)
on an algorithm:

- the split of a message into calls to `update` does not affect the result;
- a copy is equivalent to the original;
- seed 0, and an empty byte seed, are default construction, and distinct seeds give distinct states;
- each further call to `result` continues a sequence that an equal state repeats.

### Models

- Every Boost.Hash2 algorithm models it.
- Each form a result and a block size take in Hash2 satisfies it: an unsigned integer or a fixed-size byte array,
  with a block size or none.
- A hasher, which hashes a key rather than updating with bytes, does not satisfy it.
- A `bool` result is turned away.
- A `block_size` of a type other than `size_t` is turned away.

### See also

| Entity | Relation |
| :----- | :------- |
| [`hash`](../functional/hash.md) | constrains `H64` and `H32` on it |
| [`digest`](../functional/digest.md) | constrains `H` on it |
| [`hash_algorithm_is_avalanching`](../type_traits/hash_algorithm_is_avalanching.md) | whether a model avalanches |
