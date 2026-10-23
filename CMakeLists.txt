cmake_minimum_required(VERSION 3.20)

project(GeneticAlgorithm LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(ga INTERFACE)

target_include_directories(ga
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

enable_testing()

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.18.0
)

FetchContent_MakeAvailable(googletest)

add_subdirectory(tests)


