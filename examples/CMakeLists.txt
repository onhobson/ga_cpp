add_executable(basic_usage
    basic_usage/main.cpp
)

target_link_libraries(basic_usage
    PRIVATE
        ga
)

add_custom_target(run_basic_usage
    COMMAND "$<TARGET_FILE:basic_usage>"
    DEPENDS basic_usage
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
