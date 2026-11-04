cmake_minimum_required(VERSION 3.20...4.4)

project(GeneticAlgorithm LANGUAGES CXX)

add_library(ga INTERFACE)

target_include_directories(ga
    INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_features(ga INTERFACE cxx_std_23)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    option(BUILD_TESTS "Build the tests" ON)
else()
    option(BUILD_TESTS "Build the tests" OFF)
endif()

option(BUILD_EXAMPLES "Build examples" ON)

if(BUILD_TESTS)
    enable_testing()
    
    include(FetchContent)

    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.18.0
    )
    FetchContent_MakeAvailable(googletest)

    add_subdirectory(tests)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
