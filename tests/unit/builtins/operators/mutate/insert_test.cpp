#include "ga/builtins/operators/mutate_permutation.hpp"

#include "ga/utility/random.hpp"

#include "helpers.hpp"

#include <gtest/gtest.h>

#include <ranges>
#include <vector>


namespace
{
    void validateInsertOperation(const std::vector<int>& child)
    {
        // Assumes parent is 1 - 9

        // Find first point in child not following parent seq
        auto it { child.begin() };
        int num { 1 };

        while (it != child.end() && *it == num)
        {
            ASSERT_EQ(*it, num);
            ++it;
            ++num;
        }

        if (it == child.end())
        {
            std::cout << "AH!\n";
            int whoa = 0;
            ++whoa;
        }
        ASSERT_NE(it, child.end());

        const int insertedNum { *it };
        ++it;

        // Rest of child should be parent w/o insertedNum
        while (it != child.end())
        {
            if (num == insertedNum)
            {
                ++num;
                continue;
            }

            ASSERT_EQ(*it, num);     
            
            ++it;
            ++num;
        }
    }
}

TEST(InsertMutationTest, ReturnsValidPermutation)
{
    std::vector<int> parent { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    for (std::uint32_t seed { 0 }; seed < 32; ++seed)
    {
        auto rng { makeRandom(seed) };
        std::ranges::shuffle(parent, rng.getEngine());

        const auto child { ga::mutate_op::insert(parent, rng) };

        ASSERT_EQ(child.size(), parent.size());
        expectPermutationOf(child, parent);
    }
}

TEST(InsertMutationTest, PerformsInsertionCorrectly)
{
    std::vector<int> parent { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    for (std::uint32_t seed { 0 }; seed < 32; ++seed)
    {
        auto rng { makeRandom(seed) };

        const auto child { ga::mutate_op::insert(parent, rng) };

        ASSERT_EQ(child.size(), parent.size());
        validateInsertOperation(child);
    }
}

TEST(InsertMutationTest, WorksWithDifferentContainerTypes)
{
    std::array<int, 10> arr { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    std::string str { "BIGol'String" };

    outputsPermutation<std::array<int, 10>>(arr, ga::mutate_op::insert<std::array<int, 10>>);
    outputsPermutation<std::string>(str, ga::mutate_op::insert<std::string>);
}


TEST(InsertMutationTest, InvalidInputThrows)
{
    ga::Random rng { ga::Random::generateMtEngine() };

    std::vector<int> parent {};
    EXPECT_THROW(ga::mutate_op::insert(parent, rng), std::invalid_argument);

    for (int n : { 1, 2 })
    {
        parent.push_back(n);
        EXPECT_THROW(ga::mutate_op::insert(parent, rng), std::invalid_argument);
    }
}
