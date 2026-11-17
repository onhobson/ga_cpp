# Initialization Guide {#initialization}

#### What is an Initializer?

An initializer is a callable that creates an initial population from a single base sequence. It must satisfy @ref ga::constraints::InitializationFunction and accept the base sequence, the requested population size, and a @ref ga::Random instance. It returns a `std::vector<Sequence>` whose size matches the requested population size.

An initializer is required when calling @ref ga::GeneticAlgorithm::run "ga::GeneticAlgorithm::run()" with a base sequence. If the algorithm is started from an existing population, the initialization step is bypassed. The initializer produces sequences, not chromosomes; @ref ga::GeneticAlgorithm then wraps those sequences in chromosomes to build the initial population.

#### Using an Initializer

Initializers are supplied to @ref ga::GeneticAlgorithm::run "ga::GeneticAlgorithm::run()" through a @ref ga::RunOptions object.

Built-in:
- For permutation problems, the default initializer is @ref ga::init::RandomPermutation.

Custom:
- Define a callable that satisfies @ref ga::constraints::InitializationFunction.
- Pass it through the `initializer` field of @ref ga::RunOptions.
- Provide the configured @ref ga::RunOptions object to @ref ga::GeneticAlgorithm::run "ga::GeneticAlgorithm::run()".

### Built-in Initializers

The library includes a default initializer for permutation problems:
- @ref ga::init::RandomPermutation

This built-in initializer creates the provided base sequence and then shuffles it until the requested population size is reached.

### Custom Initializers

Define a callable with the following interface:

@code
std::vector<Sequence> initializer(
    const Sequence& baseSequence,
    std::size_t populationSize,
    ga::Random& rng
);
@endcode

The callable must satisfy @ref ga::constraints::InitializationFunction. Once defined, assign it to the initializer field in @ref ga::RunOptions and pass the options object to @ref ga::GeneticAlgorithm::run "ga::GeneticAlgorithm::run()".
