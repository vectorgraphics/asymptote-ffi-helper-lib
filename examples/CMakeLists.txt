project(asyffi-helper-examples)

function(add_example filename)
    add_library(
            ${filename}
            MODULE EXCLUDE_FROM_ALL
            ${filename}.cc
    )
    target_link_libraries(${filename} PUBLIC asyffi_helper)
endfunction()

add_example(threadSample)
add_example(structSample)
