# `<xstd/misc/concepts/proxy_semi_iterator.hpp>`

## `xstd::proxy_semi_iterator`

```cpp
template<class I>
concept proxy_semi_iterator =
  indirectly_readable<I> and proxy_semi_reference<iter_reference_t<I>, iter_value_t<I>>;
```

An iterator whose reference is a proxy for its value type.

### Models

- An iterator whose reference is a class prvalue standing in for its value type models it: `vector<bool>`'s
  iterator, `zip_view`'s.
- A pointer, an iterator yielding a language reference, and one yielding its value type by value do not.

### Example

```cpp
--8<-- "examples/concepts/proxy_semi_iterator.cpp"
```

Output:

```text
--8<-- "examples/concepts/proxy_semi_iterator.out"
```

### See also

| Entity | Relation |
| :----- | :------- |
| [`proxy_semi_reference`](proxy_semi_reference.md) | what its reference models |
| [`proxy_iterator`](proxy_iterator.md) | the stronger concept, whose reference is a proxy reference |
