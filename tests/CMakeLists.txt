include(GoogleTest)

add_executable("ga_tests"
    unit/builtins/init_test.cpp
    unit/builtins/terminators_test.cpp
    
    unit/builtins/operators/crossover/alternating_edge_test.cpp
    unit/builtins/operators/crossover/edge_recombination_test.cpp
    unit/builtins/operators/crossover/order_test.cpp
    unit/builtins/operators/crossover/single_point_test.cpp
    
    unit/builtins/operators/mutate/insert_test.cpp
    unit/builtins/operators/mutate/inversion_test.cpp
    unit/builtins/operators/mutate/scramble_test.cpp
    unit/builtins/operators/mutate/swap_test.cpp

    unit/core/config_test.cpp
    unit/core/evaluator_test.cpp
    unit/core/populator_test.cpp
    unit/core/selector_test.cpp

    unit/operations/crossover_test.cpp
    unit/operations/mutate_test.cpp
    unit/operations/operator_selector_test.cpp

    unit/utility/constraints_test.cpp
    unit/utility/random_get_exclude_test.cpp
    unit/utility/random_get_integral_test.cpp
    unit/utility/random_get_floating_test.cpp
    unit/utility/random_seed_test.cpp

    integration/config_effects_test.cpp
    integration/evaluation_test.cpp
    integration/genetic_algorithm_test.cpp
    integration/initialization_test.cpp
    integration/termination_test.cpp

    end_to_end/basic_usage_test.cpp
)

target_include_directories("ga_tests"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries("ga_tests" 
    PRIVATE
        ga::ga 
        GTest::gtest_main
)

gtest_discover_tests("ga_tests"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)

set_target_properties("ga_tests" 
    PROPERTIES 
        FOLDER tests
)
