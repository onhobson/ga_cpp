#include "ga/core/chromosome.hpp"
#include "ga/core/evaluator.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <vector>


using Chromosome = ga::Chromosome<int, int>;

class EvaluatorTest : public testing::Test
{
protected:
    int evalCount = 0;
    ga::detail::Evaluator<int, int, std::greater<int>> evaluator;

    std::vector<int> values { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    std::vector<Chromosome> population;

    EvaluatorTest()
        : evaluator {
            [this](int n)
            {
                ++evalCount;
                return n * 2;
            },
            1
        }
    { 
        for (int n : values)
            population.emplace_back( Chromosome { n, 0 } );
    }
};


TEST_F(EvaluatorTest, EvaluateFunctionIsCalledCorrectAmount)
{    
    evaluator.evaluate(population);

    EXPECT_EQ(evalCount, population.size());
}

TEST_F(EvaluatorTest, EvaluateFunctionUpdatesFitnessesCorrectly)
{
    evaluator.evaluate(population);

    for (std::size_t i = 0; i < population.size(); ++i)
        EXPECT_EQ(population[i].fitness, population[i].sequence * 2);
}

TEST_F(EvaluatorTest, ElitesAreKeptInFrontOfPopulation)
{
    auto tmpPop { population };

    for (std::size_t eliteCount : { 0, 1, 3, 6, 7, 9, 10 } )
    {
        SCOPED_TRACE("eliteCount=" + std::to_string(eliteCount));
        population = tmpPop;

        ga::detail::Evaluator<int, int, std::greater<int>> evaluator { 
            [](int n)
            { 
                return n * 2; 
            },
            eliteCount 
        };

        evaluator.evaluate(population);

        if (eliteCount == 0)
            eliteCount = 1;

        for (std::size_t i { 0 }; i < eliteCount; ++i)
        {
            // First indexes should be elites, descending (assumes population is sequential ints from 1)
            const int seq { values.back() - static_cast<int>(i) };

            EXPECT_EQ(population[i].sequence, seq);
            EXPECT_EQ(population[i].fitness, seq * 2);
        }

        // Rest of population should contain non elites
        population.erase(population.begin(), population.begin() + eliteCount);

        std::vector<Chromosome> expected;
        for (int i { 1 }; i <= values.back() - static_cast<int>(eliteCount); ++i)
            expected.emplace_back( Chromosome { i, i * 2 } );

        std::ranges::sort(population, {}, &Chromosome::sequence);

        for (std::size_t i { 0 }; i < expected.size(); ++i)
        {
            EXPECT_EQ(population[i].sequence, expected[i].sequence);
            EXPECT_EQ(population[i].fitness, expected[i].fitness);
        }
    }
}

TEST_F(EvaluatorTest, MoreThanHalfOfThePopulationCanBeElite)
{
    for (std::size_t i { 0 }; i < population.size(); ++i)
        population[i].sequence = static_cast<int>(population.size() - i);

    ga::detail::Evaluator<int, int, std::greater<int>> evaluator { 
        [](int n)
        { 
            return n * 2; 
        },
        7 
    };

    evaluator.evaluate(population);

    for (std::size_t i { 0 }; i < 7; ++i)
    {
        const int expectedSequence { static_cast<int>(values.size() - i) };
        EXPECT_EQ(population[i].sequence, expectedSequence);
        EXPECT_EQ(population[i].fitness, expectedSequence * 2);
    }
}

TEST_F(EvaluatorTest, ThrowsPopulationSizeLessThanEliteCount)
{
    ga::detail::Evaluator<int, int, std::greater<int>> evaluator {
        [](int n) { return n; },
        20
    };

    EXPECT_DEBUG_DEATH(
        evaluator.evaluate(population),
        "Population size must be greater than or equal to elite count"
    );

    while (population.size() < 19)
        population.emplace_back(Chromosome { 2, 0 });

    EXPECT_DEBUG_DEATH(
        evaluator.evaluate(population),
        "Population size must be greater than or equal to elite count"
    );
}
