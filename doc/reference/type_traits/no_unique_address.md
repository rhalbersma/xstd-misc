# `<xstd/misc/type_traits/no_unique_address.hpp>`

## `XSTD_NO_UNIQUE_ADDRESS`

- Expands to the attribute name that gives `[[no_unique_address]]`'s semantics: `msvc::no_unique_address` on MSVC,
  whose standard spelling is layout-neutral, and `no_unique_address` elsewhere. An empty member declared with it
  needs no storage of its own.
