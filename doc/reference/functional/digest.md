# `<xstd/misc/functional/digest.hpp>`

## `digest`

```cpp
template<class T, hash_algorithm H, class Flavor>
class digest {
public:
  using result_type = typename H::result_type;

  digest() = default;
  constexpr explicit digest(uint64_t seed);
  constexpr digest(const unsigned char* p, size_t n);

  constexpr auto operator()(const T& v) const -> result_type;
};
```

- `H` is constrained to model `hash_algorithm`. A `digest` does not model it: it takes a key rather than updates.
- `H` defaults to `xxh3_128` and `Flavor` to `little_endian_flavor`, so a digest is the same on every target.
- *Constructors:* As for `hash`.
- `operator()`:
  - *Returns:* `h.result()`, where `h` is a copy of the prototype after `boost::hash2::hash_append(h, Flavor(), v)`.
  - An argument converts to `T` before it is appended.
  - *Throws:* Any exception the key's `hash_append` throws. It is not `noexcept`.
- The digests of fixed-width keys under the default algorithm match the reference xxHash implementation's.

## `digest<basic_string<CharT, Traits, Allocator>, H, Flavor>`

As the primary template, with `operator()` taking `basic_string_view<CharT, Traits>`. A string, its view and a C
string digest the same.
