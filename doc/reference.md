# Reference

Each public entity of xstd-misc is specified here the way the standard specifies the library: a synopsis per header,
then for each entity the elements of [[structure.specifications]](https://eel.is/c++draft/structure.specifications) that apply. The baseline is
[C++20](https://wg21.link/N4861), and a name with no namespace is in `std`.

The synopses leave out `[[nodiscard]]`, as the standard's own have since [P2422R1](https://github.com/cplusplus/papers/issues/1744), which made
it a recommended practice rather than part of a library's specification. The code carries it on every function that
returns a value and on every constructor, so discarding any result here draws a warning.

## `<xstd/misc/concepts.hpp>`

```cpp
namespace xstd {
  template<class R, class T>
    concept container_compatible_range = see below;
  template<class H>
    concept hash_algorithm = see below;
  template<class R, class V>
    concept proxy_semi_reference = see below;
  template<class I>
    concept proxy_semi_iterator = see below;
  template<class R>
    concept proxy_reference = see below;
  template<class I>
    concept proxy_iterator = see below;
  template<class Alloc>
    concept simple_allocator = see below;
  template<class T, template<class...> class Primary>
    concept specialization_of = see below;
}
```

The umbrella exports each header below.

### `container_compatible_range`

```cpp
template<class R, class T>
concept container_compatible_range =
  ranges::input_range<R> and convertible_to<ranges::range_reference_t<R>, T>;
```

- The definition is the working draft's exposition-only *container-compatible-range*,
  [[container.intro.reqmts]/2](https://eel.is/c++draft/container.intro.reqmts#2), word for word.
- An input range of references that convert to `T` models it, and nothing else does.

### `simple_allocator`

```cpp
template<class Alloc>
concept simple_allocator =
  requires(Alloc alloc, size_t n) {
    { *alloc.allocate(n) } -> same_as<typename Alloc::value_type&>;
    { alloc.deallocate(alloc.allocate(n), n) };
  } and copy_constructible<Alloc> and equality_comparable<Alloc>;
```

- The definition is the working draft's exposition-only *simple-allocator*,
  [[allocator.requirements.general]/99](https://eel.is/c++draft/allocator.requirements.general#99), word for word.
- A type models it if it meets the requirements of
  [[allocator.requirements.general]](https://eel.is/c++draft/allocator.requirements.general).

### `hash_algorithm`

```cpp
template<class H>
concept hash_algorithm =
  semiregular<H> and
  constructible_from<H, uint64_t> and
  constructible_from<H, const unsigned char*, size_t> and
  requires(H& h, const void* p, size_t n) {
    typename H::result_type;
    h.update(p, n);
    { h.result() } -> same_as<typename H::result_type>;
  } and
  ((unsigned_integral<typename H::result_type> and not same_as<typename H::result_type, bool>) or
   (boost::hash2::has_constant_size<typename H::result_type>::value and
    ranges::contiguous_range<typename H::result_type> and
    same_as<ranges::range_value_t<typename H::result_type>, unsigned char>)) and
  (not requires { H::block_size; } or same_as<remove_cv_t<decltype(H::block_size)>, size_t>);
```

- Every [Boost.Hash2](https://www.boost.org/doc/libs/latest/libs/hash2/) algorithm models it.
- Each form a result and a block size take in Hash2 satisfies it: an unsigned integer or a fixed-size byte array,
  with a block size or none.
- A hasher, which hashes a key rather than updating with bytes, does not satisfy it.
- A `bool` result is turned away.
- A `block_size` of a type other than `size_t` is turned away.
- A type models it if it meets [Boost.Hash2's documented requirements](https://www.boost.org/doc/libs/latest/libs/hash2/doc/html/hash2.html#hashing_bytes_hash_algorithm_requirements) on an algorithm:
  - the split of a message into calls to `update` does not affect the result.
  - a copy is equivalent to the original.
  - seed 0, and an empty byte seed, are default construction, and distinct seeds give distinct states.
  - each further call to `result` continues a sequence that an equal state repeats.

### `proxy_semi_reference`

```cpp
template<class R, class V>
concept proxy_semi_reference = is_class_v<R> and (not same_as<R, V>) and convertible_to<R, V>;
```

- Every library's `vector<bool>::reference` and `bitset<N>::reference` models it for `bool`.
- A class that converts to the named `V` models it, with or without a way back from its address.
- `V` itself, a scalar, a class naming a value it does not convert to, and a language reference do not.

### `proxy_semi_iterator`

```cpp
template<class I>
concept proxy_semi_iterator =
  indirectly_readable<I> and proxy_semi_reference<iter_reference_t<I>, iter_value_t<I>>;
```

- An iterator whose reference is a class prvalue standing in for its value type models it: `vector<bool>`'s
  iterator, `zip_view`'s.
- A pointer, an iterator yielding a language reference, and one yielding its value type by value do not.

### `proxy_reference`

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

### `proxy_iterator`

```cpp
template<class I>
concept proxy_iterator = proxy_semi_iterator<I> and proxy_reference<iter_reference_t<I>>;
```

- A semi-iterator whose reference is a proxy reference models it.
- A semi-iterator whose reference keeps the built-in address does not.
- *Remarks:* It subsumes `proxy_semi_iterator<I>`.

### `specialization_of`

```cpp
template<class T, template<class...> class Primary>
concept specialization_of = is_specialization_of_v<remove_const_t<T>, Primary>;
```

- A specialization of `Primary` models it, and so does that specialization `const`.
- A reference to a specialization does not.
- An adaptor constrained on it accepts a `const` owner and a mutable one.

## `<xstd/misc/type_traits.hpp>`

```cpp
#define XSTD_NO_UNIQUE_ADDRESS see below

namespace xstd {
  template<class Tag = void>
    struct empty_base_type {};
  template<class Tag = void>
    struct empty_member_type;
  template<bool Condition, class Type, class Tag>
    using conditional_data_member = conditional<Condition, Type, empty_member_type<Tag>>;
  template<bool Condition, class Type, class Tag>
    using conditional_data_member_t = conditional_t<Condition, Type, empty_member_type<Tag>>;
  template<class T, template<class...> class Primary>
    constexpr bool is_specialization_of_v = see below;
  template<class T, template<class...> class Primary>
    using is_specialization_of = bool_constant<is_specialization_of_v<T, Primary>>;
  template<class H>
    struct hash_algorithm_is_avalanching;
  template<class H>
    constexpr bool hash_algorithm_is_avalanching_v = hash_algorithm_is_avalanching<H>::value;
}
```

The umbrella's facilities compose into a member present only under a condition, and the trait recognises its absent
branch.

### `XSTD_NO_UNIQUE_ADDRESS`

- Expands to the attribute name that gives `[[no_unique_address]]`'s semantics: `msvc::no_unique_address` on MSVC,
  whose standard spelling is layout-neutral, and `no_unique_address` elsewhere. An empty member declared with it
  needs no storage of its own.

### `empty_base_type`

- An empty, regular, trivial class, whose special members do not throw.
- `Tag` defaults to `void`, and distinct tags give distinct types, so a class can derive from more than one.
- It declares no comparison, so none reaches a class derived from it.

### `empty_member_type`

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

### `conditional_data_member`, `conditional_data_member_t`

- `Type` when `Condition` holds, and otherwise `empty_member_type<Tag>`.

### `is_specialization_of_v`, `is_specialization_of`

- `true` exactly when `T` is `Primary<Args...>` for some `Args`, for a primary template whose parameters are all
  types.
- A constrained primary template is no obstacle.
- The question is exact: a `const` specialization is not one.
- The concept and the trait agree.

### `hash_algorithm_is_avalanching`, `hash_algorithm_is_avalanching_v`

- A [*Cpp17UnaryTypeTrait*](https://eel.is/c++draft/meta.rqmts) whose base characteristic is `true_type` for every Boost.Hash2 algorithm but `fnv1a_32` and
  `fnv1a_64`, and for `boost::hash2::hmac<H>` exactly where it is for `H`, and `false_type` otherwise.
- `true` means that flipping any one bit of the input flips each bit of the result with probability ½, measured
  through `hash`'s whole path: the seed, `hash_append` and the fold to `size_t`.
- *Remarks:* A program may specialize it for an algorithm of its own.

## `<xstd/misc/functional.hpp>`

```cpp
namespace xstd {
  template<class T, hash_algorithm H64 = boost::hash2::xxhash_64, hash_algorithm H32 = H64,
           class Flavor = boost::hash2::default_flavor>
      requires (sizeof(typename H32::result_type) <= sizeof(typename H64::result_type))
    class hash;
  template<class CharT, class Traits, class Allocator, hash_algorithm H64, hash_algorithm H32, class Flavor>
      requires (sizeof(typename H32::result_type) <= sizeof(typename H64::result_type))
    class hash<basic_string<CharT, Traits, Allocator>, H64, H32, Flavor>;
  template<class T, hash_algorithm H = boost::hash2::xxh3_128, class Flavor = boost::hash2::little_endian_flavor>
    class digest;
  template<class CharT, class Traits, class Allocator, hash_algorithm H, class Flavor>
    class digest<basic_string<CharT, Traits, Allocator>, H, Flavor>;
}
```

Where the digest is as wide as `size_t`, the hash is the digest unchanged.

### `hash`

```cpp
template<class T, hash_algorithm H64, hash_algorithm H32, class Flavor>
class hash {
public:
  using hash_type      = conditional_t<numeric_limits<size_t>::digits <= 32, H32, H64>;
  using is_avalanching = bool_constant<hash_algorithm_is_avalanching_v<hash_type>>;

  hash() = default;
  constexpr explicit hash(uint64_t seed);
  constexpr hash(const unsigned char* p, size_t n);

  constexpr auto operator()(const T& v) const -> size_t;
};
```

The class holds a prototype of `hash_type`.

- `H64` and `H32` are constrained to model `hash_algorithm`, so a second key type in either place is rejected.
- `H64` defaults to `xxhash_64`, `H32` to `H64` and `Flavor` to `default_flavor`, which appends in the target's own
  byte order.
- `hash_type` is `H32` where `size_t` is at most 32 bits, and `H64` otherwise. One algorithm named alone runs on
  every target.
- *Remarks:* A pair whose 32-bit algorithm is named first, such as `xxhash_32, xxhash_64`, is rejected.
- `is_avalanching` is what [`boost::hash_is_avalanching`](https://www.boost.org/doc/libs/latest/libs/container_hash/doc/html/hash.html#ref_hash_is_avalanchinghash) reads.
- *Constructors, Effects:* Value-initializes the prototype, or initializes it from `seed`, or from the `n` bytes at
  `p`. Each is a constant expression, and the integer one is `explicit`.
- `operator()`:
  - *Effects:* Copies the prototype to `h` and calls `boost::hash2::hash_append(h, Flavor(), v)`. The prototype is
    unchanged.
  - *Returns:* `boost::hash2::get_integral_result<size_t>(h)`.
  - An argument converts to `T` before it is appended, so `42` hashes as the `int64_t` a table holds.
  - *Throws:* Any exception the key's `hash_append` throws. It is not `noexcept`.
  - *Remarks:* It is unconstrained: Hash2 declares `hash_append` for every type, so a constraint would admit keys it
    then rejects, such as `thread::id`.
  - *Remarks:* As of Boost.Hash2 1.92 a call is not a constant expression, since
    [`get_integral_result`](https://www.boost.org/doc/libs/latest/libs/hash2/doc/html/hash2.html#ref_get_integral_result) is not `constexpr`.
- Equal values hash equal.
- Under FNV-1a, keys that differ only in their high bits share a bucket wherever a table masks the hash, and spread
  in a `boost::unordered_flat_set`, which mixes it first.
- It serves as the hash of `unordered_set` and `boost::unordered_flat_set`, seeded per container.

### `hash<basic_string<CharT, Traits, Allocator>, H64, H32, Flavor>`

As the primary template, with `is_transparent` declared and `operator()` taking `basic_string_view<CharT, Traits>`.

- Only a string key is transparent: equal values of other types need not append the same bytes.
- A string, its view and a C string hash the same.
- A set of strings finds a key by its view, or by a literal, without building a string.

### `digest`

```cpp
template<class T, hash_algorithm H, class Flavor>
class digest {
public:
  using result_type = typename H::result_type;

  digest() = default;
  constexpr explicit digest(uint64_t seed);
  constexpr digest(const unsigned char* p, size_t n);

  constexpr auto operator()(const T& v) const -> result_type;
};
```

- `H` is constrained to model `hash_algorithm`. A `digest` does not model it: it takes a key rather than updates.
- `H` defaults to `xxh3_128` and `Flavor` to `little_endian_flavor`, so a digest is the same on every target.
- *Constructors:* As for `hash`.
- `operator()`:
  - *Returns:* `h.result()`, where `h` is a copy of the prototype after `boost::hash2::hash_append(h, Flavor(), v)`.
  - An argument converts to `T` before it is appended.
  - *Throws:* Any exception the key's `hash_append` throws. It is not `noexcept`.
- The digests of fixed-width keys under the default algorithm match the reference xxHash implementation's.

### `digest<basic_string<CharT, Traits, Allocator>, H, Flavor>`

As the primary template, with `operator()` taking `basic_string_view<CharT, Traits>`. A string, its view and a C
string digest the same.

## `<xstd/misc/format.hpp>`

```cpp
namespace std {
  template<size_t N>
    struct formatter<boost::hash2::digest<N>>;
}
```

The umbrella exports it.

### `formatter<boost::hash2::digest<N>>`

- `parse`:
  - *Effects:* Accepts an empty specification, `x` or `X`.
  - *Throws:* `format_error` for any other specification.
- `format`:
  - *Effects:* Writes two hexadecimal digits per byte, in byte order, lowercase unless the specification is `X`.

## `<xstd/misc/utility.hpp>`

```cpp
namespace xstd {
  template<class Enum>
    constexpr auto to_underlying(Enum value) noexcept -> underlying_type_t<Enum>;
  template<class Enum, Enum N>
    requires is_enum_v<Enum>
    constexpr auto to_underlying(integral_constant<Enum, N>) noexcept;
  template<proxy_reference R>
    constexpr auto to_underlying(R ref) noexcept(see below) -> underlying_type_t<iter_value_t<decltype(&ref)>>;
}
```

The umbrella exports it.

### `to_underlying`

- `to_underlying(Enum value)`:
  - *Remarks:* A type `Enum` that is not an enumeration is a substitution failure, since `underlying_type_t<Enum>`
    names no type for it.
  - *Returns:* `static_cast<underlying_type_t<Enum>>(value)`, the Returns element of [[utility.underlying]/1](https://eel.is/c++draft/utility.underlying#1).
  - It is `noexcept`, as is the next form.
- `to_underlying(integral_constant<Enum, N>)`:
  - *Returns:* `integral_constant<underlying_type_t<Enum>, to_underlying(N)>()`, the value kept at the type level.
  - A non-enumeration constant is a substitution failure.
- `to_underlying(R ref)`:
  - *Constraints:* `R` models `proxy_reference`.
  - *Returns:* `to_underlying(static_cast<V>(ref))`, where `V` is that value type.
  - *Remarks:* A value type `V` that is not an enumeration is a substitution failure, through the return type
    `underlying_type_t<V>`. The exception specification is `is_nothrow_convertible_v<R, V>`.
