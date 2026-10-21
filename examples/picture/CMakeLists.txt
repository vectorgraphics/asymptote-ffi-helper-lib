
# picture -- self-contained Asymptote FFI plugin (draws a path in red).
# See ../README.md for build and run instructions.

cmake_minimum_required(VERSION 3.31.0)
project(picture-ffi-plugin CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# When building standalone (not as part of the root project), pull in the
# helper library: defines the asyffi_helper target and loads
# cmake/asyffi_plugin.cmake (giving us asyffi_add_plugin).
if(NOT TARGET asyffi_helper)
    set(ASYFFI_HELPER_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)
    add_subdirectory(
        ${ASYFFI_HELPER_ROOT}
        ${CMAKE_BINARY_DIR}/helperlib
        EXCLUDE_FROM_ALL
    )
endif()

# Build the plugin and force the output into THIS directory (".") so the .so
# sits right next to test.asy.
asyffi_add_plugin(picture ${CMAKE_CURRENT_SOURCE_DIR}/picture.cc)
set_target_properties(picture PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

