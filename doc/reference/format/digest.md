# `<xstd/misc/format/digest.hpp>`

## `std::formatter<boost::hash2::digest<N>>`

```cpp
template<size_t N>
struct formatter<boost::hash2::digest<N>>;
```

Formats a Boost.Hash2 digest, such as the result of `xxh3_128` or of `xstd::digest`, as hexadecimal.

### Format specification

| Specification | Output |
| :------------ | :----- |
| empty, or `x` | two lowercase hexadecimal digits per byte, in byte order |
| `X` | two uppercase hexadecimal digits per byte, in byte order |

### Member functions

- `parse`:
    - *Effects:* Accepts an empty specification, `x` or `X`.
    - *Throws:* `format_error` for any other specification.
- `format`:
    - *Effects:* Writes two hexadecimal digits per byte, in byte order, lowercase unless the specification is `X`.

### Example

Ported from the example of [`std::formatter`](https://en.cppreference.com/w/cpp/utility/format/formatter) on
cppreference.com, at
[revision 177164](https://en.cppreference.com/mwiki/index.php?title=cpp/utility/format/formatter&oldid=177164), by
cppreference.com contributors. This example is under
[CC-BY-SA 3.0 and the GFDL](../../../examples/cppreference/LICENSE), as cppreference.com's content is; the rest of this
page is under the [Boost Software License](../../../LICENSE_1_0.txt).

```cpp
--8<-- "examples/cppreference/formatter.cpp"
```

Output:

```text
--8<-- "examples/cppreference/formatter.out"
```

### See also

| Entity | Relation |
| :----- | :------- |
| [`digest`](../functional/digest.md) | the class whose result this formats |
| [`std::formatter`](https://en.cppreference.com/w/cpp/utility/format/formatter) | the standard's formatter, which this specializes |
