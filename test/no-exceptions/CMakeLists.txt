#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# The headers under -fno-exceptions, as a project built that way includes them: GCC rejects every throw it compiles
# there, so a header that throws outright fails to build. Boost.Test reports a failed check by throwing, so these are
# plain programs. One formats as the unit tests do; the other gives a specification the formatter rejects, and
# check_abort.cmake passes it when it aborts having written the formatter's message. MSVC and clang-cl turn exceptions
# off through /EHs- and the STL's _HAS_EXCEPTIONS, which its <format> does not support, so the suite builds where
# -fno-exceptions does.
if(MSVC)
    return()
endif()

foreach(program format_digest format_digest_rejects)
    add_executable(no_exceptions.${program} ${program}.cpp)
    target_link_libraries(no_exceptions.${program} PRIVATE xstd::misc ${project_id}_test_options)
    # Last, so it wins over a caller's -fexceptions, as Emscripten's workflow passes.
    target_compile_options(no_exceptions.${program} PRIVATE -fno-exceptions)
    target_link_options(no_exceptions.${program} PRIVATE -fno-exceptions)
endforeach()

add_test(NAME no_exceptions.format_digest COMMAND no_exceptions.format_digest)

add_test(
    NAME no_exceptions.format_digest_rejects
    COMMAND ${CMAKE_COMMAND}
        -Dprogram=$<TARGET_FILE:no_exceptions.format_digest_rejects>
        -Demulator=$<TARGET_PROPERTY:no_exceptions.format_digest_rejects,CROSSCOMPILING_EMULATOR>
        "-Dmessage=a digest takes x, X or no format specification"
        -P ${CMAKE_CURRENT_SOURCE_DIR}/check_abort.cmake
)
