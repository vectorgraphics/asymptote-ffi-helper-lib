
# run_roundtrip.cmake — executed via `cmake -P` by ctest.
#
# Expected input variables:
#   ASY_BIN   — absolute path to the asy binary
#   ASY_FILE  — absolute path to the generated .asy file
#   OUT_DIR   — directory for output artifacts

if(NOT ASY_BIN)
    message(FATAL_ERROR "run_roundtrip.cmake: ASY_BIN not set")
endif()
if(NOT ASY_FILE)
    message(FATAL_ERROR "run_roundtrip.cmake: ASY_FILE not set")
endif()
if(NOT OUT_DIR)
    set(OUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
endif()
if(NOT OUT_BASE)
    set(OUT_BASE "rt_test")
endif()
if(NOT NAME)
    set(NAME "roundtrip")
endif()

# 1) Check that the asy binary exists.
if(NOT EXISTS "${ASY_BIN}")
    message(FATAL_ERROR "FFI round-trip test: asy binary not found at '${ASY_BIN}'. "
        "Build the Asymptote core first (cd /home/ai/asymptote && make).")
endif()

# 2) Check that the .asy file exists.
if(NOT EXISTS "${ASY_FILE}")
    message(FATAL_ERROR "FFI round-trip test: generated .asy file not found at '${ASY_FILE}'")
endif()

# 3) Run asy and capture exit code.
set(ASY_OUT_SVG "${OUT_DIR}/${OUT_BASE}.svg")
execute_process(
    COMMAND "${ASY_BIN}" -f svg -o "${OUT_DIR}/${OUT_BASE}" "${ASY_FILE}"
    RESULT_VARIABLE ASY_EXIT
    OUTPUT_VARIABLE ASY_OUT
    ERROR_VARIABLE ASY_ERR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_STRIP_TRAILING_WHITESPACE
)

if(ASY_EXIT EQUAL 0)
    message(STATUS "${NAME}: PASS (asy exit code 0, output: ${ASY_OUT_SVG})")
else()
    message(FATAL_ERROR
        "${NAME}: FAIL\n"
        "  asy exit code: ${ASY_EXIT}\n"
        "  stdout: ${ASY_OUT}\n"
        "  stderr: ${ASY_ERR}")
endif()

