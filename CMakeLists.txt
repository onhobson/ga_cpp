cmake_minimum_required(VERSION 3.20...4.4)

project(GeneticAlgorithm 
    VERSION 1.0.0
    LANGUAGES CXX
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)


# --------------------------------------------------
# Library target
# --------------------------------------------------


add_library(ga INTERFACE)

add_library(ga::ga ALIAS ga)

target_include_directories(ga
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)


# --------------------------------------------------
# Installation and export
# --------------------------------------------------


target_compile_features(ga INTERFACE cxx_std_23)

install(TARGETS ga EXPORT gaTargets)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT gaTargets
    FILE gaTargets.cmake
    NAMESPACE ga::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/GeneticAlgorithm
)

# Package configuration
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GeneticAlgorithmConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/GeneticAlgorithmConfig.cmake"
    INSTALL_DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/GeneticAlgorithm
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/GeneticAlgorithmConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/GeneticAlgorithmConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/GeneticAlgorithmConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/GeneticAlgorithm
)


# --------------------------------------------------
# Tests
# --------------------------------------------------

option(BUILD_TESTS "Build the tests" OFF)

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
