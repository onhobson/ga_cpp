# Genetic Algorithm in C++

A generic, header-only C++ library for building and configuring genetic algorithms. Provides a flexible framework for evolutionary optimization with customizable crossover, mutation, and termination strategies.

## Features

- Configurable population and generation parameters.
- Preconfigured crossover and mutation operators, as well as support for custom operators.
- Support for custom population initialization strategies.
- Configurable termination policies.
- Callback support for monitoring algorithm execution.

## Requirements

- C++ 23 capable compiler.
- CMake 3.20 or later.

## Documentation

Full API reference, usage guides, and examples are available in the project documentation.

[Link to Documentation](https://onhobson.github.io/ga_cpp/index.html)

## Installation

### CMake - `find_package()`

Clone the repository:

```bash
git clone https://github.com/onhobson/ga_cpp.git
cd ga_cpp
```

Configure and install the library:

```bash
cmake -S . -B build -DCMAKE_INSTALL_PREFIX="<installation-prefix>"
cmake --install build
```

Replace \<installation-prefix\> with the desired installation directory.

After installation, configure your project with the installation prefix:

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH="<installation-prefix>"
```

In your project's CMakeLists.txt, use the package name `ga_cpp` and link the exported `ga::ga` target:

```cmake
find_package(ga_cpp CONFIG REQUIRED)

target_link_libraries(my_program PRIVATE ga::ga)
```

### CMake - `FetchContent()`

In your project's CMakeLists.txt:

```cmake
include(FetchContent)

FetchContent_Declare(
    ga_cpp
    GIT_REPOSITORY https://github.com/onhobson/ga_cpp.git
    GIT_TAG v1.0.0
)

FetchContent_MakeAvailable(ga_cpp)

target_link_libraries(my_program PRIVATE ga::ga)
```


## Example

### A Traveling Salesman Problem Heuristic

This example describes using the library to solve a traveling salesman problem (TSP). The raw code can be seen at [examples/att48/att48.cpp](examples/att48/att48.cpp)

This example defines a `Sequence` as:
```cpp
using Sequence = std::vector<std::size_t>;
```

#### Define an Evaluation Function

The library requires a fitness evaluation function to assess the quality of each candidate solution. For the TSP, this function calculates the total distance of a given sequence of cities.

```cpp
int evalFunc(const Sequence& seq)
{
    int fitness = 0;

    for (std::size_t i { 0 }; i < seq.size(); ++i)
        fitness += distances[ seq[i] ][ seq[(i + 1) % seq.size()] ];

    return fitness;
}
```

#### Initialize a Config object and the GeneticAlgorithm object

The `ga::Config` object is used to configure the genetic algorithm's parameters, such as population size, elite count, and tournament size.

```cpp
ga::Config config {
    .populationSize = 50,
    .eliteCount = 3,
    .tournamentSize = 3
};
```

The `ga::GeneticAlgorithm` object is then created with the specified sequence type and fitness type, along with the evaluation function, predefined permutation operators, and the config object.

```cpp
ga::GeneticAlgorithm<Sequence, int> ga {
    evalFunc,
    ga::prebuilt::permutationOperators<Sequence>(),
    config
};
```

#### Initialize objects for the `ga.run()` call

##### Sequence

The `run()` method requires either a base sequence and an initializer function or a pre-initialized population. In this example, we use a base sequence initialized with values from 0 to the size of the att48 matrix and a predefined permutation initializer.

```cpp
Sequence base(distances.size());
std::ranges::iota(base, 0);
```

##### Options

The `ga::RunOptions` object is used to specify the optional initializer, termination policy, and callback functions for the genetic algorithm run. In this example, we use the default, predefined permutation initializer, a termination policy that terminates after 5000 generations or 250 iterations without improvement, and a callback function that prints when a new best fitness is found.

```cpp
ga::terminate::TerminationPolicy policy {
    5000,
    ga::terminate::StagnationLimit<int>{ 250 }
};
```

```cpp    
struct PrintCB
{
    int prevBest { std::numeric_limits<int>::max() };

    void operator()(const ga::context::CallbackContext<Sequence, int>& context)
    {
        if (context.best.fitness < prevBest)
        {
            prevBest = context.best.fitness;
            std::cout << "Generation: " << context.generation << "\tFitness: " << prevBest << '\n';
        }
    }
};
```

```cpp
ga::RunOptions options {
    .terminator = policy,
    .callback = cb
};
```

#### Run the Genetic Algorithm

Finally, the `run()` method is called with the base sequence and options to execute the genetic algorithm, storing the result.

```cpp
ga::Result result { ga.run(
    base,
    options
)};
```
