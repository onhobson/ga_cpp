#include "ga/ga.hpp"

#include <iostream>
#include <vector>


int evaluationFunction(const std::vector<int>& sequence)
{
    int fitness {};
    
    // Fitness = difference between every adjacent sequence element
    for (std::size_t j {1}; j < sequence.size(); ++j)
    {
        fitness += std::abs(sequence[j] - sequence[j - 1]);
    }

    return fitness;
}


int main()
{   
    using Sequence = std::vector<int>;

    ga::Operators<Sequence> ops {
        ga::Crossover<Sequence> {
            ga::functions::Crossover::singlePoint<Sequence>
        },
        ga::Mutate<Sequence> {
            ga::functions::Mutate::swap<Sequence>
        }
    };
    

    ga::Config config {
        10,
        30,
        3,
        0.7
    };

    ga::GeneticAlgorithm<Sequence, int> ga { 
        {81, 83, 14, 64, 33, 10, 27, 51, 61, 23},
        evaluationFunction,
        ops,
        config
    };


    std::cout << "\nRUNNING\n\n";

    ga::Chromosome elite { ga.run() };

    std::cout << "Sequence: ";

    for (auto num : elite.sequence)
        std::cout << num << ' ';
    
    std::cout << "\nFitness: " << elite.fitness << '\n';
}
