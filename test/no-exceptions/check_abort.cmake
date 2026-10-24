#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# Runs a program that is to abort, and passes when it fails and writes the expected message to stderr. CTest counts an
# aborted process as failed whatever else is set, so the verdict is this script's:
#
#   cmake -Dprogram=<executable> -Dmessage=<text> [-Demulator=<program>] -P check_abort.cmake
#
# The emulator is the program's CROSSCOMPILING_EMULATOR, which add_test would otherwise prepend.

foreach(argument program message)
    if(NOT DEFINED ${argument})
        message(FATAL_ERROR "check_abort.cmake needs -D${argument}=...")
    endif()
endforeach()

execute_process(
    COMMAND ${emulator} ${program}
    OUTPUT_VARIABLE printed
    ERROR_VARIABLE diagnostics
    RESULT_VARIABLE status
)
if(status EQUAL 0)
    message(FATAL_ERROR "${program} exited normally where it was to abort\n${printed}${diagnostics}")
endif()

string(FIND "${diagnostics}" "${message}" found)
if(found EQUAL -1)
    message(FATAL_ERROR "${program} failed with ${status} but did not write \"${message}\"\n${diagnostics}")
endif()
