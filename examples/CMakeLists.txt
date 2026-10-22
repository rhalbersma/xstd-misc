#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# The examples written for the reference, under the Boost Software License. Each <group>/<header>.cpp is the Example
# section of doc/reference/<group>/<header>.md: built with the tests' warnings, run, and what it prints compared with
# the <header>.out beside it, which the page shows as its Output. The ported examples under cppreference/ are built by
# a CMakeLists.txt of their own, under their own licences.
file(
    GLOB_RECURSE examples
    CONFIGURE_DEPENDS
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)
list(FILTER examples EXCLUDE REGEX "^cppreference/")

foreach(e ${examples})
    string(REGEX REPLACE "[.]cpp$" "" stem ${e})
    string(REPLACE "/" "." example_id ${stem})
    set(expected ${CMAKE_CURRENT_SOURCE_DIR}/${stem}.out)
    if(NOT EXISTS ${expected})
        message(FATAL_ERROR "examples/${e} has no examples/${stem}.out: run it and save what it prints there")
    endif()

    add_executable(example.${example_id} ${e})
    target_link_libraries(
        example.${example_id} PRIVATE
        xstd::misc
        ${project_id}_test_options
    )
    # An example lays a class out as its reader would, and -Wpadded reports the padding of every such layout.
    target_compile_options(example.${example_id} PRIVATE $<$<CXX_COMPILER_ID:Clang>:-Wno-padded>)
    add_test(
        NAME example.${example_id}
        COMMAND ${CMAKE_COMMAND}
            -Dexample=$<TARGET_FILE:example.${example_id}>
            -Demulator=$<TARGET_PROPERTY:example.${example_id},CROSSCOMPILING_EMULATOR>
            -Dexpected=${expected}
            -P ${CMAKE_CURRENT_SOURCE_DIR}/check_output.cmake
    )
endforeach()

if(NOT examples)
    message(FATAL_ERROR "examples/ holds no example, so no reference page has one")
endif()
