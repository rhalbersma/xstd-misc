# `<xstd/misc/type_traits/no_unique_address.hpp>`

## `XSTD_NO_UNIQUE_ADDRESS`

```cpp
#define XSTD_NO_UNIQUE_ADDRESS see below
```

A portable spelling of the attribute name that gives `[[no_unique_address]]`'s semantics. It expands to
`msvc::no_unique_address` on MSVC, whose standard spelling is layout-neutral, and to `no_unique_address` elsewhere.

### Notes

An empty member declared with it needs no storage of its own.

### Example

Adapted from the example of
[`[[no_unique_address]]`](https://en.cppreference.com/w/cpp/language/attributes/no_unique_address) on cppreference.com,
at
[revision 174656](https://en.cppreference.com/mwiki/index.php?title=cpp/language/attributes/no_unique_address&oldid=174656),
by cppreference.com contributors. This example is under
[CC-BY-SA 3.0 and the GFDL](../../../examples/cppreference/LICENSE), as cppreference.com's content is; the rest of this
page is under the [Boost Software License](../../../LICENSE_1_0.txt).

```cpp
--8<-- "examples/cppreference/no_unique_address.cpp"
```

Output:

```text
--8<-- "examples/cppreference/no_unique_address.out"
```

### See also

| Entity | Relation |
| :----- | :------- |
| [`empty_member_type`](empty_member_type.md) | the empty type such a member usually has |
| [`[[no_unique_address]]`](https://en.cppreference.com/w/cpp/language/attributes/no_unique_address) | the standard attribute it spells |
