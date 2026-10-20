# `<xstd/misc/type_traits/empty_member_type.hpp>`

## `empty_member_type`

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

- An empty, regular, totally ordered, trivial class, whose special members do not throw, usable in constant
  expressions.
- Every two of its values compare equal and equivalent.
- `Tag` defaults to `void`, and distinct tags give distinct types.
- Its comparisons reach a class derived from it, and not a class that holds it as a member.
- The variadic constructor ignores its arguments, so an enclosing class can construct the member as it would the
  type it stands in for. *Constraints:* no `Args` is the class itself, so copying from a mutable lvalue stays the
  trivial copy rather than the variadic constructor, which would match that lvalue better.
