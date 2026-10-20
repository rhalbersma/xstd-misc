# `<xstd/misc/type_traits/hash_algorithm_is_avalanching.hpp>`

## `hash_algorithm_is_avalanching`, `hash_algorithm_is_avalanching_v`

- A [*Cpp17UnaryTypeTrait*](https://eel.is/c++draft/meta.rqmts) whose base characteristic is `true_type` for every Boost.Hash2 algorithm but `fnv1a_32` and
  `fnv1a_64`, and for `boost::hash2::hmac<H>` exactly where it is for `H`, and `false_type` otherwise.
- `true` means that flipping any one bit of the input flips each bit of the result with probability ½, measured
  through `hash`'s whole path: the seed, `hash_append` and the fold to `size_t`.
- *Remarks:* A program may specialize it for an algorithm of its own.
