//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_FUNCTIONAL_DETAIL_STANDARD_KEYS_HPP
#define XSTD_MISC_FUNCTIONAL_DETAIL_STANDARD_KEYS_HPP

#include <boost/hash2/endian.hpp>      // endian
#include <boost/hash2/hash_append.hpp> // hash_append, hash_append_tag
#include <array>                       // array
#include <bit>                         // bit_cast, endian
#include <bitset>                      // bitset
#include <chrono>                      // day, duration, month, time_point, weekday, year, year_month_day, ...
#include <coroutine>                   // coroutine_handle
#include <cstddef>                     // ptrdiff_t, size_t
#include <cstdint>                     // uint16_t, uint64_t
#include <filesystem>                  // path
#include <iterator>                    // next
#include <limits>                      // numeric_limits
#include <memory>                      // shared_ptr, unique_ptr
#include <string_view>                 // string_view
#include <system_error>                // error_code, error_condition
#include <thread>                      // thread
#include <type_traits>                 // has_unique_object_representations_v, is_floating_point_v, is_same_v
#include <typeindex>                   // type_index
#include <version>                     // __cpp_lib_stacktrace, __cpp_lib_text_encoding

#ifdef __cpp_lib_stacktrace
#include <stacktrace> // IWYU pragma: keep; stacktrace_entry
#endif

#ifdef __cpp_lib_text_encoding
#include <text_encoding> // IWYU pragma: keep; text_encoding
#endif

#if (defined(__cpp_lib_chrono) and __cpp_lib_chrono >= 201907L) or (defined(_LIBCPP_HAS_EXPERIMENTAL_TZDB) and _LIBCPP_HAS_EXPERIMENTAL_TZDB and _LIBCPP_HAS_TIME_ZONE_DATABASE and _LIBCPP_HAS_FILESYSTEM and _LIBCPP_HAS_LOCALIZATION)
#define XSTD_MISC_DETAIL_HAS_TIME_ZONES
#endif

namespace xstd::detail {

#ifdef __SIZEOF_INT128__
__extension__ using int128  = __int128;
__extension__ using uint128 = unsigned __int128;

template<class T>
inline constexpr auto is_int128_v = std::is_same_v<T, int128> or std::is_same_v<T, uint128>;
#endif

// Hash2 has no 16-byte write where is_integral admits a 128-bit integer, and no overload where it does not.
template<class Hash, class Flavor, class T>
constexpr auto append_key(Hash& h, Flavor const& f, T const& v)
        -> void
{
#ifdef __SIZEOF_INT128__
        if constexpr (is_int128_v<T>) {
                auto const low  = static_cast<std::uint64_t>(static_cast<uint128>(v));
                auto const high = static_cast<std::uint64_t>(static_cast<uint128>(v) >> static_cast<unsigned>(std::numeric_limits<std::uint64_t>::digits));
                if constexpr (Flavor::byte_order == boost::hash2::endian::big) {
                        boost::hash2::hash_append(h, f, high);
                        boost::hash2::hash_append(h, f, low);
                } else {
                        boost::hash2::hash_append(h, f, low);
                        boost::hash2::hash_append(h, f, high);
                }
        } else {
                boost::hash2::hash_append(h, f, v);
        }
#else
        boost::hash2::hash_append(h, f, v);
#endif
}

} // namespace xstd::detail

