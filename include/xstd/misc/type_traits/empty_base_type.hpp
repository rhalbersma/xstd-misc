//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_TYPE_TRAITS_EMPTY_BASE_TYPE_HPP
#define XSTD_MISC_TYPE_TRAITS_EMPTY_BASE_TYPE_HPP

namespace xstd {

// A stand-in for a base class that is not there, and a BASE is what it is for. It carries nothing, and that is
// the whole difference from empty_member_type: a base's hidden friends ARE associated with the derived type,
// so a comparison here would be found by ADL for every derived object and would answer EQUAL for any two of
// them, having only the empty base to compare. A member's are not, which is why the other half can afford one.
//
// The price is exact and worth knowing before reaching for this: a derived class CANNOT default its own
// comparisons over a base that has none -- the defaulted operator is defined as deleted. So this serves the
// incomparable case, a view following std::span, which P1085 stripped of == and <=> alike. A class wanting an
// empty base AND defaulted comparisons wants a base that carries them, which is a different type and its own.
//
// The tag keeps two empty bases distinct: a class cannot derive from the same one twice.
template<class Tag = void>
struct empty_base_type {};

} // namespace xstd

#endif // XSTD_MISC_TYPE_TRAITS_EMPTY_BASE_TYPE_HPP
