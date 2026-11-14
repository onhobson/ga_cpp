#include "ga/builtins/operators/mutate_permutation.hpp"

#include "ga/utility/random.hpp"

#include "helpers.hpp"

#include <gtest/gtest.h>

#include <ranges>
#include <vector>
#include <limits>

namespace
{
    void validateSwapOperation(const std::vector<int> child)
    {
        // Assumes parent is sequentially increasing by 1 from 0

        std::size_t i1 { std::numeric_limits<std::size_t>::max() };
        std::size_t i2 { std::numeric_limits<std::size_t>::max() };

        // Find indexes of two out of place elements
        for (std::size_t i { 0 }; i < child.size(); ++i)
        {
            if (child[i] != static_cast<int>(i))
            {
                if (i1 == std::numeric_limits<std::size_t>::max())
                {
                    i1 = i;
                }
                else
                {
                    ASSERT_EQ(i2, std::numeric_limits<std::size_t>::max()) << "More than 2 elements out of place in child";

                    i2 = i;
                }
            }
        }

        // Values should be equal to other's index (for parent starting at 0)
        ASSERT_EQ(static_cast<int>(i1), child[i2]);
        ASSERT_EQ(static_cast<int>(i2), child[i1]);
    }
}


TEST(SwapMutationTest, ReturnsValidPermutation)
{
    std::vector<int> parent(32);
    std::ranges::iota(parent, 0);

    for (std::uint32_t seed { 0 }; seed < 32; ++seed)
    {
        auto rng { makeRandom(seed) };
        
        std::ranges::shuffle(parent, rng.getEngine());
        const auto child { ga::mutate_op::swap(parent, rng) };
        
        ASSERT_EQ(parent.size(), child.size());
        expectPermutationOf(parent, child);
    }
}

TEST(SwapMutationTest, PerformsSwapCorrectly)
{
    std::vector<int> parent(16);
    std::ranges::iota(parent, 0);

    for (std::uint32_t seed { 0 }; seed < 64; ++seed)
    {
        auto rng { makeRandom(seed) };

        const auto child { ga::mutate_op::swap(parent, rng) };

        ASSERT_EQ(parent.size(), child.size());
        validateSwapOperation(child);
    }
}

TEST(SwapMutationTest, WorksWithDifferentContainerTypes)
{
    std::array<int, 16> arr;
    std::ranges::iota(arr, 0);
    std::string str { "BIGol'String" };

    outputsPermutation<std::array<int, 16>>(arr, ga::mutate_op::swap<std::array<int, 16>>);
    outputsPermutation<std::string>(str, ga::mutate_op::swap<std::string>);
}

TEST(SwapMutationTest, InvalidInputThrows)
{
    ga::Random rng { ga::Random::generateMtEngine() };

    std::vector<int> parent {};
    EXPECT_THROW(ga::mutate_op::swap(parent, rng), std::invalid_argument);

    parent.push_back(1);
    EXPECT_THROW(ga::mutate_op::swap(parent, rng), std::invalid_argument);
}

