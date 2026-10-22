# `<xstd/misc/type_traits/is_specialization_of.hpp>`

## `xstd::is_specialization_of`, `xstd::is_specialization_of_v`

```cpp
template<class T, template<class...> class Primary>
constexpr bool is_specialization_of_v = see below;

template<class T, template<class...> class Primary>
using is_specialization_of = bool_constant<is_specialization_of_v<T, Primary>>;
```

Whether a type is a specialization of a class template whose parameters are all types.

### Value

`true` exactly when `T` is `Primary<Args...>` for some `Args`, and `false` otherwise.

### Notes

- A constrained primary template is no obstacle.
- The question is exact: a `const` specialization is not one.
- A primary template with a value parameter, such as `array`, cannot be named as `Primary`.
- The concept and the trait agree.

### Example

```cpp
--8<-- "examples/type_traits/is_specialization_of.cpp"
```

Output:

```text
--8<-- "examples/type_traits/is_specialization_of.out"
```

### See also

| Entity | Relation |
| :----- | :------- |
| [`specialization_of`](../concepts/specialization_of.md) | the concept, which also admits a `const` specialization |
