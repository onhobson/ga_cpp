#include "ga/operations/functions/mutate/misc.hpp"
#include "ga/common/random.hpp"

#include <gtest/gtest.h>

class MutateSwapTest : public testing::Test
{
protected:
    ga::Random rng { ga::Random::generateMtEngine() };

    std::vector<int> sampleSequence { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    std::vector<int> returnSequence { ga::functions::mutate::swap(sampleSequence, rng) };
};

TEST_F(MutateSwapTest, SwapChangesSequence)
{
    EXPECT_NE(sampleSequence, returnSequence);
}

TEST_F(MutateSwapTest, SwapKeepsSameSize)
{
    EXPECT_EQ(sampleSequence.size(), returnSequence.size());
}

TEST_F(MutateSwapTest, SwapKeepsSameElements)
{
    std::sort(sampleSequence.begin(), sampleSequence.end());
    std::sort(returnSequence.begin(), returnSequence.end());

    EXPECT_EQ(sampleSequence, returnSequence);
}

TEST_F(MutateSwapTest, SwapChangesTwoElements)
{
    int diffCount {};
    
    for (std::size_t i {0}; i < sampleSequence.size(); ++i)
    {
        if (sampleSequence[i] != returnSequence[i])
            ++diffCount;
    }

    EXPECT_EQ(diffCount, 2);
}

