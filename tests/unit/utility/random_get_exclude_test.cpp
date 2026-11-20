#include "ga/utility/random.hpp"

#include <gtest/gtest.h>


class RandomGetExcludeTest : public testing::Test
{
protected:
    ga::Random rng {};
};

TEST_F(RandomGetExcludeTest, GetIntegralExcludeSingleValue)
{
    for (int i { 0 }; i < 100; ++i)
    {
        EXPECT_EQ(
            rng.get(0, 2, 1),
            0
        );
        EXPECT_EQ(
            rng.get(-10, -8, -10),
            -9
        );
    }
}

TEST_F(RandomGetExcludeTest, GetIntegralExcludeMultipleValues)
{
    std::vector<int> excluded;

    for (int i { 0 }; i < 1000; ++i)
    {
        excluded.clear();
        while (excluded.size() < 3)
        {
            int val { rng.get(0, 6) };
            if (std::ranges::find(excluded, val) == excluded.end())
                excluded.push_back(val);
        }
        std::ranges::sort(excluded);

        const int randVal = rng.get(0, 6, excluded);

        EXPECT_GE(randVal, 0);
        EXPECT_LT(randVal, 6);
        
        for (int val : excluded)
            EXPECT_NE(randVal, val);
    }
}

TEST_F(RandomGetExcludeTest, GetIntegralExcludeSingleThrowsInvalidRange)
{
    EXPECT_DEBUG_DEATH(
        rng.get(0, 0, 0),
        "Max value must be greater than min value"
    );

    EXPECT_DEBUG_DEATH(
        rng.get(-10, -15, -8),
        "Max value must be greater than min value"
    );
}

TEST_F(RandomGetExcludeTest, GetIntegralExcludeSingleThrowsInvalidExcludeValue)
{
    EXPECT_DEBUG_DEATH(
        rng.get(0, 10, 12),
        "Excluded value not in requested range"
    );
    
    EXPECT_DEBUG_DEATH(
        rng.get(-20, -10, -22),
        "Excluded value not in requested range"
    );
}

TEST_F(RandomGetExcludeTest, GetIntegralExcludeSingleThrowsExcludesAll)
{
    EXPECT_DEBUG_DEATH(
        rng.get(0, 1, 0),
        "Impossible request: Excluded only possible value"
    );
    
    EXPECT_DEBUG_DEATH(
        rng.get(-10, -9, -10),
        "Impossible request: Excluded only possible value"
    );
}

TEST_F(RandomGetExcludeTest, GetIntegralExcludeMultipleThrowsInvalidRange)
{
    EXPECT_DEBUG_DEATH(
        rng.get(0, 0, std::vector<int> { 0, 1 }),
        "Max value must be greater than min value"
    );

    EXPECT_DEBUG_DEATH(
        rng.get(-10, -15, std::vector<int> { -8, -9, -10 }),
        "Max value must be greater than min value"
    );
}

TEST_F(RandomGetExcludeTest, GetIntegralExcludeMultipleThrowsNotSortedAsc)
{
    EXPECT_DEBUG_DEATH(
        rng.get(-10, 0, std::vector<int> { -8, -9, -10 }),
        "Exclude list must be in sorted order"
    );
    
    EXPECT_DEBUG_DEATH(
        rng.get(0, 10, std::vector<int> { 1, 2, 3, 0 }),
        "Exclude list must be in sorted order"
    );
}

TEST_F(RandomGetExcludeTest, GetIntegralExcludeMultipleThrowsDuplicates)
{
    EXPECT_DEBUG_DEATH(
        rng.get(-10, 0, std::vector<int> { -10, -9, -8, -8 }),
        "Exclude list must have no duplicates"
    );
    
    EXPECT_DEBUG_DEATH(
        rng.get(0, 10, std::vector<int> { 0, 0 }),
        "Exclude list must have no duplicates"
    );
}

TEST_F(RandomGetExcludeTest, GetIntegralExcludeMultipleThrowsOutsideRange)
{
    EXPECT_DEBUG_DEATH(
        rng.get(0, 10, std::vector<int> { 0, 1, 2, 10 }),
        "Exclude list must be within"
    );
    
    EXPECT_DEBUG_DEATH(
        rng.get(-10, 0, std::vector<int> { -11 }),
        "Exclude list must be within"
    );
}

TEST_F(RandomGetExcludeTest, GetIntegralExcludeMultipleThrowsExcludesAll)
{
    EXPECT_DEBUG_DEATH(
        rng.get(0, 5, std::vector<int> { 0, 1, 2, 3, 4 }),
        "Impossible request: Exclude list excludes all values in given range"
    );
    
    EXPECT_DEBUG_DEATH(
        rng.get(0, 1, std::vector<int> { 0 }),
        "Impossible request: Exclude list excludes all values in given range"
    );
}
