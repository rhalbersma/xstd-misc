# `<xstd/misc/concepts/simple_allocator.hpp>`

## `simple_allocator`

```cpp
template<class Alloc>
concept simple_allocator =
  requires(Alloc alloc, size_t n) {
    { *alloc.allocate(n) } -> same_as<typename Alloc::value_type&>;
    { alloc.deallocate(alloc.allocate(n), n) };
  } and copy_constructible<Alloc> and equality_comparable<Alloc>;
```

- The definition is the working draft's exposition-only *simple-allocator*,
  [[allocator.requirements.general]/99](https://eel.is/c++draft/allocator.requirements.general#99), word for word.
- A type models it if it meets the requirements of
  [[allocator.requirements.general]](https://eel.is/c++draft/allocator.requirements.general).
