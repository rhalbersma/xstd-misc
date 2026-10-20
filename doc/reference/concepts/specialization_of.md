# `<xstd/misc/concepts/specialization_of.hpp>`

## `specialization_of`

```cpp
template<class T, template<class...> class Primary>
concept specialization_of = is_specialization_of_v<remove_const_t<T>, Primary>;
```

- A specialization of `Primary` models it, and so does that specialization `const`.
- A reference to a specialization does not.
- An adaptor constrained on it accepts a `const` owner and a mutable one.
