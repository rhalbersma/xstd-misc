#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# The project name as an identifier and as a cache-option prefix, so a rename carries the test tree with it.
string(MAKE_C_IDENTIFIER ${PROJECT_NAME} project_id)
# In that order: TOUPPER alone would leave XSTD-MISC, which is no name a cache option can have.
string(TOUPPER ${project_id} project_option_prefix)

add_library(${project_id}_test_options INTERFACE)
option(${project_option_prefix}_WARNINGS_AS_ERRORS "Treat warnings as errors in ${PROJECT_NAME} tests" ${PROJECT_IS_TOP_LEVEL})

# clang-cl reports CXX_COMPILER_ID Clang but only understands its MSVC-style
# driver flags, not GNU-style ones like -pedantic-errors.
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    set(${project_id}_is_clang_cl ON)
else()
    set(${project_id}_is_clang_cl OFF)
endif()

set(cxx_compile_definitions
    BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE  # https://github.com/boostorg/config/issues/207
    BOOST_ALL_NO_LIB
    BOOST_TEST_MAIN
    $<$<CXX_COMPILER_ID:MSVC>:
        _SCL_SECURE_NO_WARNINGS
    >
)

set(cxx_compile_options_warnings
    $<$<CXX_COMPILER_ID:MSVC>:
        /W4
        /permissive-
    >
    $<$<CXX_COMPILER_ID:Clang>:
        -Weverything
        -Wno-unknown-warning-option         # forward compatibility for new Clang warnings
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-disabled-macro-expansion       # triggered by Boost.Test
        -Wno-global-constructors            # triggered by Boost.Test
        -Wno-used-but-marked-unused         # triggered by Boost.Test
        -Wno-c2y-extensions                 # triggered by Boost.Test's use of __COUNTER__
        -Wno-lifetime-safety-intra-tu-suggestions             # Clang 23+; suggests [[clang::lifetimebound]], which GCC would reject as unknown
        -Wno-lifetime-safety-intra-tu-constructor-suggestions # ditto, for constructors
        -Wno-lifetime-safety-experimental-field-origin-escape # Clang 24+; rejects a non-owning view's accessor returning through its pointer member
    >
    $<$<CXX_COMPILER_ID:GNU>:
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wshadow
        -Wsign-compare
        -Wsign-conversion
        -Wsign-promo
    >
    $<$<AND:$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>,$<NOT:$<BOOL:${${project_id}_is_clang_cl}>>>:
        -pedantic-errors
    >
)

# COFF caps an object at 65,279 sections, which a test instantiated over every Hash2 algorithm exceeds (MSVC's C1128).
# clang-cl needs no flag: LLVM switches to the big-object format by itself once the count calls for it.
set(cxx_compile_options_big_obj
    $<$<CXX_COMPILER_ID:MSVC>:
        /bigobj
    >
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<PLATFORM_ID:Windows>>:
        -Wa$<COMMA>-mbig-obj
    >
)

