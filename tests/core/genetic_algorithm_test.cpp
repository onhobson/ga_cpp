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
        baseSequence,
        [this](const Sequence& seq) { return evalFunction(seq); },
        operators,
        config
    };

    ga.run();

    // +1 because initial pop is evaluated once before run() loop
    int expectedEvaluationCalls = config.populationSize * (config.generationCount + 1); 

    EXPECT_EQ(evaluationCount, expectedEvaluationCalls); 
}

TEST_F(GeneticAlgorithmTest, RunCallsOperatorFunctionCorrectAmount)
{
    ga::GeneticAlgorithm<Sequence, int> ga {
        baseSequence,
        [this](const Sequence& seq) { return evalFunction(seq); },
        operators,
        config
    };

    ga.run();

    int expectedOperatorCalls = config.generationCount * (config.populationSize - config.eliteCount);

    EXPECT_EQ(crossoverCount, 0);
    EXPECT_EQ(mutationCount, expectedOperatorCalls);
}

TEST_F(GeneticAlgorithmTest, RunCallsCallbackFunctionCorrectAmount)
{
    ga::GeneticAlgorithm<Sequence, int> ga {
        baseSequence,
        [this](const Sequence& seq) { return evalFunction(seq); },
        operators,
        config
    };

    ga.run(cb);

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
            baseSequence,
            [this](const Sequence& seq){ return evalFunction(seq); },
            operators,
            config
        };

        Chromosome result = ga.run();

        EXPECT_EQ(result.sequence, baseSequence);
        EXPECT_EQ(result.fitness, evalFunction(baseSequence));
    }
}


TEST_F(GeneticAlgorithmTest, CarriesElitesBetweenGenerations)
{
    operators.mutate = ga::Mutate<Sequence> {
        ga::functions::mutate::swap<Sequence>
    };

    // Lehmer code to create unique fitness for each sequence permutation
    auto evalFunc = [](const Sequence& seq)
    {
        Sequence tmp { seq };
        std::ranges::sort(tmp);

        int n = static_cast<int>(seq.size());
        int fitness = 0;

        auto factorial = [](int n)
        {
            int result = 1;
            for (int i = 1; i <= n; ++i)
            {
                result *= i;
            }
            return result;
        };

        for (std::size_t i = 0; i < seq.size(); ++i)
        {
            auto it = std::ranges::find(tmp, seq[i]);
            int rank = it - tmp.begin();

            tmp.erase(it);

            fitness += rank * factorial(n - 1 - i);
        }

        return fitness;
    };

    for (const std::size_t eliteCount : { 1, 3 })
    {
        SCOPED_TRACE("eliteCount=" + std::to_string(eliteCount));
        config.eliteCount = eliteCount;

        // Remember top chromosomes from prev generation
        std::vector<Chromosome> prevTop;

        ga::GeneticAlgorithm<Sequence, int> ga {
            baseSequence,
            evalFunc,
            operators,
            config
        };

        const Chromosome result = ga.run(
            [this, &prevTop, eliteCount] (const std::vector<Chromosome>& population, const Chromosome& elite)
            {
                ASSERT_EQ(population.size(), config.populationSize);

                if (prevTop.empty()) // First iteration
                {
                    // Copy top eliteCount chromosomes to prevTop
                    std::vector<Chromosome> tmpPop { population };
                    std::ranges::sort(tmpPop, std::ranges::greater{}, &Chromosome::fitness);
                    std::ranges::copy_n(tmpPop.begin(), eliteCount, std::back_inserter(prevTop));
                    return;
                }
                
                // Get elites in sorted order, for easy comparison with prevTop
                std::vector<Chromosome> elites;
                std::ranges::copy_n(population.begin(), eliteCount, std::back_inserter(elites));
                std::ranges::sort(elites, std::ranges::greater{}, &Chromosome::fitness);

                // Compare saved elites against prevTop
                ASSERT_EQ(elites.size(), prevTop.size());
                for (std::size_t i = 0; i < eliteCount; ++i)
                {
                    EXPECT_EQ(elites[i].sequence, prevTop[i].sequence) << "elite index: " << i;
                    EXPECT_EQ(elites[i].fitness, prevTop[i].fitness) << "elite index: " << i;
                }

                // Update prevTop
                std::vector<Chromosome> tmpPop { population };
                std::ranges::sort(tmpPop, std::ranges::greater{}, &Chromosome::fitness);
                std::ranges::copy_n(tmpPop.begin(), eliteCount, prevTop.begin());
            }
        );
    }
}

TEST_F(GeneticAlgorithmTest, ZeroEliteCountDoesntCarry)
{
    config.eliteCount = 0;
    baseSequence = { 10 };

    operators.mutate = {
        [](const Sequence& seq, ga::Random&)
        {
            return Sequence { seq.front() - 1 };
        }
    };
    
    ga::GeneticAlgorithm<Sequence, int> ga {
        baseSequence,
        [this](const Sequence& seq){ return evalFunction(seq); },
        operators,
        config
    };

    int count = 0;

    ga.run(
        [this, &count](const std::vector<Chromosome>& population, const Chromosome&)
        {
            ++count;
            EXPECT_EQ(population.front().fitness, baseSequence.front() - count);
        }
    );
}
