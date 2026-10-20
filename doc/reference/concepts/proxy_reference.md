# `<xstd/misc/concepts/proxy_reference.hpp>`

## `proxy_reference`

```cpp
template<class R>
concept proxy_reference =
  requires(R& ref) { { &ref } -> proxy_semi_iterator; } and
  same_as<iter_reference_t<decltype(&declval<R&>())>, R> and
  indirectly_writable<decltype(&declval<R&>()), iter_value_t<decltype(&declval<R&>())>>;
```

- A proxy whose address is a semi-iterator dereferencing back to it, and which writes through to its value when
  `const`, models it.
- A library's bit reference models it exactly where the library gives it an `operator&` and an assignment through
  `const`.
- A read-only proxy, such as `vector<bool>::const_reference`, does not.
- A class that converts without the round trip does not.
- An address that dereferences to the value rather than back to the proxy is turned away.
- *Remarks:* A type that models it is a semi-reference for the value type its address names, and its address
  models `proxy_iterator`.
- *Remarks:* It does not subsume `proxy_semi_reference<R, V>`, whose `V` the caller names: overloads constrained on
  each are ambiguous for a type that satisfies both, and `and (not proxy_reference<R>)` on the weaker one resolves
  them.