target_compile_definitions(
    ${project_id}_test_options INTERFACE
    ${cxx_compile_definitions}
)
target_compile_options(
    ${project_id}_test_options INTERFACE
    ${cxx_compile_options_warnings}
    ${cxx_compile_options_big_obj}
    $<$<AND:$<BOOL:${${project_option_prefix}_WARNINGS_AS_ERRORS}>,$<CXX_COMPILER_ID:MSVC>>:/WX>
    $<$<AND:$<BOOL:${${project_option_prefix}_WARNINGS_AS_ERRORS}>,$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>>:-Werror>
)
target_include_directories(
    ${project_id}_test_options INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Each public header is compiled as its own translation unit (linked against
# the same warning flags as the unit tests), proving it is self-sufficient.
# Any target named after the header is one it needs to be self-sufficient against.
set(public_header_dir ${PROJECT_SOURCE_DIR}/include)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/header_self_sufficiency)

function(${project_id}_add_header_self_sufficiency_test h)
    string(REPLACE "/" "." header_test_id ${h})
    string(REGEX REPLACE "[.]hpp$" "" header_test_id ${header_test_id})
    set(header_test_source ${CMAKE_CURRENT_BINARY_DIR}/header_self_sufficiency/${header_test_id}.cpp)
    # A trailing return type because clang-tidy runs over these generated
    # sources too, and modernize-use-trailing-return-type flags "int main()".
    file(
        WRITE ${header_test_source}
        "#include <${h}> // IWYU pragma: keep\nauto main() -> int {}\n"
    )

    add_executable(header.${header_test_id} ${header_test_source})
    target_link_libraries(
        header.${header_test_id} PRIVATE
        xstd::misc
        ${ARGN}
        ${project_id}_test_options
    )
    add_test(NAME header.${header_test_id} COMMAND header.${header_test_id})
endfunction()

file(
    GLOB_RECURSE public_headers
    CONFIGURE_DEPENDS
    RELATIVE ${public_header_dir}
    ${public_header_dir}/xstd/*.hpp
)

# The libraries are leaves: xstd/misc may include neither of its siblings, or
# the split that gave each of them a repository would make a chain of them.
foreach(h ${public_headers})
    file(STRINGS ${public_header_dir}/${h} offending REGEX "#include[ \t]*<xstd/(bits|ints)/")
    if(offending)
        message(FATAL_ERROR "include/${h} includes a sibling library: xstd/misc must stay a leaf.\n  ${offending}")
    endif()
endforeach()

foreach(h ${public_headers})
    cmake_language(CALL ${project_id}_add_header_self_sufficiency_test ${h})
endforeach()

# Each C++ example in the README is compiled as its own translation unit, so the documentation cannot drift from the
# code: readme.example<n> is the README's n-th ```cpp block, with an empty main appended.
set(readme ${PROJECT_SOURCE_DIR}/README.md)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${readme})
file(READ ${readme} readme_text)
# A Windows checkout may carry CRLF line endings, which the fences below are matched without.
string(REPLACE "\r\n" "\n" readme_text "${readme_text}")
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/readme)
set(readme_example 0)
while(TRUE)
    string(FIND "${readme_text}" "\n```cpp\n" fence)
    if(fence EQUAL -1)
        break()
    endif()
    math(EXPR fence "${fence} + 8")
    string(SUBSTRING "${readme_text}" ${fence} -1 readme_text)
    string(FIND "${readme_text}" "\n```\n" fence)
    if(fence EQUAL -1)
        message(FATAL_ERROR "README.md: a ```cpp block is never closed")
    endif()
    math(EXPR fence "${fence} + 1")
    string(SUBSTRING "${readme_text}" 0 ${fence} example_code)
    string(SUBSTRING "${readme_text}" ${fence} -1 readme_text)
    math(EXPR readme_example "${readme_example} + 1")

    set(example_source ${CMAKE_CURRENT_BINARY_DIR}/readme/example${readme_example}.cpp)
    file(WRITE ${example_source} "${example_code}\nauto main() -> int {}\n")
    add_executable(readme.example${readme_example} ${example_source})
    target_link_libraries(
        readme.example${readme_example} PRIVATE
        xstd::misc
        ${project_id}_test_options
    )
    # An example declares its variables at namespace scope, where a reader would declare them in a function.
    target_compile_options(
        readme.example${readme_example} PRIVATE
        $<$<CXX_COMPILER_ID:Clang>:-Wno-missing-variable-declarations -Wno-exit-time-destructors -Wno-unused-const-variable>
    )
    add_test(NAME readme.example${readme_example} COMMAND readme.example${readme_example})
endwhile()
if(readme_example EQUAL 0)
    message(FATAL_ERROR "README.md has no ```cpp block, so no example of it is compiled")
endif()

# Each reference page's example is built with the same warnings, run, and what it prints checked against the output
# the page shows. Those written for this library are under its licence; those ported from cppreference.com are under
# CC-BY-SA 3.0 and the GFDL, and so is the CMakeLists.txt beside them that builds them.
add_subdirectory(${PROJECT_SOURCE_DIR}/examples ${CMAKE_CURRENT_BINARY_DIR}/examples)
add_subdirectory(${PROJECT_SOURCE_DIR}/examples/cppreference ${CMAKE_CURRENT_BINARY_DIR}/examples/cppreference)

# A header under a detail/ directory is machinery, not a public header: what includes it specifies it, and tests it.
list(FILTER public_headers EXCLUDE REGEX "/detail/")

# The test tree mirrors the header tree: every public header is answered by exactly
# one test source at the same path. A reviewer reading include/xstd/misc/utility/to_underlying.hpp
# knows where its tests are without looking. Enforced here so it cannot drift --
# the glob above already lists every header.
set(unmirrored_headers)
foreach(h ${public_headers})
    # xstd/misc/utility/to_underlying.hpp is answered by test/src/misc/utility/to_underlying.cpp.
    string(REGEX REPLACE "^xstd/" "" mirrored ${h})
    string(REGEX REPLACE "[.]hpp$" ".cpp" mirrored ${mirrored})
    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/${mirrored})
        list(APPEND unmirrored_headers "    include/${h}\n      -> expected test/src/${mirrored}")
    endif()
endforeach()

if(unmirrored_headers)
    list(JOIN unmirrored_headers "\n" unmirrored_report)
    message(
        FATAL_ERROR
        "These headers have no mirrored test source:\n${unmirrored_report}\n"
        "Every public header is mirrored by exactly one test source; add it, "
        "or the mirror is no longer a rule."
    )
endif()

# The interface line: no public header opens a detail namespace, so what a user never spells sits in a header under detail/.
set(detail_opening_headers)
foreach(h ${public_headers})
    file(STRINGS ${public_header_dir}/${h} detail_namespaces REGEX "^namespace [a-z:]*detail")
    if(detail_namespaces)
        list(APPEND detail_opening_headers "    include/${h}")
    endif()
endforeach()

if(detail_opening_headers)
    list(JOIN detail_opening_headers "\n" detail_opening_report)
    message(
        FATAL_ERROR
        "These public headers open a detail namespace:\n${detail_opening_report}\n"
        "Move what they define into a header under a detail/ directory and include it from there."
    )
endif()

if(NOT DEFINED Boost_USE_STATIC_LIBS)
    set(Boost_USE_STATIC_LIBS ON)
endif()

find_package(
    Boost 1.70 CONFIG REQUIRED COMPONENTS
    unit_test_framework
)

# The other unordered container the hasher is tested in, beside the standard's; the library itself never names it.
find_package(boost_unordered CONFIG QUIET)
if(TARGET Boost::unordered)
    set(${project_id}_test_boost_unordered Boost::unordered)
else()
    set(${project_id}_test_boost_unordered)
endif()

set(current_source_dir ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(
    GLOB_RECURSE targets
    CONFIGURE_DEPENDS
    RELATIVE ${current_source_dir}
    ${current_source_dir}/*.cpp
)
foreach(t ${targets})
    cmake_path(GET t PARENT_PATH target_path)
    cmake_path(GET t STEM target_name_we)
    string(REPLACE "/" "." target_id ${target_path}/${target_name_we})
    string(REGEX REPLACE "^[.]" "" target_id ${target_id})

    add_executable(${target_id} src/${t})

    target_link_libraries(
        ${target_id} PRIVATE
        xstd::misc
        Boost::unit_test_framework
        ${${project_id}_test_boost_unordered}
        ${project_id}_test_options
    )

    add_test(NAME ${target_id} COMMAND ${target_id} --log_level=all)
endforeach()

# The headers built with exceptions off, as a project compiled with -fno-exceptions includes them.
add_subdirectory(no-exceptions)

# Off by default: the standard libraries' own test suites, fetched at the stable rungs' tags and rewritten onto the
# entities that have a standard counterpart.
option(${project_option_prefix}_STD_TEST_SUITES "Run the libc++ and libstdc++ test suites against the entities with a standard counterpart" OFF)
if(${project_option_prefix}_STD_TEST_SUITES)
    add_subdirectory(std-test-suite)
endif()
