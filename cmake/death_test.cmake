# Runs a death-test executable and passes only if it terminates abnormally
# AND printed an assert/check failure message. Invoked by ctest as:
#   cmake -DEXE=<path> [-DEMULATOR=<a|b|c>] -P death_test.cmake
# (EMULATOR uses '|' as separator because ';' would be split by add_test.)
#
# Needed because ctest reports death-by-signal (SIGABRT/SIGTRAP on Linux) as
# "Exception", which WILL_FAIL does not invert.

string(REPLACE "|" ";" emulator "${EMULATOR}")
execute_process(
    COMMAND ${emulator} "${EXE}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE out
    ERROR_VARIABLE err
)

if(result STREQUAL "0")
    message(FATAL_ERROR "exited cleanly; expected an assert/check to fire\n${out}${err}")
endif()
if(NOT "${out}${err}" MATCHES "(Assertion|Check) failed:")
    message(FATAL_ERROR "terminated (${result}) without an assert/check message:\n${out}${err}")
endif()
message(STATUS "terminated as expected (${result})")
