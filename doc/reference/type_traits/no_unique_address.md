# `<xstd/misc/type_traits/no_unique_address.hpp>`

## `XSTD_NO_UNIQUE_ADDRESS`

```cpp
#define XSTD_NO_UNIQUE_ADDRESS see below
```

A portable spelling of the attribute name that gives `[[no_unique_address]]`'s semantics. It expands to
`msvc::no_unique_address` on MSVC, whose standard spelling is layout-neutral, and to `no_unique_address` elsewhere.

### Notes

An empty member declared with it needs no storage of its own.

### See also

| Entity | Relation |
| :----- | :------- |
| [`empty_member_type`](empty_member_type.md) | the empty type such a member usually has |
| [`[[no_unique_address]]`](https://en.cppreference.com/w/cpp/language/attributes/no_unique_address) | the standard attribute it spells |
