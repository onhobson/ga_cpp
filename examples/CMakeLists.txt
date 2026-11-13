add_executable(att48
    att48/att48.cpp
)

target_link_libraries(att48
    PRIVATE
        ga::ga
)

add_custom_target(run_att48
    COMMAND "$<TARGET_FILE:att48>"
    DEPENDS att48
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
