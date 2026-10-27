#include "ga/core/chromosome.hpp"
#include "ga/core/evaluator.hpp"

#include <gtest/gtest.h>

#include <vector>


using Chromosome = ga::Chromosome<int, int>;

class EvaluatorTest : public testing::Test
{
protected:
    int evalCount = 0;
    ga::detail::Evaluator<int, int> evaluator;

    std::vector<int> values { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    std::vector<Chromosome> population;

    EvaluatorTest()
        : evaluator { [this](int n) 
            {
                ++evalCount;
                return n * 2;
            }}
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
        EXPECT_EQ(population[i].fitness, values[i] * 2);
}
