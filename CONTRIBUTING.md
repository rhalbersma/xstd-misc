# Contributing to xstd-misc

## Workflow

When adding or changing a public utility:

1. Add or update the relevant header under `include/xstd/`.
2. Add or update matching tests under `test/src/`; CMake creates one test executable per `.cpp` file in that directory.
3. Build and test locally (see below).
4. Update the feature table and examples in [README.md](README.md) when the public API changes. Each ```` ```cpp ```` block
   there is compiled as a test, so an example includes what it uses and declares what it names.
5. Give a new header's reference page an `### Example` section before *See also*: a short program of typical use, at
   `examples/<group>/<header>.cpp`, and what it prints, at `<header>.out` beside it, which is written by running the
   example, never by hand. The page pulls both in with a `--8<--` line, so no code is copied into it. An entity with a
   counterpart on cppreference.com ports that page's example instead; see [License](#license).

## What a PR must satisfy before it can merge

This repository enforces its quality bar through CI rather than through review discretion. A PR is mergeable once every required check below is green; none of these are aspirational:

- **Every compiler/platform leg passes.** See the table in [README.md](README.md) for the current matrix (GCC, Clang, Clang-CL, MSVC, MinGW, Apple Clang). Every leg is required, including every `Development` leg (`17-SVN`, `24-SVN`, `2026-Preview`) and the `libc++` leg: xstd tracks the development channel deliberately and none of these are advisory. On a pull request each ladder builds only its floor and ceiling rungs, in Debug, while the single-purpose legs beside it run as they are; the nightly canary builds every rung and build type on `main`.
- **`clang-tidy` is clean.** The [Clang-Tidy workflow](.github/workflows/clang-tidy.yml) runs the checks in [`.clang-tidy`](.clang-tidy) with `WarningsAsErrors: '*'` over the public headers, so any finding fails the job outright - there is no "advisory, fix later" mode.
- **Line and branch coverage stay at 100%, project-wide and for the PR's own diff.** [`codecov.yml`](.github/codecov.yml) sets both the `project` and `patch` Codecov status checks to a 100% target with zero tolerance, backed by the [Coverage workflow](.github/workflows/coverage.yml)'s own `gcovr --fail-under-line 100 --fail-under-branch 100` gate. New code needs a test that exercises every line and branch it adds; existing coverage may not regress. Excluded from this bar: `assert(...)` contract checks (their failure path is undefined behavior by design, not something a correct test can hit), compiler-synthesized `= default;` special members (gcov cannot attribute a hit counter to them regardless of how often they run), and the exception-unwinding branch gcc/gcov attaches to any call that could throw (`--exclude-throw-branches`/`--exclude-unreachable-branches`) - not a code path a test can meaningfully hit either.
- **No new sanitizer failures.** The [sanitizers workflow](.github/workflows/sanitizers.yml) must stay green.
- **The public headers stay self-sufficient.** Each header is compiled as its own translation unit (see `test/CMakeLists.txt`); don't rely on include order from another header.
- **The README's examples compile.** `test/CMakeLists.txt` writes each ```` ```cpp ```` block of [README.md](README.md) to a translation unit of its own, `readme.example<n>`, built with the tests' warnings on every leg.
- **The reference's examples print what their pages show.** [`examples/CMakeLists.txt`](examples/CMakeLists.txt) builds each example written here, and [`examples/cppreference/CMakeLists.txt`](examples/cppreference/CMakeLists.txt) each one ported from cppreference.com, with the tests' warnings on every leg. CTest runs each as `example.<group>.<header>` or `example.cppreference.<page>`, and [`examples/check_output.cmake`](examples/check_output.cmake) compares what it prints with the `.out` beside it, line endings aside. The site build fails on a header page with no Example section and on an example no page shows.
- **The documentation site builds.** The [Pages workflow](.github/workflows/pages.yml) runs `mkdocs build --strict` over the README and `doc/`, which fails on a link to a page or an anchor that does not exist, and deploys the site from `main`.
- **The library stays a leaf.** `test/CMakeLists.txt` fails the configure step if any public header includes `<xstd/ints/...>` or `<xstd/bits/...>`. A facility that needs one of them belongs in that library, not here.
- **`clang-format` is clean.** The [Clang-Format workflow](.github/workflows/clang-format.yml) runs `clang-format --dry-run --Werror` over every header, test source and example against [`.clang-format`](.clang-format), so any diff fails the job. Run `clang-format -i` on changed files before pushing.
- **Workflow files pass `actionlint`.** The [Actionlint workflow](.github/workflows/actionlint.yml) validates GitHub Actions syntax and expressions.
- **The documented consumption methods work.** The [Consumption workflow](.github/workflows/consumption.yml) builds a consumer using `find_package`, `add_subdirectory`, and `FetchContent`.
- **CodeQL analysis is clean.** The [CodeQL workflow](.github/workflows/codeql.yml) runs the C/C++ `security-extended` query suite.

