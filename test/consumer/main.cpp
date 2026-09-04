//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/concepts/specialization_of.hpp>
#include <xstd/misc/type_traits.hpp>
#include <xstd/misc/utility.hpp>
#include <complex>
#include <type_traits>

enum class color : unsigned { red = 1 };

struct storage
{
        [[XSTD_NO_UNIQUE_ADDRESS]] xstd::conditional_data_member_t<false, int, storage> absent;
        int present;
};

int main()
{
        static_assert(xstd::specialization_of<std::complex<double>, std::complex>);
        static_assert(xstd::is_specialization_of_v<std::complex<double>, std::complex>);
        static_assert(decltype(xstd::to_underlying(std::integral_constant<color, color::red>()))::value == 1);
        static_assert(sizeof(storage) == sizeof(int));

        return xstd::to_underlying(color::red) == 1u ? 0 : 1;
}
