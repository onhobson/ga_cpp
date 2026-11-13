#include "ga/ga.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <numeric>
#include <vector>

namespace
{
    using Sequence = std::vector<int>;

    // Returns sum of distances between each element
    auto eval = [](const Sequence& sequence)
    {
        int fitness {};
        
        for (std::size_t j {1}; j < sequence.size(); ++j)
            fitness += std::abs(sequence[j] - sequence[j - 1]);

        return fitness;
    };
}

TEST(EndToEndTest, BasicUsage)
{
    Sequence baseSequence(20);
    std::ranges::iota(baseSequence, 0);

    ga::GeneticAlgorithm<Sequence, int, std::greater<int>> ga {
        eval,
        ga::prebuilt::permutationOperators<Sequence>(),
        ga::Config {}
    };

    ga::Result result { ga.run(baseSequence, ga::RunOptions{}) };

    EXPECT_NE(result.best.sequence, baseSequence);
    EXPECT_GT(result.best.fitness, 0);
}
