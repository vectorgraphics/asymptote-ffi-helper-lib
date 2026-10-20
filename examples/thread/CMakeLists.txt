
# thread -- self-contained Asymptote FFI plugin (parallel random pens/points).
# See ../README.md for build and run instructions.

cmake_minimum_required(VERSION 3.31.0)
project(thread-ffi-plugin CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# The asyffi_helper repository lives two directories up (examples/thread -> root).
set(ASYFFI_HELPER_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)

# Pull in the helper library: defines the asyffi_helper target and loads
# cmake/asyffi_plugin.cmake (giving us asyffi_add_plugin).
add_subdirectory(
    ${ASYFFI_HELPER_ROOT}
    ${CMAKE_BINARY_DIR}/helperlib
    EXCLUDE_FROM_ALL
)

# Build the plugin and force the output into THIS directory (".") so the .so
# sits right next to test.asy.
asyffi_add_plugin(thread ${CMAKE_CURRENT_SOURCE_DIR}/thread.cc)
set_target_properties(thread PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

