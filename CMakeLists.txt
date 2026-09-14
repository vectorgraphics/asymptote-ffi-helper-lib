CMAKE_MINIMUM_REQUIRED(VERSION 3.31.0)
include(FetchContent)

project(asyffi-helper-lib CXX)

# whether to enable compilation of examples. In production environment, this
# should be turned off to speed cmake loading (unless if examples are needed)
option(ASYFFI_HELPER_LOAD_EXAMPLES "Whether to enable loading of example targets" FALSE)

option(ASYFFI_BUILD_DOCS "Whether to build documentation" TRUE)

set(
        ASYFFI_HEADER_DOWNLOAD_URL
        "https://raw.githubusercontent.com/vectorgraphics/asymptote/52de09fa50a5f0c35a6d5f2ed50413f80b8ef730/asyffi.h"
        CACHE STRING
        "URL (or local file path) for downloading asymptote ffi header"
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

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

if (ASYFFI_HELPER_LOAD_EXAMPLES)
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
