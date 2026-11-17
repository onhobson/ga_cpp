
function(add_example TARGET_NAME)
    add_executable(${TARGET_NAME} ${ARGN})

    target_link_libraries(${TARGET_NAME}
        PRIVATE
            ga::ga
    )

    add_custom_target("run_${TARGET_NAME}"
        COMMAND "$<TARGET_FILE:${TARGET_NAME}>"
        DEPENDS ${TARGET_NAME}
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
endfunction()

add_example(att48
    att48/att48.cpp
)

add_example(basic_usage
    basic_usage/basic_usage.cpp
)
