#include "ga/ga.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <random>
#include <string>
#include <vector>

using Sequence = std::vector<int>;
using Chromosome = ga::Chromosome<Sequence, int>;

// Provides default objects to pass into GeneticAlgorithm constructor in each test
// Allows for tests to override certain defaults before instantiating ga::GeneticAlgorithm
class GeneticAlgorithmTest : public testing::Test
{
protected:
    int evaluationCount = 0;
    int crossoverCount = 0;
    int mutationCount = 0;

    ga::Config config {
        .generationCount = 10,
        .populationSize = 10,
        .eliteCount = 2,
        .crossoverRate = 0.0
    };
    ga::Operators<Sequence> operators {
        ga::Crossover<Sequence> {
            [this](const Sequence& first, const Sequence&, ga::Random&)
            {
                ++crossoverCount;
                return first;
            }
        },
        ga::Mutate<Sequence> {
            [this](const Sequence& sequence, ga::Random& rng)
            {
                ++mutationCount;
                return sequence;
            }
        }
    };

    std::vector<int> baseSequence { 1, 2, 3, 4, 5, 6, 7, 8 };

    int evalFunction(const Sequence& seq)
    {
        ++evaluationCount;
        return seq.front();
    }

    struct CountingCallback
    {
        int callbackCount = 0;

        void operator()(const std::vector<Chromosome>&, const Chromosome&)
        {
            ++callbackCount;
        }
    };

    CountingCallback cb {};
};


TEST_F(GeneticAlgorithmTest, RunCallsEvaluationFunctionCorrectAmount)
{
    ga::GeneticAlgorithm<Sequence, int> ga {
        [this](const Sequence& seq) { return evalFunction(seq); },
        operators,
        config
    };

    ga.run(baseSequence);

    // +1 because initial pop is evaluated once before run() loop
    int expectedEvaluationCalls = config.populationSize * (config.generationCount + 1); 

    EXPECT_EQ(evaluationCount, expectedEvaluationCalls); 
}

TEST_F(GeneticAlgorithmTest, RunCallsOperatorFunctionCorrectAmount)
{
    ga::GeneticAlgorithm<Sequence, int> ga {
        [this](const Sequence& seq) { return evalFunction(seq); },
        operators,
        config
    };

    ga.run(baseSequence);

    int expectedOperatorCalls = config.generationCount * (config.populationSize - config.eliteCount);

    EXPECT_EQ(crossoverCount, 0);
    EXPECT_EQ(mutationCount, expectedOperatorCalls);
}

TEST_F(GeneticAlgorithmTest, RunCallsCallbackFunctionCorrectAmount)
{
    ga::GeneticAlgorithm<Sequence, int> ga {
        [this](const Sequence& seq) { return evalFunction(seq); },
        operators,
        config
    };

    ga.run(baseSequence, {}, cb);

    EXPECT_EQ(cb.callbackCount, config.generationCount);
}

TEST_F(GeneticAlgorithmTest, RunReturnsTopChromosome)
{
    // baseSequence is highest fitness possible with evalFunc/mutate operator.
    // Call run() and check that baseSequence kept and returned

    baseSequence = { 10 };

    operators.mutate = {
        [](const Sequence& seq, ga::Random&)
        {
            return Sequence { seq.front() - 1 };
        }
    };
    
    for (const std::size_t eliteCount : { 0, 1 } )
    {
        config.eliteCount = eliteCount;

        ga::GeneticAlgorithm<Sequence, int> ga {
            [this](const Sequence& seq){ return evalFunction(seq); },
            operators,
            config
        };

        Chromosome result = ga.run(baseSequence);

        EXPECT_EQ(result.sequence, baseSequence);
        EXPECT_EQ(result.fitness, evalFunction(baseSequence));
    }
}
