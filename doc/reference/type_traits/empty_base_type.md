# `<xstd/misc/type_traits/empty_base_type.hpp>`

## `xstd::empty_base_type`

```cpp
template<class Tag = void>
struct empty_base_type {};
```

A tagged empty class that keeps the place of a base class that is not there.

### Template parameters

| Parameter | Meaning |
| :-------- | :------ |
| `Tag` | Distinguishes one empty base from another. Defaults to `void`, and distinct tags give distinct types, so a class can derive from more than one. |

### Notes

- It is an empty, regular, trivial class, whose special members do not throw.
- It declares no comparison, so none reaches a class derived from it.

### See also

| Entity | Relation |
| :----- | :------- |
| [`empty_member_type`](empty_member_type.md) | the same for a data member, and comparable |
| [`conditional_data_member`](conditional_data_member.md) | a member's type, or an empty type where a condition fails |
