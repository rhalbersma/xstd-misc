# `<xstd/misc/concepts/specialization_of.hpp>`

## `xstd::specialization_of`

```cpp
template<class T, template<class...> class Primary>
concept specialization_of = is_specialization_of_v<remove_const_t<T>, Primary>;
```

A specialization of the class template `Primary`, whose parameters are all types, `const` or not.

### Models

- A specialization of `Primary` models it, and so does that specialization `const`.
- A reference to a specialization does not.
- An adaptor constrained on it accepts a `const` owner and a mutable one.

### Example

```cpp
--8<-- "examples/concepts/specialization_of.cpp"
```

Output:

```text
--8<-- "examples/concepts/specialization_of.out"
```

### See also

| Entity | Relation |
| :----- | :------- |
| [`is_specialization_of`](../type_traits/is_specialization_of.md) | the exact trait it reads, for which a `const` specialization is not one |
