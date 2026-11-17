#include <ga/ga.hpp>

#include <cmath>
#include <iostream>
#include <vector>

int main()
{
    // Define the sequence and evaluation function.
    using Sequence = std::vector<int>;

    Sequence base(20);
    std::ranges::iota(base, 0);

    // Score based on the distance between each adjacent value
    auto evalFunc = [](const Sequence& seq)
    {
        int fitness = 0;

        for (std::size_t i = 0; i < seq.size(); ++i)
            fitness += std::abs(seq[i] - seq[(i + 1) % seq.size()]);

        return fitness;
    };

    // Configure the algorithm.
    ga::GeneticAlgorithm<Sequence, int, std::ranges::greater> ga {  // Create a maximization GA with: Sequence = std::vector<int>, Fitness = int
        evalFunc,                                       
        ga::op_set::permutationOperators<Sequence>(),               // Use the built-in permutation operator set
        ga::Config {}                                               // Default config options
    };

    ga::RunOptions options {
        .terminator = { 200 }   // Terminate after 200 generations.
    };

    // Execute the algorithm.
    ga::Result result { ga.run(
        base,
        options
    )};

    // Retrieve the best solution.
    ga::Chromosome best { result.best };


    std::cout << "Best:\n\tFitness: " << best.fitness << "\n\tSequence: ";
    for (auto n : best.sequence)
        std::cout << n << ' ';
    std::cout << '\n';

    return 0;
}
