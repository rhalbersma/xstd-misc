# `<xstd/misc/concepts/proxy_semi_reference.hpp>`

## `xstd::proxy_semi_reference`

```cpp
template<class R, class V>
concept proxy_semi_reference = is_class_v<R> and (not same_as<R, V>) and convertible_to<R, V>;
```

A class that stands in for a value of type `V`, which the caller names, and converts to it.

### Models

- Every library's `vector<bool>::reference` and `bitset<N>::reference` models it for `bool`.
- A class that converts to the named `V` models it, with or without a way back from its address.
- `V` itself, a scalar, a class naming a value it does not convert to, and a language reference do not.

### See also

| Entity | Relation |
| :----- | :------- |
| [`proxy_reference`](proxy_reference.md) | a semi-reference whose address leads back to it, and which writes through |
| [`proxy_semi_iterator`](proxy_semi_iterator.md) | an iterator whose reference models this concept |
