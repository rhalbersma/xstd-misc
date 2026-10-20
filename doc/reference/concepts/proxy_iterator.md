# `<xstd/misc/concepts/proxy_iterator.hpp>`

## `xstd::proxy_iterator`

```cpp
template<class I>
concept proxy_iterator = proxy_semi_iterator<I> and proxy_reference<iter_reference_t<I>>;
```

An iterator whose reference is a proxy reference.

### Models

- A semi-iterator whose reference is a proxy reference models it.
- A semi-iterator whose reference keeps the built-in address does not.

### Notes

It subsumes `proxy_semi_iterator<I>`.

### See also

| Entity | Relation |
| :----- | :------- |
| [`proxy_semi_iterator`](proxy_semi_iterator.md) | the weaker concept it subsumes |
| [`proxy_reference`](proxy_reference.md) | what its reference models |
