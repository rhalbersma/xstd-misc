# `<xstd/misc/format/digest.hpp>`

## `formatter<boost::hash2::digest<N>>`

- `parse`:
  - *Effects:* Accepts an empty specification, `x` or `X`.
  - *Throws:* `format_error` for any other specification.
- `format`:
  - *Effects:* Writes two hexadecimal digits per byte, in byte order, lowercase unless the specification is `X`.
