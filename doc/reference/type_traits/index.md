# `<xstd/misc/type_traits.hpp>`

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

| Header | Declares |
| :----- | :------- |
| [`<xstd/misc/type_traits/empty_base_type.hpp>`](empty_base_type.md) | `empty_base_type` |
| [`<xstd/misc/type_traits/empty_member_type.hpp>`](empty_member_type.md) | `empty_member_type` |
| [`<xstd/misc/type_traits/no_unique_address.hpp>`](no_unique_address.md) | `XSTD_NO_UNIQUE_ADDRESS` |
| [`<xstd/misc/type_traits/conditional_data_member.hpp>`](conditional_data_member.md) | `conditional_data_member` |
| [`<xstd/misc/type_traits/hash_algorithm_is_avalanching.hpp>`](hash_algorithm_is_avalanching.md) | `hash_algorithm_is_avalanching` |
| [`<xstd/misc/type_traits/is_specialization_of.hpp>`](is_specialization_of.md) | `is_specialization_of` |

The umbrella's facilities compose into a member present only under a condition, and the trait recognises its absent
branch.
