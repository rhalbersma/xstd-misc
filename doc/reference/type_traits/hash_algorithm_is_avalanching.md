# `<xstd/misc/type_traits/hash_algorithm_is_avalanching.hpp>`

## `xstd::hash_algorithm_is_avalanching`, `xstd::hash_algorithm_is_avalanching_v`

```cpp
template<class H>
struct hash_algorithm_is_avalanching;

template<class H>
constexpr bool hash_algorithm_is_avalanching_v = hash_algorithm_is_avalanching<H>::value;
```

Whether a hash algorithm avalanches, as Boost.Unordered asks of a hasher.

### Value

A [*Cpp17UnaryTypeTrait*](https://eel.is/c++draft/meta.rqmts) whose base characteristic is `true_type` for every
Boost.Hash2 algorithm but `fnv1a_32` and `fnv1a_64`, for `boost::hash2::hmac<H>` exactly where it is for `H`, and
`false_type` otherwise.

`true` means that flipping any one bit of the input flips each bit of the result with probability ½, measured through
`hash`'s whole path: the seed, `hash_append` and the fold to `size_t`.

### Remarks

A program may specialize it for an algorithm of its own.

### Example

```cpp
--8<-- "examples/type_traits/hash_algorithm_is_avalanching.cpp"
```

Output:

```text
--8<-- "examples/type_traits/hash_algorithm_is_avalanching.out"
```

### See also

| Entity | Relation |
| :----- | :------- |
| [`hash`](../functional/hash.md) | declares `is_avalanching` from it |
| [`hash_algorithm`](../concepts/hash_algorithm.md) | what `H` is meant to model |
| [`boost::hash_is_avalanching`](https://www.boost.org/doc/libs/latest/libs/container_hash/doc/html/hash.html#ref_hash_is_avalanchinghash) | the trait Boost.Unordered reads from a hasher |
