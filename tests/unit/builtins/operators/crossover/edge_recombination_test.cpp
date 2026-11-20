#include "ga/builtins/operators/crossover_permutation.hpp"

#include "ga/utility/random.hpp"

#include "helpers.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>


namespace
{

// Check the circular neighbors of a gene for one not yet added to the child.
template <typename Sequence>
bool hasUnusedNeighbor(const Sequence& parent, const typename Sequence::value_type& gene,
                       const Sequence& used)
{
    const auto position { std::find(parent.begin(), parent.end(), gene) };
    if (position == parent.end())
        return false;

    const std::size_t index { static_cast<std::size_t>(position - parent.begin()) };
    const std::size_t size { parent.size() };
    const auto& left { parent[(index + size - 1) % size] };
    const auto& right { parent[(index + 1) % size] };

    return std::find(used.begin(), used.end(), left) == used.end() ||
           std::find(used.begin(), used.end(), right) == used.end();
}

// Check whether two genes are adjacent in a parent's circular chromosome.
template <typename Sequence>
bool isParentNeighbor(const Sequence& parent, const typename Sequence::value_type& gene,
                      const typename Sequence::value_type& candidate)
{
    const auto position { std::find(parent.begin(), parent.end(), gene) };
    if (position == parent.end())
        return false;

    const std::size_t index { static_cast<std::size_t>(position - parent.begin()) };
    const std::size_t size { parent.size() };
    return parent[(index + size - 1) % size] == candidate ||
           parent[(index + 1) % size] == candidate;
}

// ERX should use a parent edge whenever the current gene still has an unused one.
template <typename Sequence>
bool usesAvailableParentEdges(const Sequence& child, const Sequence& parent1, const Sequence& parent2)
{
    for (std::size_t i { 0 }; i + 1 < child.size(); ++i)
    {
        const Sequence used { child.begin(), child.begin() + i + 1 };
        const auto& current { child[i] };
        const auto& next { child[i + 1] };
        const bool hasAvailableEdge {
            hasUnusedNeighbor(parent1, current, used) ||
            hasUnusedNeighbor(parent2, current, used)
        };
        const bool isParentEdge {
            isParentNeighbor(parent1, current, next) ||
            isParentNeighbor(parent2, current, next)
        };

        if (hasAvailableEdge && !isParentEdge)
            return false;
    }

    return true;
}

// Detect a step where both parents have no unused neighbor for the current gene.
template <typename Sequence>
bool containsFallbackTransition(const Sequence& child, const Sequence& parent1, const Sequence& parent2)
{
    for (std::size_t i { 0 }; i + 1 < child.size(); ++i)
    {
        const Sequence used { child.begin(), child.begin() + i + 1 };

        if (!hasUnusedNeighbor(parent1, child[i], used) &&
            !hasUnusedNeighbor(parent2, child[i], used))
            return true;
    }

    return false;
}

}


TEST(EdgeRecombinationTest, ReturnsAValidPermutation)
{
    const std::vector<int> originalParent1 { 1, 2, 3, 4, 5, 6, 7, 8 };
    const std::vector<int> originalParent2 { 4, 1, 7, 3, 8, 2, 6, 5 };
    ga::Random rng {};

    // Shuffle valid parents and vary the crossover's random choices across seeds.
    for (int i { 0 }; i < 64; ++i)
    {
        auto parent1 { originalParent1 };
        auto parent2 { originalParent2 };
        std::shuffle(parent1.begin(), parent1.end(), rng.getEngine());
        std::shuffle(parent2.begin(), parent2.end(), rng.getEngine());

        const auto child { ga::crossover_op::edgeRecombination(parent1, parent2, rng) };

        ASSERT_EQ(child.size(), parent1.size());
        EXPECT_EQ(child.front(), parent1.front());
        expectPermutationOf(child, parent1);
    }
}

TEST(EdgeRecombinationTest, SupportsFixedSizeRanges)
{
    const std::array<int, 4> parent1 { 1, 2, 3, 4 };
    const std::array<int, 4> parent2 { 3, 1, 4, 2 };
    ga::Random rng {};

    const auto child { ga::crossover_op::edgeRecombination(parent1, parent2, rng) };

    expectPermutationOf(child, parent1);
}

TEST(EdgeRecombinationTest, PreservesAvailableParentEdges)
{
    const std::vector<int> parent1 { 1, 2, 3, 4, 5, 6, 7 };
    const std::vector<int> parent2 { 1, 4, 7, 3, 6, 2, 5 };
    ga::Random rng {};

    // Every child transition with an available parent edge should retain one.
    for (int i { 0 }; i < 64; ++i)
    {
        const auto child { ga::crossover_op::edgeRecombination(parent1, parent2, rng) };

        EXPECT_TRUE(usesAvailableParentEdges(child, parent1, parent2));
    }
}

