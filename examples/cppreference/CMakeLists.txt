#          Copyright Rein Halbersma 2026.
# Distributed under the Creative Commons Attribution-ShareAlike 3.0 Unported License and the GNU Free Documentation
# License, as the examples it builds are: see LICENSE in this directory.

# The examples ported from cppreference.com, each the Example section of the reference page for the entity it was
# ported onto. Each <page>.cpp is built with the tests' warnings, run, and what it prints compared with the <page>.out
# beside it, which that reference page shows as its Output.
file(
    GLOB ported_examples
    CONFIGURE_DEPENDS
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

foreach(e ${ported_examples})
    string(REGEX REPLACE "[.]cpp$" "" stem ${e})
    set(expected ${CMAKE_CURRENT_SOURCE_DIR}/${stem}.out)
    if(NOT EXISTS ${expected})
        message(FATAL_ERROR "examples/cppreference/${e} has no ${stem}.out: run it and save what it prints there")
    endif()

    add_executable(example.cppreference.${stem} ${e})
    target_link_libraries(
        example.cppreference.${stem} PRIVATE
        xstd::misc
        ${project_id}_test_options
    )
    # An example lays a class out as its reader would, and -Wpadded reports the padding of every such layout.
    target_compile_options(example.cppreference.${stem} PRIVATE $<$<CXX_COMPILER_ID:Clang>:-Wno-padded>)
    add_test(
        NAME example.cppreference.${stem}
        COMMAND ${CMAKE_COMMAND}
            -Dexample=$<TARGET_FILE:example.cppreference.${stem}>
            -Demulator=$<TARGET_PROPERTY:example.cppreference.${stem},CROSSCOMPILING_EMULATOR>
            -Dexpected=${expected}
            -P ${PROJECT_SOURCE_DIR}/examples/check_output.cmake
    )
endforeach()

if(NOT ported_examples)
    message(FATAL_ERROR "examples/cppreference/ holds no example")
endif()
