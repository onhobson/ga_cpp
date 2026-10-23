#include "ga/operations/functions/crossover_functions.hpp"
#include "ga/common/random.hpp"

#include <gtest/gtest.h>

#include <vector>


class CrossoverSinglePointTest : public testing::Test
{
protected:
    ga::Random rng { ga::Random::generateMtEngine() };

    std::vector<int> sampleSequence1;
    std::vector<int> sampleSequence2;
    std::vector<int> returnSequence;

    void SetUp() override
    {
        sampleSequence1 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
        sampleSequence2 = { 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
        returnSequence = ga::functions::Crossover::singlePoint(sampleSequence1, sampleSequence2, rng);
    }
};

TEST_F(CrossoverSinglePointTest, SinglePointChangesSequence)
{
    EXPECT_NE(sampleSequence1, returnSequence);
    EXPECT_NE(sampleSequence2, returnSequence);
}

TEST_F(CrossoverSinglePointTest, SinglePointKeepsSameSize)
{
    EXPECT_EQ(sampleSequence1.size(), returnSequence.size());
    EXPECT_EQ(sampleSequence2.size(), returnSequence.size());
}

TEST_F(CrossoverSinglePointTest, SinglePointChangesAtLeastOneElement)
{
    int diffCount {};
    
    for (std::size_t i {0}; i < sampleSequence1.size(); ++i)
    {
        if (sampleSequence1[i] != returnSequence[i])
            ++diffCount;
    }

    EXPECT_GE(diffCount, 1);
}
