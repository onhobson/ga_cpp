#include "ga/builtins/operators/mutate_permutation.hpp"

#include "ga/utility/random.hpp"

#include "helpers.hpp"

#include <gtest/gtest.h>

#include <ranges>
#include <vector>


namespace
{
    void validateScrambleOperation(const std::vector<int>& child)
    {
        // Assumes parent is sequential ints from 0

        // Contains three sections:
        // Increasing - Scrambled - Increasing (either increasing can be size 0)

        auto it { child.begin() };
        int num { 0 };

        // Find lower bound of scrambled section
        if (*it == num)
        {
            do
            {
                ASSERT_EQ(*it, num) << "lower increasing section";
                ++it;
                ++num;
            } while (it != child.end() && *it == *(it - 1) + 1);            
        }

        ASSERT_NE(it, child.end()) << "child must have scrambled section";

        int scrambleNum { num };
        auto rIt { child.rbegin() };
        num = { static_cast<int>(child.size()) - 1 };

        // Find upper bound of scrambled section
        if (*rIt == num)
        {
            do
            {
                ASSERT_EQ(*rIt, num) << "upper increasing section";
                ++rIt;
                --num;
            } while (rIt != child.rend() && *rIt == *(rIt - 1) - 1);            
        }

        // Assert that scrambled section contains all elements of associated parent section
        std::vector<int> scrambledSection { it, rIt.base() };
        std::ranges::sort(scrambledSection);

        for (int n : scrambledSection)
        {
            ASSERT_EQ(n, scrambleNum) << "scrambled section";
            ++scrambleNum;
        }
    }
}


TEST(ScrambleMutationTest, ReturnsValidPermutation)
{
    std::vector<int> parent(32);
    std::ranges::iota(parent, 0);

    for (std::uint32_t seed { 0 }; seed < 32; ++seed)
    {
        auto rng { makeRandom(seed) };
        
        std::ranges::shuffle(parent, rng.getEngine());
        const auto child { ga::mutate_op::scramble(parent, rng) };
        
        ASSERT_EQ(parent.size(), child.size());
        expectPermutationOf(parent, child);
    }
}

TEST(ScrambleMutationTest, PerformsScrambleCorrectly)
{
    std::vector<int> parent(16);
    std::ranges::iota(parent, 0);

    for (std::uint32_t seed { 0 }; seed < 64; ++seed)
    {
        auto rng { makeRandom(seed) };

        const auto child { ga::mutate_op::scramble(parent, rng) };

        ASSERT_EQ(parent.size(), child.size());
        validateScrambleOperation(child);
    }
}

TEST(ScrambleMutationTest, WorksWithDifferentContainerTypes)
{
    std::array<int, 16> arr;
    std::ranges::iota(arr, 0);
    std::string str { "BIGol'String" };

    outputsPermutation<std::array<int, 16>>(arr, ga::mutate_op::scramble<std::array<int, 16>>);
    outputsPermutation<std::string>(str, ga::mutate_op::scramble<std::string>);
}

TEST(ScrambleMutationTest, InvalidInputThrows)
{
    ga::Random rng { ga::Random::generateMtEngine() };

    std::vector<int> parent {};
    EXPECT_DEBUG_DEATH(
        ga::mutate_op::scramble(parent, rng),
        "Scramble mutation requires at least 2 elements in parent"
    );

    parent.push_back(1);
    EXPECT_DEBUG_DEATH(
        ga::mutate_op::scramble(parent, rng),
        "Scramble mutation requires at least 2 elements in parent"
    );
}

