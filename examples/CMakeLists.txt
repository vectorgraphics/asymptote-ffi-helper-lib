# Build all example plugins. Each example is also independently buildable:
#   cd examples/<name> && cmake -S . -B build && cmake --build build
#
# Any subdirectory that contains a CMakeLists.txt is treated as an example.

file(GLOB _example_cmlist "${CMAKE_CURRENT_SOURCE_DIR}/*/CMakeLists.txt")
list(SORT _example_cmlist)
foreach(_cmlist ${_example_cmlist})
    get_filename_component(_ex_dir ${_cmlist} DIRECTORY)
    get_filename_component(_ex_name ${_ex_dir} NAME)
    add_subdirectory(${_ex_dir} ${CMAKE_CURRENT_BINARY_DIR}/${_ex_name})
endforeach()

