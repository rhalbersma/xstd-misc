#!/usr/bin/env bash
#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# Runs the GitHub Actions ladders' Linux legs on what setup-toolchain.sh installs, so a change is green here before it
# waits on CI's runners. Each leg does what its job does: configure, build and test the suite on one compiler, or run
# clang-tidy or the format gate. Each leg works under build/ci-local/<leg>, logs to build/ci-local/<leg>.log, and
# the run ends in one line per leg.
#
#   tools/ci-local.sh                     every leg, one after another, each on every core
#   tools/ci-local.sh gcc-17 tidy-24      the legs named
#   tools/ci-local.sh --list              the legs
#
# CI_LOCAL_BUILD_TYPE picks the suite legs' build type, Debug by default; CI runs both Debug and Release.
#
# Not here, having no Linux equivalent: MSVC, clang-cl, MinGW, Apple Clang and Emscripten. Nor the libc++ legs of the
# two newer rungs, whose libc++ cannot be installed beside libc++ 22, nor the sanitizer, CodeQL, consumption and
# actionlint jobs, whose own commands CONTRIBUTING.md gives.

set -uo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
readonly root
readonly out="${root}/build/ci-local"
readonly build_type="${CI_LOCAL_BUILD_TYPE:-Debug}"
readonly jobs="$(nproc)"
cmake="${HOME:-/root}/.local/bin/cmake"
[ -x "${cmake}" ] || cmake="cmake"
readonly cmake

readonly all_legs=(gcc-15 gcc-16 gcc-17 clang-22 clang-23 clang-24 libcxx-22 i686 coverage tidy-22 tidy-23 tidy-24 format)

# Each clang uses the libstdc++ of its own rung, as on the runners, rather than the newest GCC installed.
gcc_for_clang() {
        case "$1" in
                22) echo "--gcc-install-dir=/usr/lib/gcc/x86_64-linux-gnu/15" ;;
                23) echo "--gcc-install-dir=/usr/lib/gcc/x86_64-linux-gnu/16" ;;
                24) echo "--gcc-toolchain=/opt/gcc-latest" ;;
        esac
}

# Configure, build and test the suite, as the ladders' jobs do: <leg> <compiler> <flags> <build type> [CMake arguments...]
suite() {
        local leg="$1" cxx="$2" flags="$3" type="$4"
        shift 4
        local dir="${out}/${leg}"
        "${cmake}" -S "${root}" -B "${dir}" -G Ninja -DCMAKE_BUILD_TYPE="${type}" -DCMAKE_CXX_COMPILER="${cxx}" \
                -DCMAKE_CXX_FLAGS="${flags}" -DCMAKE_CXX_SCAN_FOR_MODULES=OFF -DCMAKE_CXX_FLAGS_DEBUG=-g1 "$@" \
                && "${cmake}" --build "${dir}" --parallel "${jobs}" \
                && ctest --test-dir "${dir}" --output-on-failure --parallel "${jobs}"
}

# A suite leg on another standard library, a trunk one or another target links a Boost.Test built for it by the same
# compiler and flags, and fails if that build does rather than fall back to the installed one: <as suite>
suite_with_boost_test() {
        local leg="$1" cxx="$2" flags="$3" type="$4" boost_test
        shift 4
        # shellcheck disable=SC2086 # the flags are a word list
        boost_test="$("${root}/tools/boost-test.sh" "${out}/boost-test-${leg}" "${cxx}" ${flags})" || return 1
        suite "${leg}" "${cxx}" "${flags}" "${type}" "$@" "${boost_test}"
}

