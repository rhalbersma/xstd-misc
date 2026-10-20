# `<xstd/misc/utility/to_underlying.hpp>`

## `xstd::to_underlying`

```cpp
// (1)
template<class Enum>
constexpr auto to_underlying(Enum value) noexcept -> underlying_type_t<Enum>;

// (2)
template<class Enum, Enum N>
  requires is_enum_v<Enum>
constexpr auto to_underlying(integral_constant<Enum, N>) noexcept;

// (3)
template<proxy_reference R>
constexpr auto to_underlying(R ref) noexcept(see below) -> underlying_type_t<iter_value_t<decltype(&ref)>>;
```

Converts an enumerator to its underlying type.

1. `std::to_underlying`, for an enumerator.
2. For an enumerator kept at the type level, in an `integral_constant`.
3. For a proxy reference to an enumerator, such as an element of a container of packed enumerators.

### Parameters

| Parameter | Meaning |
| :-------- | :------ |
| `value` | an enumerator |
| `ref` | a proxy reference whose value type `V` is an enumeration |

### Return value

1. `static_cast<underlying_type_t<Enum>>(value)`, the Returns element of [[utility.underlying]/1](https://eel.is/c++draft/utility.underlying#1).
2. `integral_constant<underlying_type_t<Enum>, to_underlying(N)>()`, the value kept at the type level.
3. `to_underlying(static_cast<V>(ref))`, where `V` is the value type of `ref`'s address.

### Exceptions

1. `noexcept`.
2. `noexcept`.
3. `noexcept(is_nothrow_convertible_v<R, V>)`.

### Remarks

1. A type `Enum` that is not an enumeration is a substitution failure, since `underlying_type_t<Enum>` names no type
   for it.
2. A non-enumeration constant is a substitution failure.
3. *Constraints:* `R` models `proxy_reference`. A value type `V` that is not an enumeration is a substitution failure,
   through the return type `underlying_type_t<V>`.

### Notes

A read-only proxy, such as a set's element, models only `proxy_semi_reference`: convert it first, as
`to_underlying(static_cast<E>(key))`.

### See also

| Entity | Relation |
| :----- | :------- |
| [`std::to_underlying`](https://en.cppreference.com/w/cpp/utility/to_underlying) | the standard's function, which (1) is |
| [`proxy_reference`](../concepts/proxy_reference.md) | what (3) asks of its argument |
| [`proxy_semi_reference`](../concepts/proxy_semi_reference.md) | what a read-only proxy models instead |
