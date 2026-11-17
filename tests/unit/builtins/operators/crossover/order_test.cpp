#include "ga/builtins/operators/crossover_permutation.hpp"

#include "ga/utility/random.hpp"

#include "helpers.hpp"

#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>


TEST(OrderCrossoverTest, PreservesParentOneSegmentAndParentTwoOrder)
{
    const std::vector<int> parent1 { 1, 2, 3, 4, 5, 6, 7, 8 };
    const std::vector<int> parent2 { 4, 1, 7, 3, 8, 2, 6, 5 };

    for (std::uint32_t seed { 0 }; seed < 32; ++seed)
    {
        auto cutRng { makeRandom(seed) };
        const std::size_t firstCut { cutRng.getInteger<std::size_t>(0, parent1.size() - 1) };
        const std::size_t secondCut { cutRng.getInteger<std::size_t>(firstCut + 1, parent1.size()) };

        auto rng { makeRandom(seed) };
        const auto child { ga::crossover_op::order(parent1, parent2, rng) };

        ASSERT_EQ(child.size(), parent1.size()) << "seed: " << seed;
        expectPermutationOf(child, parent1);

        // Check parent1 is cut correctly
        for (std::size_t i { firstCut }; i < secondCut; ++i)
            EXPECT_EQ(child[i], parent1[i]) << "seed: " << seed << ", index: " << i;

        std::vector<int> expectedRemaining {};
        std::vector<int> actualRemaining {};

        // Check parent2 is cut correctly
        for (std::size_t offset { 0 }; offset < parent1.size(); ++offset)
        {
            const std::size_t index { (secondCut + offset) % parent1.size() };
            const bool insideCut { index >= firstCut && index < secondCut };

            // Build genes that were cut
            if (!insideCut)
                actualRemaining.push_back(child[index]);

            const int gene { parent2[index] };
            bool retained {};
            for (std::size_t i { firstCut }; i < secondCut; ++i)
                retained = retained || parent1[i] == gene;

            // Build genes that should have been cut by vertex order
            if (!retained)
                expectedRemaining.push_back(gene);
        }

        EXPECT_EQ(actualRemaining, expectedRemaining) << "seed: " << seed;
    }
}

TEST(OrderCrossoverTest, HandlesTwoGenePermutations)
{
    const std::vector<int> parent1 { 1, 2 };
    const std::vector<int> parent2 { 2, 1 };
    auto rng { makeRandom(1) };

    const auto child { ga::crossover_op::order(parent1, parent2, rng) };

    EXPECT_EQ(child, parent1);
}

TEST(OrderCrossoverTest, SupportsStringGenes)
{
    const std::vector<std::string> parent1 { "alpha", "beta", "gamma", "delta" };
    const std::vector<std::string> parent2 { "gamma", "alpha", "delta", "beta" };
    auto rng { makeRandom(3) };

    const auto child { ga::crossover_op::order(parent1, parent2, rng) };

    ASSERT_EQ(child.size(), parent1.size());
    expectPermutationOf(child, parent1);
}

TEST(OrderCrossoverTest, SupportsFixedSizeRanges)
{
    const std::array<int, 4> parent1 { 1, 2, 3, 4 };
    const std::array<int, 4> parent2 { 3, 1, 4, 2 };
    auto rng { makeRandom(4) };

    const auto child { ga::crossover_op::order(parent1, parent2, rng) };

    expectPermutationOf(child, parent1);
}

TEST(OrderCrossoverTest, RejectsInvalidSequenceSizes)
{
    auto rng { makeRandom(1) };
    const std::vector<int> empty {};
    const std::vector<int> oneGene { 1 };
    const std::vector<int> twoGenes { 1, 2 };
    const std::vector<int> threeGenes { 1, 2, 3 };

    EXPECT_THROW(ga::crossover_op::order(empty, empty, rng), std::invalid_argument);
    EXPECT_THROW(ga::crossover_op::order(oneGene, oneGene, rng), std::invalid_argument);
    EXPECT_THROW(ga::crossover_op::order(twoGenes, threeGenes, rng), std::invalid_argument);
}
