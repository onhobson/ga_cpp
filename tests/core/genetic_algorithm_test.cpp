#include "ga/ga.hpp"

#include <gtest/gtest.h>

#include <vector>

using Sequence = std::vector<int>;
using Fitness = int;
using Chromosome = ga::Chromosome<Sequence, Fitness>;

class GeneticAlgorithmTest : public testing::Test
{
protected:
    int evalCount = 0;
    int mutateCount = 0;

    Sequence baseSequence { 20, 19, 18, 17, 16 };
    ga::Config config { 10, 10, 2, 0.0 };

    ga::Operators<Sequence> operators {
        ga::Crossover<Sequence>  { [](const Sequence& seq1, const Sequence& seq2, ga::Random& rng)
            {
                return seq1;
            }
        },
        ga::Mutate<Sequence> { [this](const Sequence& seq, ga::Random& rng)
            {
                ++mutateCount;
                
                Sequence newSeq;
                for (auto n : seq)
                    newSeq.push_back(n - 1);
                
                return newSeq;
            }
        }
    };

    Fitness evaluationFunction(const Sequence& seq)
    {
        ++evalCount;
        return seq.front();
    };

    ga::GeneticAlgorithm<Sequence, Fitness> ga;

    GeneticAlgorithmTest()
        : ga {
            baseSequence,
            [this](const Sequence& seq) { return evaluationFunction(seq); },
            operators,
            config
        }
    { }
};


TEST_F(GeneticAlgorithmTest, RunCallsEvaluationFunctionCorrectAmount)
{
    ga.run();

    // +1 because initial pop is evaluated once before run() loop
    int expectedEvalCalls = config.populationSize() * (config.generationCount() + 1); 

    EXPECT_EQ(evalCount, expectedEvalCalls); 
}

TEST_F(GeneticAlgorithmTest, RunCallsOperatorFunctionCorrectAmount)
{
    ga.run();

    int expectedOperatorCalls = config.generationCount() * (config.populationSize() - config.eliteCount());

    EXPECT_EQ(mutateCount, expectedOperatorCalls);
}

TEST_F(GeneticAlgorithmTest, EliteSelectionKeepsFittestChromosome)
{
    Chromosome best { ga.run() };

    EXPECT_EQ(best.fitness, baseSequence.front());
}

