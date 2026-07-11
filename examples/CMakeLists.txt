project(asyffi-helper-examples)

function(add_example filename)
    add_library(
            ${filename}
            MODULE EXCLUDE_FROM_ALL
            ${filename}.cc
    )
    target_link_libraries(${filename} PUBLIC asyffi-helper-lib)
endfunction()

add_example(threadSample)
