#include "ga/utility/random.hpp"

#include <gtest/gtest.h>

#include <limits>
#include <utility>
#include <vector>


class RandomGetFloatTest : public testing::Test
{
protected:
    ga::Random rng {};
};


TEST_F(RandomGetFloatTest, GetFloatingTypes)
{
    const double doubleValue = rng.get(2.5, 3.5);
    EXPECT_GE(doubleValue, 2.5);
    EXPECT_LT(doubleValue, 3.5);

    const float floatValue = rng.get(-1.0F, 1.0F);
    EXPECT_GE(floatValue, -1.0F);
    EXPECT_LT(floatValue, 1.0F);

    const long double longDoubleValue = rng.get(-1.0L, 1.0L);
    EXPECT_GE(longDoubleValue, -1.0L);
    EXPECT_LT(longDoubleValue, 1.0L);
}

TEST_F(RandomGetFloatTest, GetFloatingRanges)
{
    std::vector<std::pair<double, double>> ranges {
        { 0.0, 1.0 },
        { -10.0, -5.0 },
        { std::numeric_limits<double>::min(), std::numeric_limits<double>::max() }
    };

    for (auto [from, to] : ranges)
    {
        const double value { rng.get(from, to) };
        EXPECT_GE(value, from);
        EXPECT_LT(value, to);
    }
}

TEST_F(RandomGetFloatTest, GetFloatingThrowsInvalidRange)
{
    EXPECT_DEBUG_DEATH(
        rng.get<double>(0.0, 0.0),
        "Max value must be greater than min value"
    );

    EXPECT_DEBUG_DEATH(
        rng.get<double>(-10.0, -15.0),
        "Max value must be greater than min value"
    );

    EXPECT_DEBUG_DEATH(
        rng.get(
            std::numeric_limits<double>::max(),
            std::numeric_limits<double>::min()
        ),
        "Max value must be greater than min value"
    );
}
