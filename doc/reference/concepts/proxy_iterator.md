# `<xstd/misc/concepts/proxy_iterator.hpp>`

## `proxy_iterator`

```cpp
template<class I>
concept proxy_iterator = proxy_semi_iterator<I> and proxy_reference<iter_reference_t<I>>;
```

- A semi-iterator whose reference is a proxy reference models it.
- A semi-iterator whose reference keeps the built-in address does not.
- *Remarks:* It subsumes `proxy_semi_iterator<I>`.
