# `<xstd/misc/concepts.hpp>`

```cpp
namespace xstd {
  template<class R, class T>
    concept container_compatible_range = see below;
  template<class H>
    concept hash_algorithm = see below;
  template<class R, class V>
    concept proxy_semi_reference = see below;
  template<class I>
    concept proxy_semi_iterator = see below;
  template<class R>
    concept proxy_reference = see below;
  template<class I>
    concept proxy_iterator = see below;
  template<class Alloc>
    concept simple_allocator = see below;
  template<class T, template<class...> class Primary>
    concept specialization_of = see below;
}
```

| Header | Declares |
| :----- | :------- |
| [`<xstd/misc/concepts/container_compatible_range.hpp>`](container_compatible_range.md) | `container_compatible_range` |
| [`<xstd/misc/concepts/hash_algorithm.hpp>`](hash_algorithm.md) | `hash_algorithm` |
| [`<xstd/misc/concepts/proxy_iterator.hpp>`](proxy_iterator.md) | `proxy_iterator` |
| [`<xstd/misc/concepts/proxy_reference.hpp>`](proxy_reference.md) | `proxy_reference` |
| [`<xstd/misc/concepts/proxy_semi_iterator.hpp>`](proxy_semi_iterator.md) | `proxy_semi_iterator` |
| [`<xstd/misc/concepts/proxy_semi_reference.hpp>`](proxy_semi_reference.md) | `proxy_semi_reference` |
| [`<xstd/misc/concepts/simple_allocator.hpp>`](simple_allocator.md) | `simple_allocator` |
| [`<xstd/misc/concepts/specialization_of.hpp>`](specialization_of.md) | `specialization_of` |
