# Contributing to xstd-misc

## Workflow

When adding or changing a public utility:

1. Add or update the relevant header under `include/xstd/`.
2. Add or update matching tests under `test/src/`; CMake creates one test executable per `.cpp` file in that directory.
3. Build and test locally (see below).
4. Update the feature table and examples in [README.md](README.md) when the public API changes.

## What a PR must satisfy before it can merge

This repository enforces its quality bar through CI rather than through review discretion. A PR is mergeable once every required check below is green; none of these are aspirational:

- **Every compiler/platform leg passes.** See the table in [README.md](README.md) for the current matrix (GCC, Clang, Clang-CL, MSVC, MinGW, Apple Clang). Every leg is required, including every `Development` leg (`17-SVN`, `24-SVN`, `2026-Preview`) and the `libc++` leg: xstd tracks the development channel deliberately and none of these are advisory.
- **`clang-tidy` is clean.** The [Clang-Tidy workflow](.github/workflows/clang-tidy.yml) runs the checks in [`.clang-tidy`](.clang-tidy) with `WarningsAsErrors: '*'` over the public headers, so any finding fails the job outright - there is no "advisory, fix later" mode.
- **Line and branch coverage stay at 100%, project-wide and for the PR's own diff.** [`codecov.yml`](.github/codecov.yml) sets both the `project` and `patch` Codecov status checks to a 100% target with zero tolerance, backed by the [Coverage workflow](.github/workflows/coverage.yml)'s own `gcovr --fail-under-line 100 --fail-under-branch 100` gate. New code needs a test that exercises every line and branch it adds; existing coverage may not regress. Excluded from this bar: `assert(...)` contract checks (their failure path is undefined behavior by design, not something a correct test can hit), compiler-synthesized `= default;` special members (gcov cannot attribute a hit counter to them regardless of how often they run), and the exception-unwinding branch gcc/gcov attaches to any call that could throw (`--exclude-throw-branches`/`--exclude-unreachable-branches`) - not a code path a test can meaningfully hit either.
- **No new sanitizer failures.** The [sanitizers workflow](.github/workflows/sanitizers.yml) must stay green.
- **The public headers stay self-sufficient.** Each header is compiled as its own translation unit (see `test/CMakeLists.txt`); don't rely on include order from another header.
- **The library stays a leaf.** `test/CMakeLists.txt` fails the configure step if any public header includes `<xstd/ints/...>` or `<xstd/bits/...>`. A facility that needs one of them belongs in that library, not here.
- **`clang-format` is clean.** The [Clang-Format workflow](.github/workflows/clang-format.yml) runs `clang-format --dry-run --Werror` over every header and test source against [`.clang-format`](.clang-format), so any diff fails the job. Run `clang-format -i` on changed files before pushing.
- **Workflow files pass `actionlint`.** The [Actionlint workflow](.github/workflows/actionlint.yml) validates GitHub Actions syntax and expressions.
- **The documented consumption methods work.** The [Consumption workflow](.github/workflows/consumption.yml) builds a consumer using `find_package`, `add_subdirectory`, and `FetchContent`.
- **CodeQL analysis is clean.** The [CodeQL workflow](.github/workflows/codeql.yml) runs the C/C++ `security-extended` query suite.

Match the surrounding code's style by eye where `.clang-format` doesn't have an opinion, including the Boost Software License header comment at the top of every source and workflow file.

## Test suite requirements

Keep a focused test source for each public function, concept, and trait, mirroring its header path under `test/src/`. A source matching an umbrella header (for example, `concepts.cpp`, `type_traits.cpp`, or `utility.cpp`) tests only identities that span several focused facilities.

The library itself has no dependencies. Its test suite has a few, and none of those are needed to *use* xstd-misc:

| Tool | Needed for | Notes |
| :--- | :--------- | :---- |
| A conforming C++23 compiler | everything | Same requirement as the library; see the table in [README.md](README.md) for the versions under CI |
| [CMake](https://cmake.org/) 3.28+ | configuring and building | `cmake_minimum_required` in [`CMakeLists.txt`](CMakeLists.txt); CTest ships with it |
| [Boost.Test](https://www.boost.org/doc/libs/release/libs/test/) 1.70+ | the unit tests under `test/src/` | The floor the CMake project asks for, shared with the other xstd repositories; declared in the checked-in [`vcpkg.json`](vcpkg.json) manifest, the `*-vcpkg` presets pick it up from a `VCPKG_ROOT`-configured vcpkg, or install it with your system package manager |
| [gcovr](https://gcovr.com/) | reproducing the coverage gate | Only for the workflow below; `pip install gcovr` |
| `clang-tidy` and `run-clang-tidy` | reproducing the clang-tidy gate | Only for the workflow below |
| `clang-format` | the formatting gate | Run `clang-format -i` on changed files before pushing |

## Building and testing locally

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

The repository also provides CMake presets for common local configurations:

```sh
cmake --preset dev
cmake --build --preset dev
ctest --preset dev
```

The `*-vcpkg` presets additionally resolve the Boost dependency through vcpkg, using the toolchain at `VCPKG_ROOT`:

```sh
cmake --preset dev-vcpkg
cmake --build --preset dev-vcpkg
ctest --preset dev-vcpkg
```

### Reproducing the coverage gate

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS="--coverage -O0 -g" -DCMAKE_EXE_LINKER_FLAGS="--coverage"
cmake --build build
ctest --test-dir build --output-on-failure
gcovr --root . --exclude 'test/.*' --exclude 'build/.*' \
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

## Standards-paper tracking

Implementation-relevant proposals must record the paper number, the revision-pinned wording reviewed by xstd, the corresponding live `cplusplus/papers` tracking issue, the last-reviewed date, and a short statement saying whether xstd implements, partially follows, extends, or merely references the paper. Unversioned `wg21.link/P####` links are reserved for links explicitly described as the latest revision; implementation and conformance claims use pinned `P####R#` links. Durable prose does not copy volatile WG21 status labels.

A new paper revision or a change on its tracking issue triggers a review, not an automatic code change. Update the review date only after inspecting the relevant revision and reconsidering xstd's stated relationship.

| Paper | Reviewed wording | Tracking issue | Last reviewed | Relationship |
|---|---|---|---|---|
| P1682R1 | [P1682R1](https://wg21.link/P1682R1) | [cplusplus/papers#460](https://github.com/cplusplus/papers/issues/460) | 2026-09-04 | Supplies `std::to_underlying`, which the plain-enum overload delegates to rather than reimplements. xstd adds an overload the paper does not cover: an enum value wrapped in `std::integral_constant`, returned as an `integral_constant` of the underlying type. [doc/ideas.md](doc/ideas.md) is the 2016 sketch behind the paper. |
| P2098R1 | [P2098R1](https://wg21.link/P2098R1) | [cplusplus/papers#812](https://github.com/cplusplus/papers/issues/812) | 2026-09-04 | Supplies the name and the semantics for class templates whose parameters are types. xstd implements that form and its `_v` variable template, and adds the concept spelling `specialization_of`; the standard library has no counterpart to defer to. |

## License

By contributing, you agree that your contributions will be licensed under the [Boost Software License, Version 1.0](LICENSE_1_0.txt), the same license that covers the rest of this repository.
