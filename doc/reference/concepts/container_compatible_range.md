# `<xstd/misc/concepts/container_compatible_range.hpp>`

## `container_compatible_range`

```cpp
template<class R, class T>
concept container_compatible_range =
  ranges::input_range<R> and convertible_to<ranges::range_reference_t<R>, T>;
```

- The definition is the working draft's exposition-only *container-compatible-range*,
  [[container.intro.reqmts]/2](https://eel.is/c++draft/container.intro.reqmts#2), word for word.
- An input range of references that convert to `T` models it, and nothing else does.
