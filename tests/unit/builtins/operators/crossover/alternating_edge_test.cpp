#include "ga/builtins/operators/crossover_permutation.hpp"

#include "ga/utility/random.hpp"

#include "helpers.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <random>
#include <string>
#include <vector>


TEST(AlternatingEdgeTest, ReturnsPermutationOfParentGenes)
{
    const std::vector<int> originalParent1 { 1, 4, 2, 3, 5, 7, 8, 6, 9 };
    const std::vector<int> originalParent2 { 6, 7, 9, 4, 3, 2, 8, 5, 1 };
    ga::Random rng {};

    for (int i { 0 }; i < 32; ++i)
    {
        auto parent1 { originalParent1 };
        auto parent2 { originalParent2 };
        std::shuffle(parent1.begin(), parent1.end(), rng.getEngine());
        std::shuffle(parent2.begin(), parent2.end(), rng.getEngine());

        const auto child { ga::crossover_op::alternatingEdge(parent1, parent2, rng) };

        ASSERT_EQ(child.size(), parent1.size());
        expectPermutationOf(child, parent1);
    }
}

TEST(AlternatingEdgeTest, HandlesOneGenePermutation)
{
    const std::vector<int> parent { 42 };
    ga::Random rng {};

    const auto child { ga::crossover_op::alternatingEdge(parent, parent, rng) };

    EXPECT_EQ(child, parent);
}

TEST(AlternatingEdgeTest, HandlesTwoGenePermutations)
{
    const std::vector<int> parent1 { 1, 2 };
    const std::vector<int> parent2 { 2, 1 };
    ga::Random rng {};

    const auto child { ga::crossover_op::alternatingEdge(parent1, parent2, rng) };

    ASSERT_EQ(child.size(), parent1.size());
    expectPermutationOf(child, parent1);
}

TEST(AlternatingEdgeTest, SupportsStringGenes)
{
    const std::vector<std::string> parent1 { "alpha", "beta", "gamma", "delta" };
    const std::vector<std::string> parent2 { "gamma", "alpha", "delta", "beta" };
    ga::Random rng {};

    const auto child { ga::crossover_op::alternatingEdge(parent1, parent2, rng) };

    ASSERT_EQ(child.size(), parent1.size());
    expectPermutationOf(child, parent1);
}

TEST(AlternatingEdgeTest, SupportsFixedSizeRanges)
{
    const std::array<int, 4> parent1 { 1, 2, 3, 4 };
    const std::array<int, 4> parent2 { 3, 1, 4, 2 };
    ga::Random rng {};

    const auto child { ga::crossover_op::alternatingEdge(parent1, parent2, rng) };

    expectPermutationOf(child, parent1);
}

TEST(AlternatingEdgeTest, ThrowsParentsDiffSize)
{
    const std::vector<int> parent1 { 1, 2, 3, 4, 5 };
    const std::vector<int> parent2 { 1, 2, 3, 4, 5, 6 };
    ga::Random rng {};

    EXPECT_DEBUG_DEATH(
        ga::crossover_op::alternatingEdge(parent1, parent2, rng),
        "Parent sequences must match in size"
    );
}

TEST(AlternatingEdgeTest, ThrowsParentSizeZero)
{
    const std::vector<int> parent {};
    ga::Random rng {};

    EXPECT_DEBUG_DEATH(
        ga::crossover_op::alternatingEdge(parent, parent, rng),
        "Parent cannot be size 0"
    );
}

TEST(AlternatingEdgeTest, ThrowsParentsAreNotPermutations)
{
    const std::vector<int> parent1 { 1, 2, 3, 4, 5, 7 };
    const std::vector<int> parent2 { 1, 2, 3, 4, 5, 6 };
    ga::Random rng {};

    EXPECT_DEBUG_DEATH(
        ga::crossover_op::alternatingEdge(parent1, parent2, rng),
        "Parents must be permutations"
    );
}
