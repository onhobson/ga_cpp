include(GoogleTest)

function(package_add_test TESTNAME)
    add_executable("${TESTNAME}" ${ARGN})

    target_include_directories("${TESTNAME}"
        PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}"
    )
    
    target_link_libraries("${TESTNAME}" 
        PRIVATE
            ga 
            GTest::gtest_main
    )
    
    gtest_discover_tests("${TESTNAME}"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    )

    set_target_properties("${TESTNAME}" PROPERTIES FOLDER tests)
endfunction()

package_add_test(common 
    common/random_test.cpp
    common/constraints_test.cpp
)

package_add_test(operations 
    operations/operator_selector_test.cpp
    operations/mutate_test.cpp
    operations/crossover_test.cpp
)

package_add_test(crossover_misc_functions
    operations/functions/crossover/misc/single_point_test.cpp
)


package_add_test(crossover_permutation_functions
    operations/functions/crossover/permutation/alternating_edge_test.cpp
    operations/functions/crossover/permutation/order_test.cpp
    operations/functions/crossover/permutation/edge_recombination_test.cpp
)

package_add_test(mutate_permutation_functions
    operations/functions/mutate/permutation/insert_test.cpp
    operations/functions/mutate/permutation/inversion_test.cpp
    operations/functions/mutate/permutation/scramble_test.cpp
    operations/functions/mutate/permutation/swap_test.cpp
)

package_add_test(core
    core/populator_test.cpp
    core/evaluator_test.cpp
    core/config_test.cpp
    core/config_effects_test.cpp
    core/genetic_algorithm_test.cpp
)
