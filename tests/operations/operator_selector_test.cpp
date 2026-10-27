#include "ga/common/random.hpp"
#include "ga/operations/operator_selector.hpp"

#include <gtest/gtest.h>

#include <vector>

// Create derived class for testing
class TestOperatorSelector : public ga::detail::OperatorSelector<int>
{
    using Base = ga::detail::OperatorSelector<int>;
public:
    explicit TestOperatorSelector(int operation)
        : Base { operation }
    { }

    TestOperatorSelector(
        std::vector<int> operations,
        std::vector<double> weights
    )
        : Base { std::move(operations), std::move(weights) }
    { }

    int select(ga::Random& rng)
    {
        return getOperation(rng);
    }
};


TEST(OperatorSelectorTest, SingleOperationIsAlwaysSelected)
{
    TestOperatorSelector selector { 24 };
    ga::Random rng { ga::Random::generateMtEngine() };

    for (int i = 0; i < 100; ++i)
        EXPECT_EQ(selector.select(rng), 24);
}

TEST(OperatorSelectorTest, WeightedSelectionFollowsRelativeWeights)
{
    TestOperatorSelector selector {
        { 1, 2, 3 },
        { 0.0, 1.0, 3.0 }
    };
    ga::Random rng { ga::Random::generateMtEngine() };
    int operation2Count {};
    int operation3Count {};
    constexpr int sampleCount { 10000 };

    for (int i = 0; i < sampleCount; ++i)
    {
        const int operation { selector.select(rng) };
        EXPECT_NE(operation, 1);

        if (operation == 2)
            ++operation2Count;
        else if (operation == 3)
            ++operation3Count;
        else
            FAIL() << "Unexpected operation selected: " << operation;
    }

    EXPECT_NEAR(operation2Count, sampleCount * 0.25, sampleCount * 0.05);
    EXPECT_NEAR(operation3Count, sampleCount * 0.75, sampleCount * 0.05);
}


TEST(OperatorSelectorTest, ThrowsWhenOperationAndWeightCountsDiffer)
{
    EXPECT_THROW(
        (TestOperatorSelector { { 1, 2 }, { 1.0 } }),
        std::invalid_argument
    );
}

TEST(OperatorSelectorTest, ThrowsWhenNoOperationsAreProvided)
{
    EXPECT_THROW(
        (TestOperatorSelector { {}, {} }),
        std::invalid_argument
    );
}

TEST(OperatorSelectorTest, ThrowsWhenWeightIsNegative)
{
    EXPECT_THROW(
        (TestOperatorSelector { { 1, 2 }, { 1.0, -1.0 } }),
        std::invalid_argument
    );
}
