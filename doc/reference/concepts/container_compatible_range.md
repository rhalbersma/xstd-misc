# `<xstd/misc/concepts/container_compatible_range.hpp>`

## `xstd::container_compatible_range`

```cpp
template<class R, class T>
concept container_compatible_range =
  ranges::input_range<R> and convertible_to<ranges::range_reference_t<R>, T>;
```

A range whose elements a container of `T` can be built from. The definition is the working draft's exposition-only
*container-compatible-range*, [[container.intro.reqmts]/2](https://eel.is/c++draft/container.intro.reqmts#2), word
for word, under a name users can spell.

### Models

- An input range of references that convert to `T` models it, and nothing else does.

### Example

```cpp
--8<-- "examples/concepts/container_compatible_range.cpp"
```

Output:

```text
--8<-- "examples/concepts/container_compatible_range.out"
```

### See also

| Entity | Relation |
| :----- | :------- |
| [`from_range_t`](https://en.cppreference.com/w/cpp/ranges/from_range) | the tag of the standard containers' constructors that take such a range |
| [`simple_allocator`](simple_allocator.md) | the other exposition-only concept copied from the working draft |