# The clang-tidy job's passes over the compile database, and its check of each public header on its own. Every rung
# compiles against the stable rung's libstdc++, the job's gcc_tier, and the job's adapted_dirs are vcpkg's include
# directories, whose local equivalent is the installed Boost on the default search path.
tidy() {
        local version="$1" dir="${out}/tidy-$1" flags std status=0
        flags="$(gcc_for_clang 22)"
        "${cmake}" -S "${root}" -B "${dir}" -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER="clang++-${version}" \
                -DCMAKE_CXX_FLAGS="${flags}" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_CXX_SCAN_FOR_MODULES=OFF || return 1
        std="$(jq -r '.[0].command // (.[0].arguments | join(" "))' "${dir}/compile_commands.json" | grep -o -- '-std=[^ ]*' | head -1)"
        "run-clang-tidy-${version}" -quiet -p "${dir}" "${dir}/test/header_self_sufficiency/.*" || status=1
        while IFS= read -r -d '' header; do
                "clang-tidy-${version}" --quiet "${header}" -- -x c++-header "${std}" -I"${root}/include" ${flags} || status=1
        done < <(find "${root}/include" -type f -name '*.hpp' -print0)
        "run-clang-tidy-${version}" -quiet -p "${dir}" "${root}/test/src/.*" || status=1
        return "${status}"
}

# The coverage job: the suite on GCC 15 at -O0 with --coverage, then gcovr's gate at the bars coverage.yml sets.
coverage() {
        local dir="${out}/coverage"
        [ -d "${dir}" ] && find "${dir}" -name '*.gcda' -delete
        suite coverage g++-15 "--coverage -O0" Debug -DCMAKE_EXE_LINKER_FLAGS=--coverage || return 1
        (cd "${root}" && "${HOME:-/root}/.local/bin/gcovr" --root . --gcov-executable gcov-15 --merge-lines \
                --exclude 'test/.*' --exclude 'build/.*' \
                --exclude-lines-by-pattern '^\s*(assert|[A-Z][A-Z0-9_]*_ASSERT)\(' \
                --exclude-lines-by-pattern '=\s*default;' \
                --exclude-lines-by-pattern '\s*\}\s*$' \
                --exclude-branches-by-pattern '^\s*(assert|[A-Z][A-Z0-9_]*_ASSERT)\(' \
                --exclude-branches-by-pattern '^\s*.*=\s*default;' \
                --exclude-throw-branches --exclude-unreachable-branches \
                --print-summary --fail-under-line 100 --fail-under-branch 100 "${dir}")
}

# The format gate: the stable rung's clang-format over include/ and test/, reporting rather than rewriting.
format() {
        find "${root}/include" "${root}/test" -regex '.*\.\(hpp\|cpp\)' -print0 | xargs -0 clang-format-22 --dry-run --Werror
}

run_leg() {
        case "$1" in
                gcc-1[56]) suite "$1" "g++-${1#gcc-}" "" "${build_type}" ;;
                clang-2[23]) suite "$1" "clang++-${1#clang-}" "$(gcc_for_clang "${1#clang-}")" "${build_type}" ;;
                # The trunk libstdc++ is newer than the GCC 15 that built the installed Boost.Test.
                gcc-17) suite_with_boost_test "$1" g++-17 "" "${build_type}" ;;
                clang-24) suite_with_boost_test "$1" clang++-24 "$(gcc_for_clang 24)" "${build_type}" ;;
                libcxx-22) suite_with_boost_test "$1" clang++-22 "-stdlib=libc++" "${build_type}" -DCMAKE_EXE_LINKER_FLAGS=-stdlib=libc++ ;;
                # One rung in Debug, as the i686 job runs it.
                i686) suite_with_boost_test "$1" g++-15 "-m32" Debug ;;
                coverage) coverage ;;
                tidy-2[234]) tidy "${1#tidy-}" ;;
                format) format ;;
                *) echo "no leg named $1; tools/ci-local.sh --list names them" >&2; return 2 ;;
        esac
}

if [ "${1:-}" = "--list" ]; then
        printf '%s\n' "${all_legs[@]}"
        exit 0
fi
legs=("$@")
[ "${#legs[@]}" -gt 0 ] || legs=("${all_legs[@]}")

mkdir -p "${out}"
results=()
failed=0
for leg in "${legs[@]}"; do
        start="$(date +%s)"
        echo "== ${leg}"
        if run_leg "${leg}" > "${out}/${leg}.log" 2>&1; then
                verdict="pass"
        else
                verdict="FAIL"
                failed=1
        fi
        results+=("$(printf '%-4s %-11s %5ss  %s' "${verdict}" "${leg}" "$(( $(date +%s) - start ))" "${out}/${leg}.log")")
done
printf '%s\n' "${results[@]}"
exit "${failed}"
