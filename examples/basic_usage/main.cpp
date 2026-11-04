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

struct Callback
{
    int callCount {};
    std::vector<int> topFitProgression;

    void operator()(auto population, auto elite)
    {
        ++callCount;
        // std::cout << callCount << ' ' << population.size() << '\n';
        // topFitProgression.push_back(elite.fitness);
    }
};


int main()
{   
    using Sequence = std::vector<int>;

    ga::Config config {
        10,
        30,
        0,
        0.7
    };

    ga::GeneticAlgorithm<Sequence, int> ga { 
        {81, 83, 14, 64, 33, 10, 27, 51, 61, 23},
        evaluationFunction,
        ga::prebuilt::permutationOperators<Sequence>(),
        config
    };


    std::cout << "\nRUNNING\n";

    Callback cb {};
    ga::Chromosome elite { ga.run(cb) };

    for (auto n : cb.topFitProgression)
        std::cout << n << ", ";

    std::cout << "\nSequence: ";

    for (auto num : elite.sequence)
        std::cout << num << ' ';
    
    std::cout << "\nFitness: " << elite.fitness << '\n';
}
