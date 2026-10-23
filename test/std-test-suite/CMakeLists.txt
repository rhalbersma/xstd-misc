#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# cpp-ci's commit is the one the workflows call, so the suites move with a repin and take the ladders' rungs.
set(workflow ${CMAKE_CURRENT_SOURCE_DIR}/../../.github/workflows/gcc.yml)
file(STRINGS ${workflow} cpp_ci_uses REGEX "uses: rhalbersma/cpp-ci/.+@[0-9a-f]+" LIMIT_COUNT 1)
string(REGEX MATCH "@([0-9a-f]+)" cpp_ci_pin "${cpp_ci_uses}")
if(NOT CMAKE_MATCH_1)
    message(FATAL_ERROR "no rhalbersma/cpp-ci pin in ${workflow}")
endif()

include(FetchContent)
FetchContent_Declare(
    cpp-ci
    GIT_REPOSITORY https://github.com/rhalbersma/cpp-ci.git
    GIT_TAG ${CMAKE_MATCH_1}
)
FetchContent_MakeAvailable(cpp-ci)
include(${cpp-ci_SOURCE_DIR}/cmake/StdTestSuite.cmake)

# The suites test to_underlying from C++23, the first standard to declare it.
set(CMAKE_CXX_STANDARD 23)

std_test_suite(libcxx MAPPING mapping.toml LINK_LIBRARIES xstd::misc)
std_test_suite(libstdcxx MAPPING mapping.toml LINK_LIBRARIES xstd::misc)
std_test_suite(msvc-stl MAPPING mapping.toml LINK_LIBRARIES xstd::misc)
