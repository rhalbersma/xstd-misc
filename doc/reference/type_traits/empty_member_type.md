# `<xstd/misc/type_traits/empty_member_type.hpp>`

## `xstd::empty_member_type`

```cpp
template<class Tag = void>
struct empty_member_type {
  empty_member_type() = default;
  template<class... Args>
    requires ((not same_as<remove_cvref_t<Args>, empty_member_type>) and ...)
    constexpr explicit empty_member_type(Args&&...) noexcept;
  friend auto operator<=>(empty_member_type, empty_member_type) -> strong_ordering = default;
};
```

A tagged empty class that keeps the place of a data member that is not there.

### Template parameters

| Parameter | Meaning |
| :-------- | :------ |
| `Tag` | Distinguishes one empty member from another. Defaults to `void`, and distinct tags give distinct types. |

### Member functions

- The variadic constructor ignores its arguments, so an enclosing class can construct the member as it would the
  type it stands in for.
    - *Constraints:* No `Args` is the class itself, so copying from a mutable lvalue stays the trivial copy rather than
      the variadic constructor, which would match that lvalue better.
- `operator<=>`: every two of its values compare equal and equivalent.

### Notes

- It is an empty, regular, totally ordered, trivial class, whose special members do not throw, usable in constant
  expressions.
- Its comparisons reach a class derived from it, and not a class that holds it as a member.

### See also

| Entity | Relation |
| :----- | :------- |
| [`empty_base_type`](empty_base_type.md) | the same for a base class, without comparisons |
| [`conditional_data_member`](conditional_data_member.md) | picks this type where a condition fails |
| [`XSTD_NO_UNIQUE_ADDRESS`](no_unique_address.md) | lets a member of this type take no storage |
