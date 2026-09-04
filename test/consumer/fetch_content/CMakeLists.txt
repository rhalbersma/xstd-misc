#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.28)
project(consumer_fetch_content LANGUAGES CXX)

include(FetchContent)

# The repository root, found by walking up rather than through the
# environment, so this consumer configures outside CI too.
get_filename_component(xstd_misc_root "${CMAKE_CURRENT_SOURCE_DIR}/../../.." ABSOLUTE)

FetchContent_Declare(xstd-misc SOURCE_DIR "${xstd_misc_root}")
FetchContent_MakeAvailable(xstd-misc)

add_executable(main ../main.cpp)
target_link_libraries(main PRIVATE xstd::misc)