// Each key the standard specializes std::hash for and Boost.Hash2 does not hash appends the state its operator== reads.
namespace boost::hash2 {

// The floating-point types Hash2 does not hash, of neither 4 nor 8 bytes, such as x87's long double and float16_t.
template<class Provider, class Hash, class Flavor, class T>
        requires std::is_floating_point_v<T> and (sizeof(T) != sizeof(std::uint32_t)) and (sizeof(T) != sizeof(std::uint64_t))
constexpr auto tag_invoke(hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, T const* v)
        -> void
{
        constexpr auto binary128_digits = 113;
        auto const bytes                = std::bit_cast<std::array<unsigned char, sizeof(T)>>(*v + T(0));
        auto const word                 = [&](std::size_t first, std::size_t count) -> std::uint64_t {
                auto w = std::uint64_t{0};
                for (auto i = count; i > 0; --i) {
                        w = (w << static_cast<unsigned>(std::numeric_limits<unsigned char>::digits)) | bytes[first + i - 1];
                }
                return w;
        };
        if constexpr (sizeof(T) == sizeof(std::uint16_t)) {
                Provider::hash_append(h, f, static_cast<std::uint16_t>(word(0, sizeof(std::uint16_t))));
        } else if constexpr (std::numeric_limits<T>::digits == std::numeric_limits<std::uint64_t>::digits) {
                // x87's 80 bits: a 64-bit significand, then a 16-bit sign and exponent, then padding to 12 or 16 bytes.
                Provider::hash_append(h, f, word(0, sizeof(std::uint64_t)));
                Provider::hash_append(h, f, static_cast<std::uint16_t>(word(sizeof(std::uint64_t), sizeof(std::uint16_t))));
        } else if constexpr (sizeof(T) == 2 * sizeof(std::uint64_t) and std::numeric_limits<T>::digits == binary128_digits and std::endian::native == std::endian::little) {
                Provider::hash_append(h, f, word(0, sizeof(std::uint64_t)));
                Provider::hash_append(h, f, word(sizeof(std::uint64_t), sizeof(std::uint64_t)));
        } else {
                static_assert(sizeof(T) == 0, "a floating-point format of a width Boost.Hash2 does not hash, and this header does not know");
        }
}

template<class Provider, class Hash, class Flavor, std::size_t N>
constexpr auto tag_invoke(hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, std::bitset<N> const* v)
        -> void
{
        constexpr auto word_bits = static_cast<std::size_t>(std::numeric_limits<std::uint64_t>::digits);
        if constexpr (N == 0) {
                Provider::hash_append(h, f, '\x00');
        }
        for (auto first = std::size_t{0}; first < N; first += word_bits) {
                auto word = std::uint64_t{0};
                for (auto bit = std::size_t{0}; bit < word_bits and first + bit < N; ++bit) {
                        word |= std::uint64_t{(*v)[first + bit]} << bit;
                }
                Provider::hash_append(h, f, word);
        }
}

template<class Provider, class Hash, class Flavor>
auto tag_invoke(hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, std::error_code const* v)
        -> void
{
        Provider::hash_append(h, f, &v->category());
        Provider::hash_append(h, f, v->value());
}

template<class Provider, class Hash, class Flavor>
auto tag_invoke(hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, std::error_condition const* v)
        -> void
{
        Provider::hash_append(h, f, &v->category());
        Provider::hash_append(h, f, v->value());
}

template<class Provider, class Hash, class Flavor, class T, class D>
constexpr auto tag_invoke(hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, std::unique_ptr<T, D> const* v)
        -> void
{
        Provider::hash_append(h, f, v->get());
}

template<class Provider, class Hash, class Flavor, class T>
auto tag_invoke(hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, std::shared_ptr<T> const* v)
        -> void
{
        Provider::hash_append(h, f, v->get());
}

// The name rather than hash_code(), which distinct types may share.
template<class Provider, class Hash, class Flavor>
auto tag_invoke(hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, std::type_index const* v)
        -> void
{
        Provider::hash_append(h, f, std::string_view(v->name()));
}

template<class Provider, class Hash, class Flavor>
auto tag_invoke(hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, std::thread::id const* v)
        -> void
{
        static_assert(std::has_unique_object_representations_v<std::thread::id>, "a thread::id whose bytes do not determine its value");
        Provider::hash_append(h, f, std::bit_cast<std::array<unsigned char, sizeof(std::thread::id)>>(*v));
}

template<class Provider, class Hash, class Flavor, class Promise>
auto tag_invoke(hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, std::coroutine_handle<Promise> const* v)
        -> void
{
        Provider::hash_append(h, f, v->address());
}

// The parts compare() reads, in its order: the root name, whether there is a root directory, then each element after.
template<class Provider, class Hash, class Flavor>
auto tag_invoke(hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, std::filesystem::path const* v)
        -> void
{
        Provider::hash_append(h, f, v->root_name().native());
        Provider::hash_append(h, f, v->has_root_directory());
        auto element  = std::next(v->begin(), static_cast<std::ptrdiff_t>(v->has_root_name()) + static_cast<std::ptrdiff_t>(v->has_root_directory()));
        auto elements = std::size_t{0};
        for (; element != v->end(); ++element) {
                Provider::hash_append(h, f, element->native());
                ++elements;
        }
        Provider::hash_append_size(h, f, elements);
}

#ifdef __cpp_lib_stacktrace
template<class Provider, class Hash, class Flavor>
auto tag_invoke(hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, std::stacktrace_entry const* v)
        -> void
{
        Provider::hash_append(h, f, v->native_handle());
}
#endif

#ifdef __cpp_lib_text_encoding
// An unregistered name drops every 0, as libstdc++ drops one after a digit and punctuation that comp-name keeps.
template<class Provider, class Hash, class Flavor>
constexpr auto tag_invoke(hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, std::text_encoding const* v)
        -> void
{
        Provider::hash_append(h, f, v->mib());
        if (v->mib() == std::text_encoding::id::other) {
                auto name   = std::array<char, std::text_encoding::max_name_length>();
                auto length = std::size_t{0};
                for (auto const c : std::string_view(v->name())) {
                        if (('a' <= c and c <= 'z') or ('1' <= c and c <= '9')) {
                                name[length++] = c;
                        } else if ('A' <= c and c <= 'Z') {
                                name[length++] = static_cast<char>(c - 'A' + 'a');
                        }
                }
                Provider::hash_append(h, f, std::string_view(name.data(), length));
        }
}
#endif

#ifdef __cpp_lib_indirect
template<class Provider, class Hash, class Flavor, class T, class Allocator>
constexpr auto tag_invoke(hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, std::indirect<T, Allocator> const* v)
        -> void
{
        Provider::hash_append(h, f, v->valueless_after_move());
        if (!v->valueless_after_move()) {
                Provider::hash_append(h, f, **v);
        }
}
#endif

template<class Provider, class Hash, class Flavor, class Rep, class Period>
constexpr auto tag_invoke(hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, std::chrono::duration<Rep, Period> const* v)
        -> void
{
        Provider::hash_append(h, f, v->count());
}

template<class Provider, class Hash, class Flavor, class Clock, class Duration>
constexpr auto tag_invoke(hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, std::chrono::time_point<Clock, Duration> const* v)
        -> void
{
        Provider::hash_append(h, f, v->time_since_epoch());
}

template<class Provider, class Hash, class Flavor>
constexpr auto tag_invoke(hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, std::chrono::day const* v)
        -> void
{
        Provider::hash_append(h, f, static_cast<unsigned>(*v));
}

template<class Provider, class Hash, class Flavor>
constexpr auto tag_invoke(hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, std::chrono::month const* v)
        -> void
{
        Provider::hash_append(h, f, static_cast<unsigned>(*v));
}

template<class Provider, class Hash, class Flavor>
constexpr auto tag_invoke(hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, std::chrono::year const* v)
        -> void
{
        Provider::hash_append(h, f, static_cast<int>(*v));
}

template<class Provider, class Hash, class Flavor>
constexpr auto tag_invoke(hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, std::chrono::weekday const* v)
        -> void
{
        Provider::hash_append(h, f, v->c_encoding());
}

template<class Provider, class Hash, class Flavor>
constexpr auto tag_invoke(hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, std::chrono::weekday_indexed const* v)
        -> void
{
        Provider::hash_append(h, f, v->weekday());
        Provider::hash_append(h, f, v->index());
}

template<class Provider, class Hash, class Flavor>
constexpr auto tag_invoke(hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, std::chrono::weekday_last const* v)
        -> void
{
        Provider::hash_append(h, f, v->weekday());
}

template<class Provider, class Hash, class Flavor>
constexpr auto tag_invoke(hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, std::chrono::month_day const* v)
        -> void
{
        Provider::hash_append(h, f, v->month());
        Provider::hash_append(h, f, v->day());
}

template<class Provider, class Hash, class Flavor>
constexpr auto tag_invoke(hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, std::chrono::month_day_last const* v)
        -> void
{
        Provider::hash_append(h, f, v->month());
}

template<class Provider, class Hash, class Flavor>
constexpr auto tag_invoke(hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, std::chrono::month_weekday const* v)
        -> void
{
        Provider::hash_append(h, f, v->month());
        Provider::hash_append(h, f, v->weekday_indexed());
}

template<class Provider, class Hash, class Flavor>
constexpr auto tag_invoke(hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, std::chrono::month_weekday_last const* v)
        -> void
{
        Provider::hash_append(h, f, v->month());
        Provider::hash_append(h, f, v->weekday_last());
}

template<class Provider, class Hash, class Flavor>
constexpr auto tag_invoke(hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, std::chrono::year_month const* v)
        -> void
{
        Provider::hash_append(h, f, v->year());
        Provider::hash_append(h, f, v->month());
}

template<class Provider, class Hash, class Flavor>
constexpr auto tag_invoke(hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, std::chrono::year_month_day const* v)
        -> void
{
        Provider::hash_append(h, f, v->year());
        Provider::hash_append(h, f, v->month());
        Provider::hash_append(h, f, v->day());
}

template<class Provider, class Hash, class Flavor>
constexpr auto tag_invoke(hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, std::chrono::year_month_day_last const* v)
        -> void
{
        Provider::hash_append(h, f, v->year());
        Provider::hash_append(h, f, v->month_day_last());
}

template<class Provider, class Hash, class Flavor>
constexpr auto tag_invoke(hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, std::chrono::year_month_weekday const* v)
        -> void
{
        Provider::hash_append(h, f, v->year());
        Provider::hash_append(h, f, v->month());
        Provider::hash_append(h, f, v->weekday_indexed());
}

template<class Provider, class Hash, class Flavor>
constexpr auto tag_invoke(hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, std::chrono::year_month_weekday_last const* v)
        -> void
{
        Provider::hash_append(h, f, v->year());
        Provider::hash_append(h, f, v->month());
        Provider::hash_append(h, f, v->weekday_last());
}

#ifdef XSTD_MISC_DETAIL_HAS_TIME_ZONES
template<class Provider, class Hash, class Flavor, class Duration, class TimeZonePtr>
auto tag_invoke(hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, std::chrono::zoned_time<Duration, TimeZonePtr> const* v)
        -> void
{
        Provider::hash_append(h, f, v->get_time_zone());
        Provider::hash_append(h, f, v->get_sys_time());
}

// Two leap seconds compare by their date alone.
template<class Provider, class Hash, class Flavor>
auto tag_invoke(hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, std::chrono::leap_second const* v)
        -> void
{
        Provider::hash_append(h, f, v->date());
}
#endif

} // namespace boost::hash2

#endif // XSTD_MISC_FUNCTIONAL_DETAIL_STANDARD_KEYS_HPP
