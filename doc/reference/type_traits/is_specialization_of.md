# `<xstd/misc/type_traits/is_specialization_of.hpp>`

## `is_specialization_of_v`, `is_specialization_of`

- `true` exactly when `T` is `Primary<Args...>` for some `Args`, for a primary template whose parameters are all
  types.
- A constrained primary template is no obstacle.
- The question is exact: a `const` specialization is not one.
- A primary template with a value parameter, such as `array`, cannot be named as `Primary`.
- The concept and the trait agree.
