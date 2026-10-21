
# asyffi_plugin.cmake
#
# asyffi_add_plugin(target SRCS... [LIBNAME <name>] [ASY_PATH_DIR <dir>])
#
# A single-function helper for building a loadable Asymptote FFI plugin. It
# removes the fiddly, error-prone parts of doing it by hand:
#
#   - a SHARED target with POSITION_INDEPENDENT_CODE ON and C++20;
#   - the include dirs for asyffi.h and the asyffihelpers/ headers, reusing
#     whatever location the asyffi_helper target already resolved (so you do
#     NOT need to re-specify ASYFFI_HEADER_DOWNLOAD_URL);
#   - the correct output name (lib<name>.so on Linux, lib<name>.dll on Windows);
#   - an optional copy step that drops the built library into an Asymptote
#     search-path directory so the plugin can be `import lib<name>`'d directly.
#
# Usage (from a consumer project that has the asyffi_helper target available,
# e.g. via add_subdirectory of this library):
#
#   asyffi_add_plugin(hello hello.cc)
#   asyffi_add_plugin(myplug myplug.cc LIBNAME myplug ASY_PATH_DIR ${CMAKE_INSTALL_PREFIX}/asy)
#
# Notes:
#   * A leading "lib" in LIBNAME is stripped, so `LIBNAME libhello` and
#     `LIBNAME hello` both produce libhello.so (Linux) / libhello.dll (Windows).
#   * Symbol export is handled by asyffi.h itself: the generated
#     `registerAsymptotePlugin` entry point is annotated with ASY_FFI_EXPORT
#     ([[gnu::visibility("default")]] on ELF, __declspec(dllexport) on
#     Windows), which overrides any -fvisibility=hidden the target might use.
#   * On Windows the same code produces a .dll; no extra flags are required
#     beyond what is set here.

function(asyffi_add_plugin target)
    # Parse: positional source files (SRCS...), plus the LIBNAME and
    # ASY_PATH_DIR options that take a following value.
    set(_srcs "")
    set(_args ${ARGN})
    list(LENGTH _args _n)
    set(_i 0)
    while(_i LESS _n)
        list(GET _args ${_i} _arg)
        if(_arg STREQUAL "LIBNAME")
            math(EXPR _i "${_i}+1")
            list(GET _args ${_i} _val)
            set(AP_LIBNAME ${_val})
        elseif(_arg STREQUAL "ASY_PATH_DIR")
            math(EXPR _i "${_i}+1")
            list(GET _args ${_i} _val)
            set(AP_ASY_PATH_DIR ${_val})
        else()
            list(APPEND _srcs "${_arg}")
        endif()
        math(EXPR _i "${_i}+1")
    endwhile()
    set(AP_SRCS ${_srcs})

    if(NOT AP_SRCS)
        message(FATAL_ERROR "asyffi_add_plugin(${target}): no SRCS given")
    endif()

    # --- Library name (used for the output file) ---------------------------
    # Default to the target name; strip a leading "lib" so callers can write
    # either "hello" or "libhello".
    if(NOT AP_LIBNAME)
        set(AP_LIBNAME ${target})
    endif()
    if(AP_LIBNAME MATCHES "^lib(.+)$")
        set(_asyffi_plugin_name ${CMAKE_MATCH_1})
    else()
        set(_asyffi_plugin_name ${AP_LIBNAME})
    endif()

    # --- Target -------------------------------------------------------------
    # PREFIX is forced to "lib" on all platforms so the built file is
    # lib<name>.so (Linux) / lib<name>.dll (Windows) -- same base name on both,
    # giving one extension-less import name that is portable across platforms.
    add_library(${target} SHARED ${AP_SRCS})

    set_target_properties(${target} PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        OUTPUT_NAME ${_asyffi_plugin_name}
        PREFIX "lib"
    )

    # --- Includes & helper symbols -----------------------------------------
    # Linking the asyffi_helper static lib propagates both the asyffi.h include
    # dir and the asyffihelpers/ include dir (they are PUBLIC on that target),
    # and reuses the header location already resolved by the helper build.
    if(TARGET asyffi_helper)
        target_link_libraries(${target} PUBLIC asyffi_helper)
    elseif(DEFINED asyffi-header-dl_SOURCE_DIR)
        # Fallback: the FetchContent'd header is available but we do not want
        # to force the static lib into the link line.
        target_include_directories(
            ${target} PUBLIC
            ${asyffi-header-dl_SOURCE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/../include
        )
    else()
        message(
            FATAL_ERROR
            "asyffi_add_plugin(${target}): could not locate the asyffi.h header "
            "or the asyffi_helper target. Build the asyffi_helper library first "
            "(it resolves ASYFFI_HEADER_DOWNLOAD_URL and exposes its include dirs)."
        )
    endif()

    # --- Optional: copy into an Asymptote search path ----------------------
    if(AP_ASY_PATH_DIR)
        add_custom_command(
            TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory ${AP_ASY_PATH_DIR}
            COMMAND ${CMAKE_COMMAND} -E copy
                    $<TARGET_FILE:${target}> ${AP_ASY_PATH_DIR}/
            COMMENT
                "Copying $<TARGET_FILE_NAME:${target}> into Asymptote search path: ${AP_ASY_PATH_DIR}"
            VERBATIM
        )
    endif()
endfunction()

