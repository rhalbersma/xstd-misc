# `<xstd/misc/concepts/proxy_semi_iterator.hpp>`

## `proxy_semi_iterator`

```cpp
template<class I>
concept proxy_semi_iterator =
  indirectly_readable<I> and proxy_semi_reference<iter_reference_t<I>, iter_value_t<I>>;
```

- An iterator whose reference is a class prvalue standing in for its value type models it: `vector<bool>`'s
  iterator, `zip_view`'s.
- A pointer, an iterator yielding a language reference, and one yielding its value type by value do not.
