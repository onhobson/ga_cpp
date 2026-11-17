# Installation Guide {#installation}

The library is header-only and requires CMake 3.20 or later and a compiler with C++23 support. Its exported CMake target provides the headers and C++ standard requirement to consuming projects.

## Install to a Prefix

Clone the repository, configure an installation prefix, and install:

@code{.sh}
git clone https://github.com/onhobson/ga_cpp.git
cd ga_cpp
cmake -S . -B build -DCMAKE_INSTALL_PREFIX="<installation-prefix>"
cmake --install build
@endcode

Replace `<installation-prefix>` with the directory where you want the library installed.

In the consuming project's `CMakeLists.txt`, locate the package and link the exported `ga::ga` target:

@code{.cmake}
find_package(GeneticAlgorithm CONFIG REQUIRED)

target_link_libraries(my_program PRIVATE ga::ga)
@endcode

When configuring the consuming project, point CMake at the installation prefix if it is not already in a standard package search location:

@code{.sh}
cmake -S . -B build -DCMAKE_PREFIX_PATH="<installation-prefix>"
@endcode

## Use CMake `FetchContent`

To fetch the library as part of your project configuration, declare it in the consuming project's `CMakeLists.txt` and link the provided target:

@code{.cmake}
include(FetchContent)

FetchContent_Declare(
    GeneticAlgorithm
    GIT_REPOSITORY https://github.com/onhobson/ga_cpp.git
    GIT_TAG v1.0.0
)

FetchContent_MakeAvailable(GeneticAlgorithm)

target_link_libraries(my_program PRIVATE ga::ga)
@endcode
