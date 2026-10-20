# `<xstd/misc/utility.hpp>`

```cpp
namespace xstd {
  template<class Enum>
    constexpr auto to_underlying(Enum value) noexcept -> underlying_type_t<Enum>;
  template<class Enum, Enum N>
    requires is_enum_v<Enum>
    constexpr auto to_underlying(integral_constant<Enum, N>) noexcept;
  template<proxy_reference R>
    constexpr auto to_underlying(R ref) noexcept(see below) -> underlying_type_t<iter_value_t<decltype(&ref)>>;
}
```

| Header | Declares |
| :----- | :------- |
| [`<xstd/misc/utility/to_underlying.hpp>`](to_underlying.md) | `to_underlying` |
