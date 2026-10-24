//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_FORMAT_DETAIL_THROW_FORMAT_ERROR_HPP
#define XSTD_MISC_FORMAT_DETAIL_THROW_FORMAT_ERROR_HPP

#if defined(__cpp_exceptions) or defined(_CPPUNWIND)
#include <format> // format_error
#else
#include <cstdio>      // fputc, stderr
#include <cstdlib>     // abort
#include <string_view> // string_view
#endif

namespace xstd::detail {

// Throws where exceptions are on; under -fno-exceptions, writes what() to stderr and aborts, as libstdc++ does.
[[noreturn]] inline auto throw_format_error(char const* what)
        -> void
{
#if defined(__cpp_exceptions) or defined(_CPPUNWIND)
        throw std::format_error(what);
#else
        // GCOVR_EXCL_START: reached only by the death test, whose abort leaves no coverage data behind.
        // One character at a time: Clang's -Wunsafe-buffer-usage turns away every libc call that takes a buffer.
        for (auto const c : std::string_view(what)) {
                static_cast<void>(std::fputc(c, stderr));
        }
        static_cast<void>(std::fputc('\n', stderr));
        std::abort();
        // GCOVR_EXCL_STOP
#endif
}

} // namespace xstd::detail

#endif // XSTD_MISC_FORMAT_DETAIL_THROW_FORMAT_ERROR_HPP
