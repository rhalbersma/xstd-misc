# `<xstd/misc/type_traits/conditional_data_member.hpp>`

## `xstd::conditional_data_member`, `xstd::conditional_data_member_t`

```cpp
template<bool Condition, class Type, class Tag>
using conditional_data_member = conditional<Condition, Type, empty_member_type<Tag>>;

template<bool Condition, class Type, class Tag>
using conditional_data_member_t = conditional_t<Condition, Type, empty_member_type<Tag>>;
```

A data member's type where a condition holds, and a tagged empty type where it does not: `Type` when `Condition`
holds, and otherwise `empty_member_type<Tag>`.

### Notes

Declared with `XSTD_NO_UNIQUE_ADDRESS`, a member of this type takes no storage where the condition fails.

### See also

| Entity | Relation |
| :----- | :------- |
| [`empty_member_type`](empty_member_type.md) | the type it picks where the condition fails |
| [`XSTD_NO_UNIQUE_ADDRESS`](no_unique_address.md) | lets that member take no storage |
| [`std::conditional`](https://en.cppreference.com/w/cpp/types/conditional) | the standard trait it is built on |
