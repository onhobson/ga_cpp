
add_executable(ga_tests
    common/random_test.cpp
    operations/functions/mutate_test.cpp
    operations/functions/crossover_test.cpp
    operations/operator_selector_test.cpp
    operations/mutate_test.cpp
    operations/crossover_test.cpp
    core/populator_test.cpp
    core/evaluator_test.cpp
    core/config_test.cpp
    core/genetic_algorithm_test.cpp
)

target_link_libraries(ga_tests
    PRIVATE
        ga
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(ga_tests)
