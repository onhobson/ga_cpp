#include "ga/ga.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <numeric>
#include <vector>

namespace
{
    using Sequence = std::vector<int>;

    int evalFunction(const Sequence& sequence)
    {
        int fitness {};
        
        for (std::size_t j {1}; j < sequence.size(); ++j)
            fitness += std::abs(sequence[j] - sequence[j - 1]);

        return fitness;
    }
    
    auto evalLambda = [](const Sequence& sequence)
    {
        return evalFunction(sequence);
    };

    struct EvalStruct
    {
        int operator()(const Sequence& sequence)
        {
            return evalFunction(sequence);
        }
    };

    std::array<int, 10> nMap {
        -8, -6, -4, -2, 0, 2, 4, 6, 8, 10
    };

    struct EvalStructExternal
    {
        int operator()(const Sequence& sequence)
        {
            int fitness { 0 };

            for (std::size_t i { 1 }; i < sequence.size(); ++i)
            {
                fitness += sequence[i] * nMap[sequence[i - 1]];
            }

            return fitness;
        }
    };

    template <typename Eval>
    void validateEvalInput(Eval&& eval)
    {
        Sequence baseSequence(20);
        std::ranges::iota(baseSequence, 0);

        ga::GeneticAlgorithm<Sequence, int> ga {
            std::forward<Eval>(eval),
            ga::prebuilt::permutationOperators<Sequence>(),
            ga::Config {}
        };

        ga::Chromosome elite { ga.run(baseSequence, ga::RunOptions{}) };

        EXPECT_NE(elite.sequence, baseSequence);
        EXPECT_GT(elite.fitness, 0);
    }
}

TEST(EndToEndTest, EvaluationFunction)
{
    validateEvalInput(evalFunction);
}

TEST(EndToEndTest, EvaluationLambda)
{
    validateEvalInput(evalLambda);
}

TEST(EndToEndTest, EvaluationStruct)
{
    validateEvalInput(EvalStruct{});
}

TEST(EndToEndTest, EvaluationWithExternalData)
{
    validateEvalInput(EvalStructExternal{});
}
