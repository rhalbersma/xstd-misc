# `<xstd/misc/format.hpp>`

```cpp
namespace std {
  template<size_t N>
    struct formatter<boost::hash2::digest<N>>;
}
```

| Header | Declares |
| :----- | :------- |
| [`<xstd/misc/format/digest.hpp>`](digest.md) | `formatter<boost::hash2::digest<N>>` |

Every header here uses Boost.Hash2, so `<xstd/misc.hpp>` leaves this group out: include it and link `Boost::hash2`.
