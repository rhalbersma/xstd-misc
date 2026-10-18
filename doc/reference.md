# Reference

Each public entity of xstd-misc is specified here the way the standard specifies the library: a synopsis per header,
then for each entity the elements of [structure.specifications] that apply. The baseline is
[C++20](https://wg21.link/N4861), and a name with no namespace is in `std`.

Every element names the test that holds the code to it, as `test-file: TestCase` under `test/src/misc/`. An element
that no test can hold names why instead: either no existing type makes the mistake it turns away, which the
repository's test rules then leave to the definition, or the element copies the working draft, which
`tools/check-draft-wording.py` checks. An element that disagrees with the code is a defect in one of the two.

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

The umbrella exports each header below. *Test:* `concepts.cpp: ReExportsTheWholeDirectory`.

### `container_compatible_range`

```cpp
template<class R, class T>
concept container_compatible_range =
  ranges::input_range<R> and convertible_to<ranges::range_reference_t<R>, T>;
```

- The definition is the working draft's exposition-only *container-compatible-range*, at the paragraph the header
  cites. *Checked by* `tools/check-draft-wording.py`.
- An input range of references that convert to `T` models it, and nothing else does.
  *Test:* `concepts/container_compatible_range.cpp: AnInputRangeOfConvertibleReferencesModelsIt`.

### `simple_allocator`

```cpp
template<class Alloc>
concept simple_allocator =
  requires(Alloc alloc, size_t n) {
    { *alloc.allocate(n) } -> same_as<typename Alloc::value_type&>;
    { alloc.deallocate(alloc.allocate(n), n) };
  } and copy_constructible<Alloc> and equality_comparable<Alloc>;
```

- The definition is the working draft's exposition-only *simple-allocator*, at the paragraph the header cites.
  *Checked by* `tools/check-draft-wording.py`.
- A type models it if it meets the requirements of [allocator.requirements.general].
  *Test:* `concepts/simple_allocator.cpp: TheStandardAllocatorsModelIt`, over the standard's allocators, and against
  a comparator and a plain type.

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

- Every Boost.Hash2 algorithm models it.
  *Test:* `concepts/hash_algorithm.cpp: EveryHash2AlgorithmIsAHashAlgorithm`.
- Each form a result and a block size take in Hash2 satisfies it: an unsigned integer or a fixed-size byte array,
  with a block size or none.
  *Test:* `concepts/hash_algorithm.cpp: EachFormHash2TakesIsAHashAlgorithm`.
- A hasher, which hashes a key rather than updating with bytes, does not satisfy it.
  *Test:* `functional/hash.cpp: AHashIsNotAHashAlgorithm`; `functional/digest.cpp: ADigestIsNotAHashAlgorithm`.
- A `bool` result is turned away. *Definition only:* no existing algorithm has one.
- A `block_size` of a type other than `size_t` is turned away. *Definition only:* no existing algorithm has one.
- A type models it if it meets Boost.Hash2's documented requirements on an algorithm:
  - the split of a message into calls to `update` does not affect the result.
    *Test:* `concepts/hash_algorithm.cpp: UpdateIsBlindToHowTheMessageIsSplit`.
  - a copy is equivalent to the original.
    *Test:* `concepts/hash_algorithm.cpp: ACopyIsEquivalentToTheOriginal`.
  - seed 0, and an empty byte seed, are default construction, and distinct seeds give distinct states.
    *Test:* `concepts/hash_algorithm.cpp: AnIntegerSeedInitializesTheStateItNames`,
    `AByteSeedInitializesTheStateItNames`.
  - each further call to `result` continues a sequence that an equal state repeats.
    *Test:* `concepts/hash_algorithm.cpp: ResultCanBeCalledAgain`.

### `proxy_semi_reference`

```cpp
template<class R, class V>
concept proxy_semi_reference = is_class_v<R> and (not same_as<R, V>) and convertible_to<R, V>;
```

- Every library's `vector<bool>::reference` and `bitset<N>::reference` models it for `bool`.
  *Test:* `concepts/proxy_semi_reference.cpp: TheStandardBitReferencesStandInForBool`.
- A class that converts to the named `V` models it, with or without a way back from its address.
  *Test:* `concepts/proxy_semi_reference.cpp: AClassConvertingToTheNamedValueIsOne`.
- `V` itself, a scalar, a class naming a value it does not convert to, and a language reference do not.
  *Test:* `concepts/proxy_semi_reference.cpp: TheValueAScalarOrAnotherValueIsNone`.

### `proxy_semi_iterator`

```cpp
template<class I>
concept proxy_semi_iterator =
  indirectly_readable<I> and proxy_semi_reference<iter_reference_t<I>, iter_value_t<I>>;
```

- An iterator whose reference is a class prvalue standing in for its value type models it: `vector<bool>`'s
  iterator, `zip_view`'s.
  *Test:* `concepts/proxy_semi_iterator.cpp: AClassPrvalueStandingInForTheValueIsAProxy`.
- A pointer, an iterator yielding a language reference, and one yielding its value type by value do not.
  *Test:* `concepts/proxy_semi_iterator.cpp: AReferenceOrTheValueItselfIsNoProxy`.

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
  *Test:* `concepts/proxy_reference.cpp: AWritableProxyClosedUnderAddressAndDereferenceIsOne`.
- A library's bit reference models it exactly where the library gives it an `operator&` and an assignment through
  `const`. *Test:* `concepts/proxy_reference.cpp:
  ALibraryBitReferenceIsOneWhereItsAddressRoundTripsAndItWritesThrough`.
- A read-only proxy, such as `vector<bool>::const_reference`, does not.
  *Test:* `concepts/proxy_reference.cpp: AReadOnlyBitReferenceIsNone`.
- A class that converts without the round trip does not.
  *Test:* `concepts/proxy_reference.cpp: AConversionWithoutTheRoundTripIsNone`.
- An address that dereferences to the value rather than back to the proxy is turned away.
  *Definition only:* no existing type has one.
- *Remarks:* A type that models it is a semi-reference for the value type its address names, and its address
  models `proxy_iterator`.
  *Test:* `concepts/proxy_reference.cpp: IsASemiReferenceWhoseAddressIsAProxyIterator`.
- *Remarks:* It does not subsume `proxy_semi_reference<R, V>`, whose `V` the caller names: overloads constrained on
  each are ambiguous for a type that satisfies both, and `and (not proxy_reference<R>)` on the weaker one resolves
  them. *Test:* `concepts/proxy_reference.cpp: DoesNotSubsumeTheSemiReference`.

### `proxy_iterator`

```cpp
template<class I>
concept proxy_iterator = proxy_semi_iterator<I> and proxy_reference<iter_reference_t<I>>;
```

- A semi-iterator whose reference is a proxy reference models it.
  *Test:* `concepts/proxy_iterator.cpp: ItsReferenceIsAProxyReference`.
- A semi-iterator whose reference keeps the built-in address does not.
  *Test:* `concepts/proxy_iterator.cpp: ASemiIteratorWithoutTheRoundTripIsNone`.
- *Remarks:* It subsumes `proxy_semi_iterator<I>`. *Test:* `concepts/proxy_iterator.cpp: SubsumesTheSemiIterator`.

### `specialization_of`

```cpp
template<class T, template<class...> class Primary>
concept specialization_of = is_specialization_of_v<remove_const_t<T>, Primary>;
```

- A specialization of `Primary` models it, and so does that specialization `const`.
  *Test:* `concepts/specialization_of.cpp: ConstrainsToSpecializationsOfThePrimaryTemplate`,
  `TheConstraintTakesAConstOwner`.
- A reference to a specialization does not. *Test:* `concepts/specialization_of.cpp: TheConstraintTakesAConstOwner`.
- An adaptor constrained on it accepts a `const` owner and a mutable one.
  *Test:* `concepts/specialization_of.cpp: AnAdaptorTakesAConstOwnerAndAMutableOne`.

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
branch. *Test:* `type_traits.cpp: TheStorageUtilitiesComposeIntoAnOptionalMember`,
`TheAbsentBranchIsAnEmptyTypeTheTraitRecognises`.

### `XSTD_NO_UNIQUE_ADDRESS`

- Expands to the attribute name that gives `[[no_unique_address]]`'s semantics: `msvc::no_unique_address` on MSVC,
  whose standard spelling is layout-neutral, and `no_unique_address` elsewhere. An empty member declared with it
  needs no storage of its own. *Test:* `type_traits/no_unique_address.cpp: EmptyMemberNeedsNoAdditionalStorage`.

### `empty_base_type`

- An empty, regular, trivial class, whose special members do not throw.
  *Test:* `type_traits/empty_base_type.cpp: IsEmpty`, `IsRegular`, `IsTrivial`, `IsNoThrow`.
- `Tag` defaults to `void`, and distinct tags give distinct types, so a class can derive from more than one.
  *Test:* `type_traits/empty_base_type.cpp: TagDefaultsToVoid`, `DifferentTagsGiveDistinctTypes`.
- It declares no comparison, so none reaches a class derived from it.
  *Test:* `type_traits/empty_base_type.cpp: ItHasNoComparisonAndHandsNoneToADerivedClass`.

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
  expressions. *Test:* `type_traits/empty_member_type.cpp: IsEmpty`, `IsRegular`, `IsTotallyOrdered`, `IsTrivial`,
  `IsNoThrow`, `IsUsableInConstantExpressions`.
- Every two of its values compare equal and equivalent.
  *Test:* `type_traits/empty_member_type.cpp: InstancesAreEqual`, `InstancesAreEquallyOrdered`.
- `Tag` defaults to `void`, and distinct tags give distinct types.
  *Test:* `type_traits/empty_member_type.cpp: TagDefaultsToVoid`, `DifferentTagsGiveDistinctTypes`.
- Its comparisons reach a class derived from it, and not a class that holds it as a member.
  *Test:* `type_traits/empty_member_type.cpp: ItsComparisonReachesADerivedTypeAndNotAnEnclosingOne`.
- The variadic constructor ignores its arguments, so an enclosing class can construct the member as it would the
  type it stands in for. *Constraints:* no `Args` is the class itself, so copy and move construction are not taken
  over. *Definition only:* the copy and move constructors are trivial either way, so no existing type observes it.

### `conditional_data_member`, `conditional_data_member_t`

- `Type` when `Condition` holds, and otherwise `empty_member_type<Tag>`.
  *Test:* `type_traits/conditional_data_member.cpp: SelectsTheMemberOrATaggedEmptyStandIn`.

### `is_specialization_of_v`, `is_specialization_of`

- `true` exactly when `T` is `Primary<Args...>` for some `Args`, for a primary template whose parameters are all
  types. *Test:* `type_traits/is_specialization_of.cpp: TypesOnly`.
- A constrained primary template is no obstacle.
  *Test:* `type_traits/is_specialization_of.cpp: AConstrainedParameterIsNoObstacle`.
- The question is exact: a `const` specialization is not one.
  *Test:* `type_traits/is_specialization_of.cpp: TheTraitIsExact`.
- The concept and the trait agree. *Test:* `misc.cpp: TheConceptAndTheTraitAreOnePredicate`.

### `hash_algorithm_is_avalanching`, `hash_algorithm_is_avalanching_v`

- A *Cpp17UnaryTypeTrait* whose base characteristic is `true_type` for every Boost.Hash2 algorithm but `fnv1a_32` and
  `fnv1a_64`, and for `boost::hash2::hmac<H>` exactly where it is for `H`, and `false_type` otherwise.
  *Test:* `type_traits/hash_algorithm_is_avalanching.cpp: EveryHash2AlgorithmButFnv1aAvalanches`.
- `true` means that flipping any one bit of the input flips each bit of the result with probability ½, measured
  through `hash`'s whole path: the seed, `hash_append` and the fold to `size_t`.
  *Test:* `type_traits/hash_algorithm_is_avalanching.cpp: AgreesWithTheAvalancheCriterion`.
- *Remarks:* A program may specialize it for an algorithm of its own.
  *Test:* `type_traits/hash_algorithm_is_avalanching.cpp: AnAlgorithmOutsideHash2AvalanchesOnceItsAuthorSaysSo`.

## `<xstd/misc/functional.hpp>`

```cpp
namespace xstd {
  template<class T, hash_algorithm H = boost::hash2::xxhash_64, class Flavor = boost::hash2::default_flavor>
    class hash;
  template<class CharT, class Traits, class Allocator, hash_algorithm H, class Flavor>
    class hash<basic_string<CharT, Traits, Allocator>, H, Flavor>;
  template<class T, hash_algorithm H = boost::hash2::xxh3_128, class Flavor = boost::hash2::little_endian_flavor>
    class digest;
  template<class CharT, class Traits, class Allocator, hash_algorithm H, class Flavor>
    class digest<basic_string<CharT, Traits, Allocator>, H, Flavor>;
}
```

Where the digest is as wide as `size_t`, the hash is the digest unchanged.
*Test:* `functional.cpp: AHashIsTheDigestAtTheWidthOfSizeT`.

### `hash`

```cpp
template<class T, hash_algorithm H, class Flavor>
class hash {
public:
  using is_avalanching = bool_constant<hash_algorithm_is_avalanching_v<H>>;

  hash() = default;
  constexpr explicit hash(uint64_t seed);
  constexpr hash(const unsigned char* p, size_t n);

  constexpr auto operator()(const T& v) const -> size_t;
};
```

The class holds a prototype of `H`.

- `H` is constrained to model `hash_algorithm`, so a second key type in its place is rejected.
  *Test:* `functional/hash.cpp: TheAlgorithmIsAHashAlgorithm`.
- `H` defaults to `xxhash_64` and `Flavor` to `default_flavor`, which appends in the target's own byte order.
  *Test:* `functional/hash.cpp: TheDefaultAlgorithmIsXxhash64InNativeBytes`.
- `is_avalanching` is what `boost::hash_is_avalanching` reads.
  *Test:* `functional/hash.cpp: ReportsWhetherItsAlgorithmAvalanches`.
- *Constructors, Effects:* Value-initializes the prototype, or initializes it from `seed`, or from the `n` bytes at
  `p`. Each is a constant expression, and the integer one is `explicit`.
  *Test:* `functional/hash.cpp: IsConstructibleInConstantExpressions`, `SeedsAreExplicit`, `SeedsChangeTheResult`.
- `operator()`:
  - *Effects:* Copies the prototype to `h` and calls `boost::hash2::hash_append(h, Flavor(), v)`. The prototype is
    unchanged. *Test:* `functional/hash.cpp: EachCallStartsFromThePrototype`, `HashesItsKeyAsHash2Does`.
  - *Returns:* `boost::hash2::get_integral_result<size_t>(h)`. *Test:* `functional/hash.cpp: IsOneValuePerWidthOfSizeT`.
  - An argument converts to `T` before it is appended, so `42` hashes as the `int64_t` a table holds.
    *Test:* `functional/hash.cpp: ConvertsEachArgumentToItsKeyType`.
  - *Throws:* Any exception the key's `hash_append` throws. It is not `noexcept`.
    *Test:* `functional/hash.cpp: TheCallIsNotNoexcept`.
  - *Remarks:* It is unconstrained: Hash2 declares `hash_append` for every type, so a constraint would admit keys it
    then rejects, such as `thread::id`. *Test:* `functional/hash.cpp: HashAppendIsDeclaredForKeysHash2CannotHash`.
  - *Remarks:* As of Boost.Hash2 1.92 a call is not a constant expression, since `get_integral_result` is not
    `constexpr`. *Untested:* a probe would need a lambda in a constraint, which compilers treat inconsistently.
- Equal values hash equal. *Test:* `functional/hash.cpp: EqualValuesHashEqual`.
- Under FNV-1a, keys that differ only in their high bits share a bucket wherever a table masks the hash, and spread
  in a `boost::unordered_flat_set`, which mixes it first.
  *Test:* `functional/hash.cpp: TaggedIdsShareABucketUnderFnv1aWhereATableMasksTheirHash`,
  `TaggedIdsSpreadUnderFnv1aWhereABoostUnorderedFlatSetMixesTheirHash`.
- It serves as the hash of `unordered_set` and `boost::unordered_flat_set`, seeded per container.
  *Test:* `functional/hash.cpp: SeedsAStdUnorderedSet`, `SeedsABoostUnorderedFlatSet`.

### `hash<basic_string<CharT, Traits, Allocator>, H, Flavor>`

As the primary template, with `is_transparent` declared and `operator()` taking `basic_string_view<CharT, Traits>`.

- Only a string key is transparent: equal values of other types need not append the same bytes.
  *Test:* `functional/hash.cpp: IsTransparentOverStringsOnly`.
- A string, its view and a C string hash the same.
  *Test:* `functional/hash.cpp: AStringHashesAsItsViewAndItsCString`.
- A set of strings finds a key by its view, or by a literal, without building a string.
  *Test:* `functional/hash.cpp: FindsAStringKeyByItsViewInAStdUnorderedSet`,
  `FindsAStringKeyByItsViewInABoostUnorderedFlatSet`.

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
  *Test:* `functional/digest.cpp: ADigestIsNotAHashAlgorithm`.
- `H` defaults to `xxh3_128` and `Flavor` to `little_endian_flavor`, so a digest is the same on every target.
  *Test:* `functional/digest.cpp: TheDefaultAlgorithmIsXxh3_128InLittleEndianBytes`,
  `TheFlavorFixesTheByteOrder`.
- *Constructors:* As for `hash`. *Test:* `functional/digest.cpp: SeedsAreExplicit`.
- `operator()`:
  - *Returns:* `h.result()`, where `h` is a copy of the prototype after `boost::hash2::hash_append(h, Flavor(), v)`.
    *Test:* `functional/digest.cpp: IsWhatHash2Gives`.
  - An argument converts to `T` before it is appended.
    *Test:* `functional/digest.cpp: ConvertsEachArgumentToItsKeyType`.
  - *Throws:* Any exception the key's `hash_append` throws. It is not `noexcept`.
    *Test:* `functional/digest.cpp: TheCallIsNotNoexcept`.
- The digests of fixed-width keys under the default algorithm match the reference xxHash implementation's.
  *Test:* `functional/digest.cpp: PersistedDigestsMatchTheReferenceImplementation`.

### `digest<basic_string<CharT, Traits, Allocator>, H, Flavor>`

As the primary template, with `operator()` taking `basic_string_view<CharT, Traits>`. A string, its view and a C
string digest the same. *Test:* `functional/digest.cpp: AStringDigestsAsItsViewAndItsCString`.

## `<xstd/misc/format.hpp>`

```cpp
namespace std {
  template<size_t N>
    struct formatter<boost::hash2::digest<N>>;
}
```

The umbrella exports it. *Test:* `format.cpp: ReExportsTheWholeDirectory`.

### `formatter<boost::hash2::digest<N>>`

- `parse`:
  - *Effects:* Accepts an empty specification, `x` or `X`.
    *Test:* `format/digest.cpp: PrintsLowercaseHexInByteOrder`, `PrintsUppercaseHexWithX`,
    `ParsesASpecificationThatEndsItsContext`.
  - *Throws:* `format_error` for any other specification.
    *Test:* `format/digest.cpp: RejectsAnyOtherSpecification`.
- `format`:
  - *Effects:* Writes two hexadecimal digits per byte, in byte order, lowercase unless the specification is `X`.
    *Test:* `format/digest.cpp: PrintsLowercaseHexInByteOrder`, `PrintsUppercaseHexWithX`, `PrintsEveryByte`.

## `<xstd/misc/utility.hpp>`

```cpp
namespace xstd {
  template<class Enum>
    requires is_enum_v<Enum>
    constexpr auto to_underlying(Enum value) noexcept -> underlying_type_t<Enum>;
  template<class Enum, Enum N>
    requires is_enum_v<Enum>
    constexpr auto to_underlying(integral_constant<Enum, N>) noexcept;
  template<proxy_reference R>
    requires is_enum_v<iter_value_t<decltype(&declval<R&>())>>
    constexpr auto to_underlying(R ref) noexcept(see below) -> underlying_type_t<iter_value_t<decltype(&ref)>>;
}
```

The umbrella exports it. *Test:* `utility.cpp: ReExportsTheWholeDirectory`.

### `to_underlying`

- `to_underlying(Enum value)`:
  - *Constraints:* `Enum` is an enumeration, and the constraint precedes the return type, so any other type is a
    substitution failure. *Test:* `utility/to_underlying.cpp: IsConstrainedToEnumerations`.
  - *Returns:* `static_cast<underlying_type_t<Enum>>(value)`, the Returns element of [utility.underlying].
    *Test:* `utility/to_underlying.cpp: YieldsTheUnderlyingValue`. *Checked by* `tools/check-draft-wording.py`.
  - It is `noexcept`, as is the next form. *Test:* `utility/to_underlying.cpp: YieldsTheUnderlyingValue`.
- `to_underlying(integral_constant<Enum, N>)`:
  - *Returns:* `integral_constant<underlying_type_t<Enum>, to_underlying(N)>()`, the value kept at the type level.
    *Test:* `utility/to_underlying.cpp: YieldsTheUnderlyingValue`.
  - A non-enumeration constant is a substitution failure.
    *Test:* `utility/to_underlying.cpp: IsConstrainedToEnumerations`.
- `to_underlying(R ref)`:
  - *Constraints:* `R` models `proxy_reference`, and the value type its address names is an enumeration.
    *Test:* `utility/to_underlying.cpp: IsConstrainedToProxiesForEnumerations`.
  - *Returns:* `to_underlying(static_cast<V>(ref))`, where `V` is that value type.
    *Test:* `utility/to_underlying.cpp: ReadsThroughAProxyForAnEnumeration`.
  - *Remarks:* The exception specification is `is_nothrow_convertible_v<R, V>`.
    *Test:* `utility/to_underlying.cpp: ReadsThroughAProxyForAnEnumeration`.
