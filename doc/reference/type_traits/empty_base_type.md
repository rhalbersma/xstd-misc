# `<xstd/misc/type_traits/empty_base_type.hpp>`

## `empty_base_type`

- An empty, regular, trivial class, whose special members do not throw.
- `Tag` defaults to `void`, and distinct tags give distinct types, so a class can derive from more than one.
- It declares no comparison, so none reaches a class derived from it.
