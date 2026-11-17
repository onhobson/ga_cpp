# Evaluation Guide {#evaluation}

#### What is an Evaluation Function?

An evaluation function assigns a fitness value to a sequence. It is problem-specific, so the library cannot provide one for every optimization problem.

The function is called for every sequence in the population when the initial population is evaluated and again for each generation. Its returned value is stored as the fitness of that sequence in a @ref ga::Chromosome.

#### Defining an Evaluation Function

The function must accept a sequence by const reference and return a fitness value:

@code
Fitness evaluate(const Sequence& sequence);
@endcode

The `Fitness` type must satisfy `std::totally_ordered`, so the algorithm can compare fitness values. The @ref ga::GeneticAlgorithm `Compare` template parameter determines their ordering and which values are considered best; see the @ref configuration "Configuration Guide" for details.

Because evaluation is performed repeatedly for the entire population, keep the function efficient.

#### Using an Evaluation Function

Pass the evaluation function as the first argument to the @ref ga::GeneticAlgorithm constructor:

@code
ga::GeneticAlgorithm<Sequence, Fitness> algorithm {
    evaluate,
    operators,
    config
};
@endcode

The constructor stores the callable by value. Mutable state captured by value is therefore updated in the algorithm's copy, not the original callable. For observing progress or collecting run-time data, use a @ref callbacks "callback" instead.
