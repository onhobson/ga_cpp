#include "ga/ga.hpp"

#include <iostream>
#include <vector>

using Sequence = std::vector<int>;

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

    void operator()(const ga::context::CallbackContext<Sequence, int>& context)
    {
        ++callCount;
        // std::cout << callCount << ' ' << population.size() << '\n';
        // topFitProgression.push_back(elite.fitness);
    }
};


int main()
{   

    ga::Operators<Sequence> {
        ga::Crossover<Sequence> { },
        ga::Mutate<Sequence> {
            {
                ga::functions::mutate::swap<Sequence>,
                ga::functions::mutate::inversion<Sequence>
            },
            { 1.0, 1.0 }
        }
    };

    ga::Config config {
        .populationSize{ 10 },
        .eliteCount{ 2 },
        .tournamentSize{ 3 },
        .crossoverRate{ 0.7 },
        .callbackInterval{ 1 }
    };

    std::vector<int> baseSeq {81, 83, 14, 64, 33, 10, 27, 51, 61, 23};

    ga::GeneticAlgorithm<Sequence, int> ga { 
        evaluationFunction,
        ga::prebuilt::permutationOperators<Sequence>(),
        config
    };


    ga::terminate::TerminationPolicy terminate {
        10000,
        ga::terminate::AllOf {
            ga::terminate::GenerationLimit { 1000 },
            ga::terminate::StagnationLimit<int> { 100 }
        }
    };


    std::cout << "\nRUNNING\n";

    Callback cb {};

    ga::RunOptions options {
        .callback = cb
    };

    ga::Chromosome elite { ga.run(baseSeq, options)};

    for (auto n : cb.topFitProgression)
        std::cout << n << ", ";

    std::cout << "\nSequence: ";

    for (auto num : elite.sequence)
        std::cout << num << ' ';
    
    std::cout << "\nFitness: " << elite.fitness << '\n';
}
