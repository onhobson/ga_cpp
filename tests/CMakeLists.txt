
add_executable(ga_tests
    mutate_test.cpp
    random_test.cpp
    operations/functions/mutate_test.cpp
    operations/functions/crossover_test.cpp
)

target_link_libraries(ga_tests
    PRIVATE
        ga
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(ga_tests)
