/**
 * Uses att48 TSP dataset from TSPLIB (https://comopt.ifi.uni-heidelberg.de/software/TSPLIB95/index.html).
 *  Minimal tour length is 33,523
 */

#include "load_att48.hpp"

#include <ga/ga.hpp>

#include <iostream>
#include <vector>
#include <ranges>


namespace
{
    using Sequence = std::vector<std::size_t>;

    std::vector<std::vector<int>> distances { Att48::getDistances() };

    // Sum the distances between every adjacent index (circular)
    int evalFunc(const Sequence& seq)
    {
        int fitness = 0;

        for (std::size_t i { 0 }; i < seq.size(); ++i)
            fitness += distances[ seq[i] ][ seq[(i + 1) % seq.size()] ];

        return fitness;
    }

    // Print info every time best fitness improves
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
}

int main()
{
    // Set up GeneticAlgorithm object

    ga::Config config {
        .populationSize = 50,
        .eliteCount = 3,
        .tournamentSize = 3
    };

    ga::GeneticAlgorithm<Sequence, int> ga {
        evalFunc,
        ga::op_set::permutationOperators<Sequence>(),
        config
    };


    // Set up run call

    Sequence base(distances.size());
    std::ranges::iota(base, 0);

    ga::terminate::TerminationPolicy policy {
        5000,
        ga::terminate::StagnationLimit<int>{ 250 }
    };

    PrintCB cb {};

    ga::RunOptions options {
        .terminator = policy,
        .callback = cb
    };

    ga::Result result { ga.run(
        base,
        options
    )};

    
    // Print result

    std::cout << '\n' << result.generation << " generations ran, best result:\n"
              << "Fitness: " << result.best.fitness << "\nSequence: ";

    for (auto n : result.best.sequence)
    {
        std::cout << n << ' ';
    }
    std::cout << '\n';

    return 0;
}
