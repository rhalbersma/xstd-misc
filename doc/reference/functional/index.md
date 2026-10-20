# `<xstd/misc/functional.hpp>`

```cpp
namespace xstd {
  template<class T, hash_algorithm H64 = boost::hash2::xxhash_64, hash_algorithm H32 = H64,
           class Flavor = boost::hash2::default_flavor>
      requires (sizeof(typename H32::result_type) <= sizeof(typename H64::result_type))
    class hash;
  template<class CharT, class Traits, class Allocator, hash_algorithm H64, hash_algorithm H32, class Flavor>
      requires (sizeof(typename H32::result_type) <= sizeof(typename H64::result_type))
    class hash<basic_string<CharT, Traits, Allocator>, H64, H32, Flavor>;
  template<class T, hash_algorithm H = boost::hash2::xxh3_128, class Flavor = boost::hash2::little_endian_flavor>
    class digest;
  template<class CharT, class Traits, class Allocator, hash_algorithm H, class Flavor>
    class digest<basic_string<CharT, Traits, Allocator>, H, Flavor>;
}
```

| Header | Declares |
| :----- | :------- |
| [`<xstd/misc/functional/digest.hpp>`](digest.md) | `digest` |
| [`<xstd/misc/functional/hash.hpp>`](hash.md) | `hash` |

Where the digest is as wide as `size_t`, the hash is the digest unchanged.
