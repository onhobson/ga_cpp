# Configuration Guide {#configuration}

Configuration options are provided to control the algorithm's internal behavior and each individual run.

The library separates algorithm-level configuration from run-level configuration.

#### Fitness Ordering with `Compare`

The @ref ga::GeneticAlgorithm class takes a `Compare` template parameter to define how fitness values are ordered. The comparison is used when ranking the population and selecting tournament winners; it does not change the fitness values returned by the evaluation function.

By default, `Compare` is `std::ranges::less`, so lower fitness values rank ahead of higher ones. This is appropriate for minimization problems. For maximization, provide `std::ranges::greater` so higher fitness values rank first. You can also provide a custom comparison type that satisfies `std::strict_weak_order`.

#### @ref ga::Config "Config"

@ref ga::Config defines the algorithm-wide configuration used when constructing a @ref ga::GeneticAlgorithm. These parameters control the population size, selection behavior, reproduction rate, and callback frequency.

The following fields are available:

- @ref ga::Config::populationSize: Number of individuals in each generation. Must be greater than 1 and greater than the elite count.
- @ref ga::Config::eliteCount: Number of elite individuals preserved from one generation to the next. Must be less than the population size.
- @ref ga::Config::tournamentSize: Number of individuals considered in tournament selection. Must not exceed the population size.
- @ref ga::Config::crossoverRate: Probability of applying crossover to a pair of parents. Must remain in the inclusive range `[0.0, 1.0]`.
- @ref ga::Config::callbackInterval: Interval between callback invocations. A value of `0` disables callbacks.

The configuration is validated when a @ref ga::GeneticAlgorithm is constructed via @ref ga::Config::validate "validate()".

#### @ref ga::RunOptions "RunOptions"

@ref ga::RunOptions defines run-specific behavior for a single invocation of @ref ga::GeneticAlgorithm::run "ga::GeneticAlgorithm::run()". It is used to provide the initializer, termination policy, and callback for that run.

The struct contains:

- @ref ga::RunOptions::initializer: Initial population generator. Defaults to @ref ga::init::RandomPermutation.
- @ref ga::RunOptions::terminator: Termination condition for the current run. Defaults to a @ref ga::terminate::TerminationPolicy with @ref ga::terminate::NoOpTerminator behavior.
- @ref ga::RunOptions::callback: Callback invoked during execution. Defaults to @ref ga::callback::NoOpCallback.

The default run behavior is therefore:
- initialize with @ref ga::init::RandomPermutation
- stop only when the configured generation limit or termination rule triggers
- perform no callback work unless explicitly configured

### Example

@code
ga::Config config {
    .populationSize = 50,
    .eliteCount = 5,
    .tournamentSize = 4,
    .crossoverRate = 0.8,
    .callbackInterval = 10
};

ga::RunOptions<> options {
    .initializer = ga::init::RandomPermutation {},
    .terminator = ga::terminate::TerminationPolicy<>{ 100 },
    .callback = myCallback
};
@endcode

This configuration defines a population of 50 individuals, keeps the top 5 elites, uses tournament selection with a size of 4, applies crossover with an 80% probability, and triggers callback execution every 10 generations.
