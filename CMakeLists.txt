CMAKE_MINIMUM_REQUIRED(VERSION 3.31.0)
include(FetchContent)

project(asyffi-helper-lib CXX)

# whether to enable compilation of examples. In production environment, this
# should be turned off to speed cmake loading (unless if examples are needed)
option(ASYFFI_HELPER_LOAD_EXAMPLES "Whether to enable loading of example targets" FALSE)

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
        types threads item contextFuncs functions
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
        asyffi-helper-lib STATIC
        ${ASYFFI_HELPER_SRC_FILES}
)

# and fetch the asyffi.h file
FetchContent_Declare(
        asyffi-dll-dl
        URL ${ASYFFI_HEADER_DOWNLOAD_URL}
        DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(asyffi-dll-dl)

# and include them alongside include/ as our include path
target_include_directories(
        asyffi-helper-lib
        PUBLIC ${asyffi-dll-dl_SOURCE_DIR} include
)

if (ASYFFI_HELPER_LOAD_EXAMPLES)
    add_subdirectory(examples)
endif()
