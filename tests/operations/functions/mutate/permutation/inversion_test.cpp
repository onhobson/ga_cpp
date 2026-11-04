#include "ga/operations/functions/mutate/permutation.hpp"
#include "ga/common/random.hpp"

#include "helpers.hpp"

#include <gtest/gtest.h>

#include <ranges>
#include <vector>


namespace
{
    void validateInversionOperation(const std::vector<int>& child)
    {
        // Assumes parent is sequential increasing from 0

        // Should contain three sections:
        // Increasing -> Decreasing -> Increasing (Either increasing may be truncated)

        auto it { child.begin() };
        int num { 0 };

        // Find point that isn't strictly increasing by 1
        if (*it == num) // Has initial increasing section
        {
            do
            {
                ASSERT_EQ(*it, num);
                ++it;
                ++num;
            } while (it != child.end() && *it == *(it - 1) + 1);
        }

        ASSERT_NE(it, child.end());

        int invNum { *it }; // Save value at start of decreasing section
        num = invNum;
        
        // Find point that isn't strictly decreasing by 1
        do
        {
            ASSERT_EQ(*it, num);
            ++it;
            --num;
        } while (it != child.end() && *it == *(it - 1) - 1);

        // Remainder should be increasing from saved value
        num = invNum + 1;

        while (it != child.end())
        {
            ASSERT_EQ(*it, num);
            ++it;
            ++num;
        }
    }
}


TEST(InversionMutationTest, ReturnsValidPermutation)
{
    std::vector<int> parent(32);
    std::ranges::iota(parent, 0);

    for (std::uint32_t seed { 0 }; seed < 32; ++seed)
    {
        auto rng { makeRandom(seed) };
        
        std::ranges::shuffle(parent, rng.getEngine());
        const auto child { ga::functions::mutate::inversion(parent, rng) };
        
        ASSERT_EQ(parent.size(), child.size());
        expectPermutationOf(parent, child);
    }
}

TEST(InversionMutationTest, PerformsInversionCorrectly)
{
    std::vector<int> parent(16);
    std::ranges::iota(parent, 0);

    for (std::uint32_t seed { 0 }; seed < 64; ++seed)
    {
        auto rng { makeRandom(seed) };

        const auto child { ga::functions::mutate::inversion(parent, rng) };

        ASSERT_EQ(parent.size(), child.size());
        validateInversionOperation(child);
    }
}

TEST(InversionMutationTest, WorksWithDifferentContainerTypes)
{
    std::array<int, 10> arr { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    std::string str { "BIGol'String" };

    outputsPermutation<std::array<int, 10>>(arr, ga::functions::mutate::inversion<std::array<int, 10>>);
    outputsPermutation<std::string>(str, ga::functions::mutate::inversion<std::string>);
}

TEST(InversionMutationTest, InvalidInputThrows)
{
    ga::Random rng { ga::Random::generateMtEngine() };

    std::vector<int> parent {};
    EXPECT_THROW(ga::functions::mutate::inversion(parent, rng), std::invalid_argument);

    parent.push_back(1);
    EXPECT_THROW(ga::functions::mutate::inversion(parent, rng), std::invalid_argument);
}
