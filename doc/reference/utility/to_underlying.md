# `<xstd/misc/utility/to_underlying.hpp>`

## `to_underlying`

- `to_underlying(Enum value)`:
  - *Remarks:* A type `Enum` that is not an enumeration is a substitution failure, since `underlying_type_t<Enum>`
    names no type for it.
  - *Returns:* `static_cast<underlying_type_t<Enum>>(value)`, the Returns element of [[utility.underlying]/1](https://eel.is/c++draft/utility.underlying#1).
  - It is `noexcept`, as is the next form.
- `to_underlying(integral_constant<Enum, N>)`:
  - *Returns:* `integral_constant<underlying_type_t<Enum>, to_underlying(N)>()`, the value kept at the type level.
  - A non-enumeration constant is a substitution failure.
- `to_underlying(R ref)`:
  - *Constraints:* `R` models `proxy_reference`.
  - *Returns:* `to_underlying(static_cast<V>(ref))`, where `V` is that value type.
  - *Remarks:* A value type `V` that is not an enumeration is a substitution failure, through the return type
    `underlying_type_t<V>`. The exception specification is `is_nothrow_convertible_v<R, V>`.
  - A read-only proxy, such as a set's element, models only `proxy_semi_reference`: convert it first, as
    `to_underlying(static_cast<E>(key))`.
