cmake_minimum_required(VERSION 3.31.0)
include(FetchContent)

project(asyffi-helper-lib CXX)

option(ASYFFI_BUILD_DOCS "Whether to build documentation" TRUE)

set(
        ASYFFI_HEADER_DOWNLOAD_URL
        "https://raw.githubusercontent.com/vectorgraphics/asymptote/52de09fa50a5f0c35a6d5f2ed50413f80b8ef730/asyffi.h"
        CACHE STRING
        "URL (or local file path) for downloading asymptote ffi header"
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Single-function helper for building loadable FFI plugins: asyffi_add_plugin().
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/asyffi_plugin.cmake)

set(
        ASYFFI_HELPER_SRC_FILES
        types threads contextFuncs functions
        typeObjs pen structs itemFnStructs
)

list(
        TRANSFORM
        ASYFFI_HELPER_SRC_FILES
        PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/src/
)

list(
        TRANSFORM
        ASYFFI_HELPER_SRC_FILES
        APPEND .cc
)


add_library(
        asyffi_helper STATIC
        ${ASYFFI_HELPER_SRC_FILES}
)

set_target_properties(asyffi_helper PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

# and fetch the asyffi.h file
FetchContent_Declare(
        asyffi-header-dl
        URL ${ASYFFI_HEADER_DOWNLOAD_URL}
        DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(asyffi-header-dl)

# and include them alongside include/ as our include path
target_include_directories(
        asyffi_helper
        PUBLIC ${asyffi-header-dl_SOURCE_DIR} include
)

enable_testing()
add_subdirectory(tests)

# Build the example plugins only when this is the top-level project. When a
# standalone example does add_subdirectory(root), it would otherwise recurse
# back into the same example target.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    add_subdirectory(examples)
endif()

# docgen

if (ASYFFI_BUILD_DOCS)
    find_package(Doxygen)
    if (Doxygen_FOUND)
        doxygen_add_docs(
                asyffi_helper_docs
                ${asyffi-header-dl_SOURCE_DIR}/asyffi.h include/
        )
    endif()
endif()