TEST(EdgeRecombinationTest, ChoosesUniqueMinimumDegreeWraparoundNeighbor)
{
    const std::vector<int> parent1 { 1, 2, 3, 4, 5, 6 };
    const std::vector<int> parent2 { 1, 3, 5, 6, 2, 4 };
    ga::Random rng {};

    // From gene 1, gene 6 is the unique lowest-degree neighbor via wraparound.
    for (int i { 0 }; i < 32; ++i)
    {
        const auto child { ga::crossover_op::edgeRecombination(parent1, parent2, rng) };

        ASSERT_EQ(child.front(), 1);
        ASSERT_GE(child.size(), 2);
        EXPECT_EQ(child[1], 6);
    }
}

TEST(EdgeRecombinationTest, HandlesTiedMinimumDegreeNeighbors)
{
    const std::vector<int> parent { 1, 2, 3, 4, 5, 6 };
    ga::Random rng {};

    // Identical parents give the starting gene two tied minimum-degree neighbors.
    for (int i { 0 }; i < 32; ++i)
    {
        const auto child { ga::crossover_op::edgeRecombination(parent, parent, rng) };

        ASSERT_EQ(child.front(), parent.front());
        ASSERT_GE(child.size(), 2);
        EXPECT_TRUE(child[1] == 2 || child[1] == 6);
        expectPermutationOf(child, parent);
    }
}

TEST(EdgeRecombinationTest, FallsBackWhenNoUnusedParentNeighborRemains)
{
    const std::vector<int> parent1 { 1, 2, 3, 4, 5, 6, 7, 8 };
    const std::vector<int> parent2 { 1, 3, 2, 4, 7, 8, 6, 5 };
    ga::Random rng {};
    bool fallbackObserved {};

    // Try seeded random tie-breaks until a child reaches the fallback case.
    for (int i { 0 }; i < 128 && !fallbackObserved; ++i)
    {
        const auto child { ga::crossover_op::edgeRecombination(parent1, parent2, rng) };

        fallbackObserved = containsFallbackTransition(child, parent1, parent2);
        EXPECT_EQ(child.size(), parent1.size());
        expectPermutationOf(child, parent1);
    }

    EXPECT_TRUE(fallbackObserved);
}

TEST(EdgeRecombinationTest, HandlesOneGenePermutation)
{
    const std::vector<int> parent { 42 };
    ga::Random rng {};

    const auto child { ga::crossover_op::edgeRecombination(parent, parent, rng) };

    EXPECT_EQ(child, parent);
}

TEST(EdgeRecombinationTest, HandlesTwoGenePermutations)
{
    const std::vector<int> parent1 { 1, 2 };
    const std::vector<int> parent2 { 2, 1 };
    ga::Random rng {};

    const auto child { ga::crossover_op::edgeRecombination(parent1, parent2, rng) };

    EXPECT_EQ(child, parent1);
}

TEST(EdgeRecombinationTest, HandlesIdenticalParents)
{
    const std::vector<int> parent { 1, 2, 3, 4, 5 };
    ga::Random rng {};

    const auto child { ga::crossover_op::edgeRecombination(parent, parent, rng) };

    ASSERT_EQ(child.front(), parent.front());
    ASSERT_EQ(child.size(), parent.size());
    expectPermutationOf(child, parent);
    EXPECT_TRUE(usesAvailableParentEdges(child, parent, parent));
}

TEST(EdgeRecombinationTest, SupportsStringGenes)
{
    const std::vector<std::string> parent1 { "alpha", "beta", "gamma", "delta" };
    const std::vector<std::string> parent2 { "gamma", "alpha", "delta", "beta" };
    ga::Random rng {};

    const auto child { ga::crossover_op::edgeRecombination(parent1, parent2, rng) };

    ASSERT_EQ(child.size(), parent1.size());
    expectPermutationOf(child, parent1);
}

TEST(EdgeRecombinationTest, ThrowsParentsDiffSize)
{
    const std::vector<int> parent1 { 1, 2, 3, 4, 5 };
    const std::vector<int> parent2 { 1, 2, 3, 4, 5, 6 };
    ga::Random rng {};

    EXPECT_DEBUG_DEATH(
        ga::crossover_op::edgeRecombination(parent1, parent2, rng),
        "Parent sequences must match in size"
    );
}

TEST(EdgeRecombinationTest, ThrowsInvalidParentSize)
{
    std::vector<int> parent {};
    ga::Random rng {};

    EXPECT_DEBUG_DEATH(
        ga::crossover_op::edgeRecombination(parent, parent, rng),
        "Parent cannot be size 0"
    );
}

TEST(EdgeRecombinationTest, ThrowsParentHasDuplicates)
{
    const std::vector<int> parent { 1, 2, 3, 4, 5, 6, 2 };
    ga::Random rng {};

    EXPECT_DEBUG_DEATH(
        ga::crossover_op::edgeRecombination(parent, parent, rng),
        "Edge recombination requires no duplicated values in parent sequence"
    );
}

TEST(EdgeRecombinationTest, ThrowsParentsAreNotPermutations)
{
    const std::vector<int> parent1 { 1, 0, 3, 4, 5, 6 };
    const std::vector<int> parent2 { 1, 2, 3, 4, 5, 6 };
    ga::Random rng {};

    EXPECT_DEBUG_DEATH(
        ga::crossover_op::edgeRecombination(parent1, parent2, rng),
        "Parents must be permutations"
    );
}
