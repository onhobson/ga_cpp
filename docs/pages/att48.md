# ATT48 Traveling Salesperson Example {#att48}

This example uses the ATT48 distance matrix to demonstrate a genetic-algorithm heuristic for the traveling salesperson problem (TSP). Each candidate tour visits all 48 cities once and returns to its starting city. The helper in `examples/att48/load_att48.hpp` parses and validates the 48-by-48 distance matrix; the executable is in `examples/att48/att48.cpp`.

The documented minimum tour length for this dataset is 33,523. The algorithm is stochastic, so a run is not guaranteed to find that tour.

## Representing and Evaluating a Tour

The example represents a tour as a sequence of city indices. The distance matrix is indexed by those same values:

@code{.cpp}
using Sequence = std::vector<std::size_t>;

std::vector<std::vector<int>> distances { Att48::getDistances() };
@endcode

The evaluation function sums the distance from each city to the next, including the final leg back to the first city. Since the objective is to minimize total distance, the default minimization for @ref ga::GeneticAlgorithm is suitable.

@code{.cpp}
int evalFunc(const Sequence& sequence)
{
    int fitness = 0;

    for (std::size_t i = 0; i < sequence.size(); ++i)
        fitness += distances[sequence[i]][sequence[(i + 1) % sequence.size()]];

    return fitness;
}
@endcode

## Configuring the Algorithm

The example uses a @ref ga::Config with a population of 50 tours, preserves 3 elites, and uses a tournament size of 3. It supplies the built-in permutation operators through `ga::op_set::permutationOperators()`.

@code{.cpp}
ga::Config config {
    .populationSize = 50,
    .eliteCount = 3,
    .tournamentSize = 3
};

ga::GeneticAlgorithm<Sequence, int> algorithm {
    evalFunc,
    ga::op_set::permutationOperators<Sequence>(),
    config
};
@endcode

The base tour contains each city index from 0 through 47. Passing it to @ref ga::GeneticAlgorithm::run "ga::GeneticAlgorithm::run()" uses the default @ref ga::init::RandomPermutation initializer to create the initial population:

@code{.cpp}
Sequence base(distances.size());
std::ranges::iota(base, 0);
@endcode

## Termination and Progress Reporting

The example stops when either 5,000 generations have been processed or the best fitness has stagnated for 250 generations. A callback reports each improvement it observes:

@code{.cpp}
ga::terminate::TerminationPolicy policy {
    5000,
    ga::terminate::StagnationLimit<int> { 250 }
};

struct PrintCB
{
    int previousBest { std::numeric_limits<int>::max() };

    void operator()(const ga::context::CallbackContext<Sequence, int>& context)
    {
        if (context.best.fitness < previousBest)
        {
            previousBest = context.best.fitness;
            std::cout << "Generation: " << context.generation
                      << "\tFitness: " << previousBest << '\n';
        }
    }
};

PrintCB callback {};
ga::RunOptions options {
    .terminator = policy,
    .callback = callback
};
@endcode

The callback object remains in scope during the run because @ref ga::RunOptions stores it by reference.

## Running and Reading the Result

Run the algorithm with the base tour and options. The returned @ref ga::Result contains the best chromosome found and the number of generations processed:

@code{.cpp}
ga::Result result { algorithm.run(base, options) };

std::cout << result.generation << " generations ran, best result:\n"
          << "Fitness: " << result.best.fitness << "\nSequence: ";

for (auto city : result.best.sequence)
    std::cout << city << ' ';
std::cout << '\n';
@endcode

The sequence in the result is the best tour found by that run, and its fitness is the total distance of its closed route.

