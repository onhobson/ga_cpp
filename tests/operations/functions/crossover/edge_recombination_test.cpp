#include "ga/operations/functions/crossover/permutation.hpp"
#include "ga/common/random.hpp"

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

    // Shuffle valid parents and vary the crossover's random choices across seeds.
    for (std::uint32_t seed { 0 }; seed < 64; ++seed)
    {
        auto rng { makeRandom(seed) };
        auto parent1 { originalParent1 };
        auto parent2 { originalParent2 };
        std::shuffle(parent1.begin(), parent1.end(), rng.getEngine());
        std::shuffle(parent2.begin(), parent2.end(), rng.getEngine());

        const auto child { ga::functions::crossover::edgeRecombination(parent1, parent2, rng) };

        ASSERT_EQ(child.size(), parent1.size()) << "seed: " << seed;
        EXPECT_EQ(child.front(), parent1.front()) << "seed: " << seed;
        expectPermutationOf(child, parent1);
    }
}

TEST(EdgeRecombinationTest, SupportsFixedSizeRanges)
{
    const std::array<int, 4> parent1 { 1, 2, 3, 4 };
    const std::array<int, 4> parent2 { 3, 1, 4, 2 };
    auto rng { makeRandom(4) };

    const auto child { ga::functions::crossover::edgeRecombination(parent1, parent2, rng) };

    expectPermutationOf(child, parent1);
}

TEST(EdgeRecombinationTest, PreservesAvailableParentEdges)
{
    const std::vector<int> parent1 { 1, 2, 3, 4, 5, 6, 7 };
    const std::vector<int> parent2 { 1, 4, 7, 3, 6, 2, 5 };

    // Every child transition with an available parent edge should retain one.
    for (std::uint32_t seed { 0 }; seed < 64; ++seed)
    {
        auto rng { makeRandom(seed) };
        const auto child { ga::functions::crossover::edgeRecombination(parent1, parent2, rng) };

        EXPECT_TRUE(usesAvailableParentEdges(child, parent1, parent2)) << "seed: " << seed;
    }
}

TEST(EdgeRecombinationTest, ChoosesUniqueMinimumDegreeWraparoundNeighbor)
{
    const std::vector<int> parent1 { 1, 2, 3, 4, 5, 6 };
    const std::vector<int> parent2 { 1, 3, 5, 6, 2, 4 };

    // From gene 1, gene 6 is the unique lowest-degree neighbor via wraparound.
    for (std::uint32_t seed { 0 }; seed < 32; ++seed)
    {
        auto rng { makeRandom(seed) };
        const auto child { ga::functions::crossover::edgeRecombination(parent1, parent2, rng) };

        ASSERT_EQ(child.front(), 1);
        ASSERT_GE(child.size(), 2);
        EXPECT_EQ(child[1], 6) << "seed: " << seed;
    }
}

TEST(EdgeRecombinationTest, HandlesTiedMinimumDegreeNeighbors)
{
    const std::vector<int> parent { 1, 2, 3, 4, 5, 6 };

    // Identical parents give the starting gene two tied minimum-degree neighbors.
    for (std::uint32_t seed { 0 }; seed < 64; ++seed)
    {
        auto rng { makeRandom(seed) };
        const auto child { ga::functions::crossover::edgeRecombination(parent, parent, rng) };

        ASSERT_EQ(child.front(), parent.front());
        ASSERT_GE(child.size(), 2);
        EXPECT_TRUE(child[1] == 2 || child[1] == 6) << "seed: " << seed;
        expectPermutationOf(child, parent);
    }
}

TEST(EdgeRecombinationTest, FallsBackWhenNoUnusedParentNeighborRemains)
{
    const std::vector<int> parent1 { 1, 2, 3, 4, 5, 6, 7, 8 };
    const std::vector<int> parent2 { 1, 3, 2, 4, 7, 8, 6, 5 };
    bool fallbackObserved {};

    // Try seeded random tie-breaks until a child reaches the fallback case.
    for (std::uint32_t seed { 0 }; seed < 128 && !fallbackObserved; ++seed)
    {
        auto rng { makeRandom(seed) };
        const auto child { ga::functions::crossover::edgeRecombination(parent1, parent2, rng) };

        fallbackObserved = containsFallbackTransition(child, parent1, parent2);
        EXPECT_EQ(child.size(), parent1.size()) << "seed: " << seed;
        expectPermutationOf(child, parent1);
    }

    EXPECT_TRUE(fallbackObserved);
}

TEST(EdgeRecombinationTest, HandlesOneGenePermutation)
{
    const std::vector<int> parent { 42 };
    auto rng { makeRandom(1) };

    const auto child { ga::functions::crossover::edgeRecombination(parent, parent, rng) };

    EXPECT_EQ(child, parent);
}

TEST(EdgeRecombinationTest, HandlesTwoGenePermutations)
{
    const std::vector<int> parent1 { 1, 2 };
    const std::vector<int> parent2 { 2, 1 };
    auto rng { makeRandom(2) };

    const auto child { ga::functions::crossover::edgeRecombination(parent1, parent2, rng) };

    EXPECT_EQ(child, parent1);
}

TEST(EdgeRecombinationTest, HandlesIdenticalParents)
{
    const std::vector<int> parent { 1, 2, 3, 4, 5 };
    auto rng { makeRandom(3) };

    const auto child { ga::functions::crossover::edgeRecombination(parent, parent, rng) };

    ASSERT_EQ(child.front(), parent.front());
    ASSERT_EQ(child.size(), parent.size());
    expectPermutationOf(child, parent);
    EXPECT_TRUE(usesAvailableParentEdges(child, parent, parent));
}

TEST(EdgeRecombinationTest, SupportsStringGenes)
{
    const std::vector<std::string> parent1 { "alpha", "beta", "gamma", "delta" };
    const std::vector<std::string> parent2 { "gamma", "alpha", "delta", "beta" };
    auto rng { makeRandom(4) };

    const auto child { ga::functions::crossover::edgeRecombination(parent1, parent2, rng) };

    ASSERT_EQ(child.size(), parent1.size());
    expectPermutationOf(child, parent1);
}

TEST(EdgeRecombinationTest, RejectsInvalidSequenceSizes)
{
    auto rng { makeRandom(5) };
    const std::vector<int> empty {};
    const std::vector<int> twoGenes { 1, 2 };
    const std::vector<int> threeGenes { 1, 2, 3 };

    EXPECT_THROW(ga::functions::crossover::edgeRecombination(empty, empty, rng), std::invalid_argument);
    EXPECT_THROW(ga::functions::crossover::edgeRecombination(twoGenes, threeGenes, rng), std::invalid_argument);
}

TEST(EdgeRecombinationTest, RejectsParentsWithDuplicateGenes)
{
    auto rng { makeRandom(6) };
    const std::vector<int> valid { 1, 2, 3 };
    const std::vector<int> duplicateFirst { 1, 1, 3 };
    const std::vector<int> duplicateSecond { 1, 2, 2 };

    EXPECT_THROW(ga::functions::crossover::edgeRecombination(duplicateFirst, valid, rng), std::invalid_argument);
    EXPECT_THROW(ga::functions::crossover::edgeRecombination(valid, duplicateSecond, rng), std::invalid_argument);
}

TEST(EdgeRecombinationTest, RejectsParentsWithDifferentGeneSets)
{
    const std::vector<int> parent1 { 1, 2, 3, 4 };
    const std::vector<int> parent2 { 1, 2, 3, 5 };
    auto rng { makeRandom(7) };

    EXPECT_THROW(ga::functions::crossover::edgeRecombination(parent1, parent2, rng), std::invalid_argument);
}
