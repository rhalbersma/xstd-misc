#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

include(FetchContent)
FetchContent_Declare(
    cpp-ci
    GIT_REPOSITORY https://github.com/rhalbersma/cpp-ci.git
    # The commit .github/workflows/ pins, so the suites' tags are the rungs' the ladders build on.
    GIT_TAG acbf0f0b3ed85dcabf95cd0d4806c0fa7c451ecf
)
FetchContent_MakeAvailable(cpp-ci)
include(${cpp-ci_SOURCE_DIR}/cmake/StdTestSuite.cmake)

# The suites test to_underlying from C++23, the first standard to declare it.
set(CMAKE_CXX_STANDARD 23)

std_test_suite(libcxx MAPPING mapping.toml LINK_LIBRARIES xstd::misc)
std_test_suite(libstdcxx MAPPING mapping.toml LINK_LIBRARIES xstd::misc)
std_test_suite(msvc-stl MAPPING mapping.toml LINK_LIBRARIES xstd::misc)
