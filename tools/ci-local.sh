#!/usr/bin/env bash
#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# Runs the GitHub Actions jobs' Linux legs on what setup-toolchain.sh installs, so a change is green here before it
# waits on CI's runners. The legs this repository's workflow stubs take from cpp-ci run through cpp-ci's
# tools/ci_local.py, at the commit the stubs pin, which reads them from the stubs themselves; this repository's own
# legs, layering, draft and site, run here beside them. The draft leg is the one no workflow runs: it fetches the
# working draft the headers cite and checks that what they copy from it still reads as it does. Legs run cheapest
# first and the run stops at the first that fails. Each leg works under build/ci-local/<leg>, logs to
# build/ci-local/<leg>.log, and the run ends in one line per leg.
#
#   tools/ci-local.sh --quick                before a commit: actionlint, format, layering, GCC 15, clang 22, clang-tidy 22
#   tools/ci-local.sh                        before a push or a pull request: every leg
#   tools/ci-local.sh gcc-17-svn clang-tidy-24-svn   the legs named, in the order given
#   tools/ci-local.sh --keep-going ...       every leg asked for, whatever fails
#   tools/ci-local.sh --list                 the legs, in the order a full run takes them
#
# CI_LOCAL_DEPS=vcpkg builds the dependencies through vcpkg, as CI does; the default, system, builds against the Boost
# setup-toolchain.sh installs, for a machine whose network refuses the source archives vcpkg downloads. CPP_CI_DIR runs
# a cpp-ci checkout of your own in place of the pinned one.
#
# Not here, having no Linux equivalent: MSVC, clang-cl, MinGW, Apple Clang, Emscripten, Android and iOS. The libc++
# and sanitizer legs of the rungs above the stable one are CI's too; --list names each with its reason.

set -uo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
readonly root
readonly out="${root}/build/ci-local"
readonly deps="${CI_LOCAL_DEPS:-system}"
cpp_ci="$("${root}/tools/cpp-ci.sh")" || exit 1
readonly cpp_ci
ci_local=(python3 -I "${cpp_ci}/tools/ci_local.py" --caller "${root}" --out "${out}" --deps "${deps}")

# This repository's own legs, the cheap checks, after format.
readonly own_checks=(layering draft site)
readonly quick_legs=(actionlint format layering gcc-15 clang-22 clang-tidy-22)

# cpp-ci's layering check, from the pinned checkout, over the folder kinds tools/layering.toml names.
layering() {
        (cd "${root}" && python3 -I "${cpp_ci}/.github/actions/layering/layering.py" tools/layering.toml)
}

# The working draft at the tag the headers cite, from the draft's own repository, then the check of their wording.
draft() {
        local dir="${out}/draft" docno
        docno="$(python3 -I "${root}/tools/check-draft-wording.py" --docno)" || return 1
        if [ ! -d "${dir}/.git" ]; then
                git init -q "${dir}" && git -C "${dir}" remote add origin https://github.com/cplusplus/draft.git || return 1
        fi
        git -C "${dir}" fetch -q --depth 1 origin tag "${docno}" && git -C "${dir}" checkout -q "${docno}" \
                && python3 -I "${root}/tools/check-draft-wording.py" "${dir}"
}

# The Pages workflow's build: MkDocs Material, pinned by hash, into a virtual environment of its own, then the site in
# --strict mode, which fails on a link to a page or an anchor that does not exist.
site() {
        local dir="${out}/site"
        if [ ! -x "${dir}/venv/bin/mkdocs" ]; then
                python3 -m venv "${dir}/venv" \
                        && "${dir}/venv/bin/pip" install --quiet --require-hashes -r "${root}/tools/mkdocs-requirements.txt" || return 1
        fi
        (cd "${root}" && PYTHONDONTWRITEBYTECODE=1 "${dir}/venv/bin/mkdocs" build --strict --site-dir "${dir}/html")
}

# cpp-ci's legs in its run order, with this repository's own checks woven in after format.
all_legs() {
        "${ci_local[@]}" --list | awk -v checks="${own_checks[*]}" '
                /^run, in order:/ { listing = 1; next }
                listing && NF == 0 { exit }
                listing { print $1; if ($1 == "format") print checks }' | tr ' ' '\n' | sed '/^$/d'
}

run_leg() {
        case "$1" in
                layering) layering > "${out}/$1.log" 2>&1 ;;
                draft) draft > "${out}/$1.log" 2>&1 ;;
                site) site > "${out}/$1.log" 2>&1 ;;
                *) "${ci_local[@]}" "$1" > "${out}/$1.run" 2>&1 ;;
        esac
}

keep_going=0
legs=()
for argument in "$@"; do
        case "${argument}" in
                --list) all_legs; exit ;;
                --quick) legs+=("${quick_legs[@]}") ;;
                -k|--keep-going) keep_going=1 ;;
                *) legs+=("${argument}") ;;
        esac
done
mapfile -t known < <(all_legs) || exit 1
[ "${#legs[@]}" -gt 0 ] || legs=("${known[@]}")
for leg in "${legs[@]}"; do
        printf '%s\n' "${known[@]}" | grep -qx -- "${leg}" \
                || { echo "no leg named ${leg}; tools/ci-local.sh --list names them" >&2; exit 2; }
done

mkdir -p "${out}"
results=()
failed=0
for leg in "${legs[@]}"; do
        if [ "${failed}" -eq 1 ] && [ "${keep_going}" -eq 0 ]; then
                results+=("$(printf '%-4s %-26s' skip "${leg}")")
                continue
        fi
        start="$(date +%s)"
        echo "== ${leg}"
        if run_leg "${leg}"; then
                verdict="pass"
        else
                verdict="FAIL"
                failed=1
        fi
        results+=("$(printf '%-4s %-26s %5ss  %s' "${verdict}" "${leg}" "$(( $(date +%s) - start ))" "${out}/${leg}.log")")
done
printf '%s\n' "${results[@]}"
exit "${failed}"