Match the surrounding code's style by eye where `.clang-format` doesn't have an opinion, including the Boost Software License header comment at the top of every source and workflow file.

## The CI matrix

The matrix follows the channel model used by
[apt.llvm.org](https://apt.llvm.org/): **stable** is the established release,
**qualification** the newest release still being qualified, and **development**
the current development branch. These are tested across the three main compiler
families, the three main standard libraries and the three main desktop platforms,
with MinGW, Apple Clang and Clang-CL filling out the remaining combinations.
Development jobs are required where a usable development compiler exists; the
[README](README.md#continuous-integration) records the versions currently assigned to each channel.

The `Web` row runs one Emscripten release on the stable rung, in Debug: wasm32 under node,
where `size_t` is 32 bits but 64-bit integers keep their 8-byte alignment. The GCC workflow
adds a second 32-bit leg, i686 with GCC `-m32`, where `size_t` is 32 bits and there is no
`__int128`. The MSVC workflow adds a third, x86 with VS 2026 in Debug, where `size_t` is 32
bits and 64-bit members keep their 8-byte alignment under MSVC's own ABI.

The `s390x` row is the one big-endian leg: GCC 15's s390x cross compiler, in Debug, with the
tests run under QEMU's user-mode emulator. Emulation is several times slower than native, so
the nightly canary dispatches it, and no pull request runs it.

The GCC and Clang with libc++ workflows each add a `std_test_suites` leg, on the stable rung in Debug, that runs the
standard libraries' own tests of the entities with a counterpart: see
[Running the standard libraries' test suites](#running-the-standard-libraries-test-suites).

## Test suite requirements

Keep a focused test source for each public function, concept, and trait, mirroring its header path under `test/src/`. A source matching an umbrella header (for example, `concepts.cpp`, `type_traits.cpp`, or `utility.cpp`) tests only identities that span several focused facilities.

The library itself needs one dependency, the header-only [Boost.Hash2](https://www.boost.org/doc/libs/latest/libs/hash2/) 1.89+, which [`vcpkg.json`](vcpkg.json) lists and the CMake project finds. Its test suite has a few more, and none of those are needed to *use* xstd-misc:

| Tool | Needed for | Notes |
| :--- | :--------- | :---- |
| A conforming C++20 compiler | everything | Same requirement as the library; see the table in [README.md](README.md) for the versions under CI |
| [CMake](https://cmake.org/) 3.30+ | configuring and building | `cmake_minimum_required` in [`CMakeLists.txt`](CMakeLists.txt); CTest ships with it. Noble's is 3.28; `setup-toolchain.sh` installs the version the workflows pin, from [`tools/cmake-requirements.txt`](tools/cmake-requirements.txt) |
| [Boost.Test](https://www.boost.org/doc/libs/latest/libs/test/) 1.70+ | the unit tests under `test/src/` | The floor the CMake project asks for, shared with the other xstd repositories; declared in the checked-in [`vcpkg.json`](vcpkg.json) manifest, the `*-vcpkg` presets pick it up from a `VCPKG_ROOT`-configured vcpkg, or install it with your system package manager |
| [Boost.Unordered](https://www.boost.org/doc/libs/latest/libs/unordered/) | the tests running `xstd::hasher` in an open-addressing container | Found quietly: without it those tests compile to a placeholder, so CI installs it through the [`vcpkg.json`](vcpkg.json) `test` feature |
| [gcovr](https://gcovr.com/) | reproducing the coverage gate | `setup-toolchain.sh` installs the coverage job's version, hash-pinned in [`tools/gcovr-requirements.txt`](tools/gcovr-requirements.txt) |
| `clang-tidy` and `run-clang-tidy` | reproducing the clang-tidy gate | Only for the workflow below |
| `clang-format` | the formatting gate | Run `clang-format -i` on changed files before pushing |
| [MkDocs Material](https://squidfunk.github.io/mkdocs-material/) | building the documentation site | Hash-pinned in [`tools/mkdocs-requirements.txt`](tools/mkdocs-requirements.txt); `ci-local.sh`'s site leg installs it into a virtual environment of its own |

## Getting a toolchain

Ubuntu 24.04 ships GCC 13, clang 18, CMake 3.28 and Boost 1.83, and none of them is what CI builds with: GCC 13
rejects `-std=c++2c`, `CMakeLists.txt` asks for CMake 3.30, clang-format before 22 reads `{ a * b }` in a
requires-expression as a pointer declaration, so it calls files dirty that are clean against
[`.clang-format`](.clang-format), and Boost 1.83 has no Boost.Hash2, which the library itself needs.
[`tools/setup-toolchain.sh`](tools/setup-toolchain.sh) brings a container as close to the GitHub Actions ladders as
an Ubuntu box gets, every rung of [README.md](README.md)'s matrix and the Boost CI resolves:

- GCC 15 and 16 from the Ubuntu toolchain PPA, and the GCC trunk snapshot the development rung uses, as `g++-17`;
- clang 22, 23 and trunk, as `clang++-24`, each with its clang-tidy, and clang-format 22 and libc++ 22, from apt.llvm.org;
- GCC 15's 32-bit multilib, for the i686 leg;
- the same clang-format 22 as an unversioned `clang-format`, CMake and the coverage job's gcovr, from PyPI, pinned by
  hash;
- Boost 1.92's Hash2, Test and Unordered, the libraries [`vcpkg.json`](vcpkg.json) asks for, built by GCC 15 from
  Boost's CMake release tarball, checked against the SHA-256 Boost publishes, into `/usr/local`.

```sh
tools/setup-toolchain.sh
```

apt's libc++ packages exclude one another, so libc++ stays at 22; the script names the command that swaps in another
for a libc++ leg of a newer rung. The script is idempotent, so re-running it on a warm container is safe.

In a [Claude Code cloud](https://code.claude.com/docs/en/claude-code-on-the-web) session, the repository's
SessionStart hook, [`.claude/hooks/session-start.sh`](.claude/hooks/session-start.sh), runs it before the session
starts, so every session starts with the rung in place and nothing has to be configured. Its output goes to
`/tmp/xstd-setup-toolchain.log`. An environment's setup script can run it too, once per container before any session,
and the hook then finds the rung installed. That field holds a script rather than a command, and it runs from outside
the checkout, so it needs a shebang and an absolute path:

```sh
#!/usr/bin/env bash
set -euo pipefail
for dir in /home/user/*/; do
        script="${dir}tools/setup-toolchain.sh"
        if [ -x "$script" ]; then
                exec "$script"
        fi
done
echo "setup-toolchain.sh not found in any checkout under /home/user" >&2
exit 1
```

The field is configured on the environment, not in this repository, and one environment serves every repository it
opens — so each of them needs this script at this path.

## Building and testing locally

The full suite needs Boost.Test, from the table above, and a compiler from the ladders. Name it: on Ubuntu 24.04 the
default `c++` is GCC 13, which rejects `-std=c++2c`. With what `setup-toolchain.sh` installs, no vcpkg is needed:

```sh
cmake -S . -B build -DCMAKE_CXX_COMPILER=g++-15
cmake --build build --parallel
ctest --test-dir build --output-on-failure --parallel
```

The repository also provides CMake presets for common local configurations. They name no compiler, so `CXX` picks it
on the first configure:

```sh
CXX=g++-15 cmake --preset dev
cmake --build --preset dev
ctest --preset dev
```

The `*-vcpkg` presets additionally resolve the Boost dependency through vcpkg, using the toolchain at `VCPKG_ROOT`:

```sh
CXX=g++-15 cmake --preset dev-vcpkg
cmake --build --preset dev-vcpkg
ctest --preset dev-vcpkg
```

The Boost.Test that `setup-toolchain.sh` builds is GCC 15's, and serves the released rungs, GCC 15 and 16 and clang 22
and 23. A compiler on another standard library, on the trunk snapshot's libstdc++ or on a 32-bit target needs a
Boost.Test built for it: linked with libc++ or with GCC 17's libstdc++, GCC 15's fails before the first test, on a
corrupted parameter. CI gets one from vcpkg, which builds Boost with the compiler under test. Locally,
[`tools/boost-test.sh`](tools/boost-test.sh) builds the framework from the installed headers with the compiler and
flags given, in about a minute, and prints the `-D` that makes the suite use it:

```sh
cmake -S . -B build-libcxx -DCMAKE_CXX_COMPILER=clang++-22 -DCMAKE_CXX_FLAGS=-stdlib=libc++ -DCMAKE_EXE_LINKER_FLAGS=-stdlib=libc++ \
      "$(tools/boost-test.sh build-boost-test-libcxx clang++-22 -stdlib=libc++)"
```

Before a push, [`tools/ci-local.sh`](tools/ci-local.sh) runs the GitHub Actions ladders' Linux legs on what
`setup-toolchain.sh` installs, so a change is green here before it waits on CI's runners:

- the suite on GCC 15, 16 and 17-SVN, and on clang 22, 23 and 24-SVN, each clang on its own rung's libstdc++;
- the libc++ 22 leg, and the i686 leg, at `-m32` in Debug;
- the coverage gate, the suite on GCC 15 with `--coverage`, held to 100% of lines and branches by the coverage job's
  `gcovr` command;
- clang-tidy 22, 23 and 24-SVN, each on GCC 15's libstdc++ as the clang-tidy job compiles, with that job's passes:
  the generated per-header units, each public header on its own, and the test sources;
- the format gate;
- the draft check, which no workflow runs: it fetches the working draft the headers cite from
  [cplusplus/draft](https://github.com/cplusplus/draft), at that draft's tag, and `tools/check-draft-wording.py` checks
  that each definition copied from it still reads as the cited paragraph does, token for token;
- the site, built as the Pages workflow builds it.

It takes a list of legs, or runs them all, one after another, with every core each; `--list` names them. Each leg
logs to `build/ci-local/<leg>.log`, and the run ends in one line per leg. The suite legs build in Debug, or in
`CI_LOCAL_BUILD_TYPE`; CI builds both.

```sh
tools/ci-local.sh                  # every leg
tools/ci-local.sh gcc-17 tidy-24   # the ones a change is likeliest to break
```

MSVC, clang-cl, MinGW, Apple Clang and Emscripten have no Linux equivalent, and the libc++ 23 and 24-SVN legs need a
libc++ that cannot be installed beside 22. The sanitizer, CodeQL, consumption and actionlint jobs are not in it
either; the coverage and clang-tidy commands are also given on their own in the sections below.

### Deleting clauses

Full coverage says a line ran, not that a test would fail if it were wrong, and a concept or a requires-clause
emits no code for a mutation tester such as [Mull](https://github.com/mull-project/mull) to change. [`tools/delete-clauses.py`](tools/delete-clauses.py)
makes those mutants itself:

- for every concept and requires-clause in a public header, it deletes, one at a time, each operand of an `and`
  or an `or`, each `not`, and each requirement of a requires-expression;
- it then compiles every test source, without running it, on GCC 15 and on clang 22 with libc++.

A mutant that some test no longer compiles under is killed. One the header itself rejects is reported as invalid.
One that every compiler accepts survives, and is answered by a test with an existing type or accepted in
[`tools/delete-clauses-survivors.toml`](tools/delete-clauses-survivors.toml), with the reason no existing type from the
standard library, Boost, Abseil or the xstd libraries fails that clause alone. The run fails on a survivor the file
does not accept, and on an accepted one that a test now kills, so the file stays the list of clauses the definition
alone states. It takes about twenty minutes on four cores. The [Mutation workflow](.github/workflows/mutation.yml)
runs it on Wednesdays and by hand, and a scheduled run that fails opens an issue titled "The weekly mutation check
failed", or comments on the open one.

```sh
tools/delete-clauses.py                                           # every header
tools/delete-clauses.py include/xstd/misc/concepts/proxy_reference.hpp
```

### Running the standard libraries' test suites

libc++, libstdc++ and the MSVC STL each test their `to_underlying` and their string hashes against the same
paragraphs this library's tests are written to. With `-DXSTD_MISC_STD_TEST_SUITES=ON`,
[`test/std-test-suite/`](test/std-test-suite/) fetches those tests at the stable rung's tags, through cpp-ci's
`std_test_suite` module at the commit the workflows pin, rewrites them onto `xstd::to_underlying` and `xstd::hash`, and
registers each as a CTest test labelled `std-test-suite`. [`mapping.toml`](test/std-test-suite/mapping.toml) holds
what is taken, how it is rewritten, and each departure with the reason for it. Its one departure is libc++'s
assertion that the string hash's call is `noexcept`, which `xstd::hash`'s is not, since a key's own `hash_append` may
throw.

| Suite | Tests | Under GCC 15 | Under Clang 22 with libc++ |
| :---- | :---- | :----------- | :------------------------- |
| libc++ | `to_underlying`, the string hash | run | run |
| libstdc++ | `to_underlying` (compile only), the string, `string_view`, PMR and debug-mode hashes | run | skipped: its support headers need libstdc++ |
| MSVC STL | `to_underlying` | run | run |

A test of diagnostics, and one that no rewrite reaches, is skipped with that reason in the build tree's
`manifest.json`. A mapping entry whose file has changed upstream fails configuration, so a moved pin is reviewed.

```sh
cmake -S . -B build/suites -G Ninja -DCMAKE_CXX_COMPILER=g++-15 -DXSTD_MISC_STD_TEST_SUITES=ON
cmake --build build/suites --target std-test-suite-libcxx std-test-suite-libstdcxx std-test-suite-msvc-stl
ctest --test-dir build/suites -L std-test-suite
```

### Reproducing the coverage gate

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS="--coverage -O0 -g" -DCMAKE_EXE_LINKER_FLAGS="--coverage"
cmake --build build
ctest --test-dir build --output-on-failure
gcovr --root . --exclude 'test/.*' --exclude 'build/.*' --exclude 'examples/.*' \
  --exclude-lines-by-pattern '^\s*assert\(' \
  --exclude-lines-by-pattern '=\s*default;' \
  --exclude-branches-by-pattern '^\s*assert\(' \
  --exclude-branches-by-pattern '^\s*.*=\s*default;' \
  --exclude-throw-branches --exclude-unreachable-branches \
  --print-summary --fail-under-line 100 --fail-under-branch 100
```

### Reproducing the clang-tidy gate

```sh
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
run-clang-tidy -quiet -p build "$PWD/build/test/header_self_sufficiency/.*"
```

### Building the documentation

[`mkdocs.yml`](mkdocs.yml) serves the README as the site's index and the pages under `doc/` beside it, and
[`tools/mkdocs_hooks.py`](tools/mkdocs_hooks.py) points each relative link, written for GitHub, at the page the site
serves for it, or at its file on GitHub. The hook also fails the build on a citation left unlinked in prose: a section
of the working draft, such as `[utility.underlying]/1`, links to it at [eel.is/c++draft](https://eel.is/c++draft/), a
WG21 paper to its [cplusplus/papers](https://github.com/cplusplus/papers) issue (`wg21.link/P1682/github` finds it),
and a paper without one, or an N-numbered document, to `wg21.link`. A Boost library links to its documentation under
`boost.org/doc/libs/latest/`. To preview the site as it is edited:

```sh
python3 -m venv build/mkdocs && build/mkdocs/bin/pip install --require-hashes -r tools/mkdocs-requirements.txt
build/mkdocs/bin/mkdocs serve --strict
```

## Standards-paper tracking

Implementation-relevant proposals must record the paper number, the revision-pinned wording reviewed by xstd, the corresponding live `cplusplus/papers` tracking issue, the last-reviewed date, and a short statement saying whether xstd implements, partially follows, extends, or merely references the paper. Unversioned `wg21.link/P####` links are reserved for links explicitly described as the latest revision; implementation and conformance claims use pinned `P####R#` links. Durable prose does not copy volatile WG21 status labels.

A new paper revision or a change on its tracking issue triggers a review, not an automatic code change. Update the review date only after inspecting the relevant revision and reconsidering xstd's stated relationship.

| Paper | Reviewed wording | Tracking issue | Last reviewed | Relationship |
|---|---|---|---|---|
| P1682R1 | [P1682R1](https://wg21.link/P1682R1) | [cplusplus/papers#460](https://github.com/cplusplus/papers/issues/460) | 2026-09-04 | Specifies `std::to_underlying`. The plain-enum overload reproduces the cast the paper specifies rather than calling `std::to_underlying`, which is C++23 and would raise the baseline. xstd adds an overload the paper does not cover: an enum value wrapped in `std::integral_constant`, returned as an `integral_constant` of the underlying type. [doc/ideas.md](doc/ideas.md) is the 2016 sketch behind the paper. |
| P0029R0 | [P0029R0](https://wg21.link/P0029R0) | none | 2026-10-19 | Merely referenced, as prior art for `hash`: its `std::hash`-integrated design, with a default algorithm randomized per run, is the alternative [doc/design.md](doc/design.md) weighs. xstd follows N3980 and Boost.Hash2 instead, with a user-chosen algorithm and a byte stream fixed across runs. |
| P0814R2 | [P0814R2](https://wg21.link/P0814R2) | [cplusplus/papers#253](https://github.com/cplusplus/papers/issues/253) | 2026-10-19 | Merely referenced, as prior art for `hash`: a `hash_combine` over `std::hash` values. xstd has no counterpart, since `hash_append` feeds every member to one algorithm state. |
| P2098R1 | [P2098R1](https://wg21.link/P2098R1) | [cplusplus/papers#812](https://github.com/cplusplus/papers/issues/812) | 2026-09-04 | Supplies the name and the semantics for class templates whose parameters are types. xstd implements that form and its `_v` variable template, and adds the concept spelling `specialization_of`; the standard library has no counterpart to defer to. |

## Required status checks

The names to tick under branch protection, exactly as GitHub reports them:

| Check | Covers |
| :--- | :--- |
| `actionlint / actionlint` | workflow syntax |
| `apple_clang / all` | Xcode 16.4 and 26.6, Debug and Release |
| `clang / all` | Clang 22, 23, 24-SVN with libstdc++ |
| `clang_cl / all` | clang-cl on VS 2022, 2026, 2026-Preview |
| `clang_format / clang-format` | `clang-format --dry-run --Werror` against [`.clang-format`](.clang-format) |
| `clang_libcxx / all` | Clang 22, 23, 24-SVN with libc++ |
| `clang_tidy / all` | clang-tidy on all three rungs |
| `codeql / Analyze` | `security-extended` |
| `consumption / Consume` | the three CMake consumption models |
| `coverage / gcovr` | 100% of lines and branches |
| `gcc / all` | GCC 15, 16, 17-SVN |
| `mingw / all` | MinGW 15 and 16 |
| `msvc / all` | cl on VS 2022, 2026 and 2026-Preview |
| `msvc_analyze / all` | `/analyze` on the same three rungs |
| `sanitizers / all` | all fifteen sanitizer legs |
| `site` | `mkdocs build --strict`: every page and anchor a link names exists |

Codecov posts two more, `codecov/project` and `codecov/patch`, which carry the same 100% bar for the whole
tree and for the diff.

The [Scorecard workflow](.github/workflows/scorecard.yml) cannot be required: it runs on pushes to `main` and
on a schedule, never on a pull request.

Nor can the [Claims workflow](.github/workflows/claims.yml), which runs on Tuesdays and by hand. It checks what can go
false with nothing here changing:
- **Hash2:** the suite at the stable rung, against Boost's newest release with Hash2 at the head of its `develop`
  branch.
- **The draft:** the wording the headers copy, against the newest draft and the draft repository's `main`, through
  `tools/check-draft-wording.py --any-draft`.
- **The matrix:** the README's CI matrix, against cpp-ci's toolchain table at the commit the workflows call and at
  cpp-ci's `main`, through `tools/check-readme-matrix.py`.

A scheduled run that fails opens an issue titled "The weekly claims check failed", or comments on the open one.

Each ladder ends in an `all` job that is red unless every one of its legs succeeded, and that gate is what
branch protection requires: a leg name changes whenever a rung moves, the gate name does not.

The gate is this repository's own job, carrying the required name, beside the shared ladder it waits for. On a
pull request, [`changes.yml`](.github/workflows/changes.yml) first compares the diff with the paths no ladder
builds: prose, `doc/`, `tools/`, the site's configuration, and the workflows that are not ladders. The README
counts as prose unless one of its ```` ```cpp ```` blocks, each compiled as a test, is added, removed or changed. An
example under `examples/`, or the output beside it, is code, and the reference page that shows it is prose: the page
pulls the example in, and holds no code of its own.
When every changed file is one of those, the ladder is skipped and its gate
passes; any other file, a new directory included, runs the ladder in full and the gate passes only if every leg
did. A manual run and the canary always build. The names under branch protection do not change. This table is
[xstd-bits](https://github.com/rhalbersma/xstd-bits/blob/main/CONTRIBUTING.md)' as well, less the C++26 legs
it alone carries, the three repositories holding one bar between them.

## License

By contributing, you agree that your contributions will be licensed under the [Boost Software License, Version 1.0](LICENSE_1_0.txt), the same license that covers the rest of this repository.

The examples ported from cppreference.com are the exception, and follow four rules:

- **Kept apart.** A ported example, and the output beside it, is under `examples/cppreference/`, whose
  [LICENSE](examples/cppreference/LICENSE) gives the Creative Commons Attribution-ShareAlike 3.0 Unported License and
  the GNU Free Documentation License, under which cppreference.com publishes its content.
- **Attributed.** Its header comment links the cppreference.com page and the revision it was ported from, credits
  cppreference.com contributors and says how it was adapted, and its reference page's Example section links the same.
- **Share-alike.** The `CMakeLists.txt` that builds, runs and checks the ported examples is under the same two
  licences, and nothing under `test/src/` or elsewhere in the repository derives from them.
- **Marked.** A line above the code on its reference page says the example is under those licences and that the rest
  of the page is under the Boost Software License.
