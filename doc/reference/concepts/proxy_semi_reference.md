# `<xstd/misc/concepts/proxy_semi_reference.hpp>`

## `proxy_semi_reference`

```cpp
template<class R, class V>
concept proxy_semi_reference = is_class_v<R> and (not same_as<R, V>) and convertible_to<R, V>;
```

- Every library's `vector<bool>::reference` and `bitset<N>::reference` models it for `bool`.
- A class that converts to the named `V` models it, with or without a way back from its address.
- `V` itself, a scalar, a class naming a value it does not convert to, and a language reference do not.
