# `<xstd/misc/concepts/simple_allocator.hpp>`

## `xstd::simple_allocator`

```cpp
template<class Alloc>
concept simple_allocator =
  requires(Alloc alloc, size_t n) {
    { *alloc.allocate(n) } -> same_as<typename Alloc::value_type&>;
    { alloc.deallocate(alloc.allocate(n), n) };
  } and copy_constructible<Alloc> and equality_comparable<Alloc>;
```

An allocator, to the syntax the working draft checks. The definition is its exposition-only *simple-allocator*,
[[allocator.requirements.general]/99](https://eel.is/c++draft/allocator.requirements.general#99), word for word,
under a name users can spell.

### Semantic requirements

A type models it if it meets the requirements of
[[allocator.requirements.general]](https://eel.is/c++draft/allocator.requirements.general).

### Example

```cpp
--8<-- "examples/concepts/simple_allocator.cpp"
```

Output:

```text
--8<-- "examples/concepts/simple_allocator.out"
```

### See also

| Entity | Relation |
| :----- | :------- |
| [`container_compatible_range`](container_compatible_range.md) | the other exposition-only concept copied from the working draft |
| [`std::allocator_traits`](https://en.cppreference.com/w/cpp/memory/allocator_traits) | the standard's uniform interface to an allocator |
