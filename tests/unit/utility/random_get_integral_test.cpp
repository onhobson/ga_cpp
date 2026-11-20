#include "ga/utility/random.hpp"

#include <gtest/gtest.h>

#include <limits>
#include <utility>
#include <vector>


class RandomGetIntTest : public testing::Test
{
protected:
    ga::Random rng {};
};

TEST_F(RandomGetIntTest, GetIntegralTypes)
{
    const int intValue = rng.get<int>(-10, 10);
    EXPECT_GE(intValue, -10);
    EXPECT_LT(intValue, 10);

    const long longValue = rng.get<long>(-10L, 10L);
    EXPECT_GE(longValue, -10L);
    EXPECT_LT(longValue, 10L);

    const unsigned short shortValue = rng.get<unsigned short>(0, 10);
    EXPECT_GE(shortValue, 0);
    EXPECT_LT(shortValue, 10);

    const std::size_t sizeValue = rng.get<std::size_t>(0, 10);
    EXPECT_GE(sizeValue, 0U);
    EXPECT_LT(sizeValue, 10U);
}

TEST_F(RandomGetIntTest, GetIntegralRanges)
{
    std::vector<std::pair<int, int>> ranges {
        { 0, 10 },
        { 0, 1 },
        { -10, 0 },
        { std::numeric_limits<int>::min(), std::numeric_limits<int>::max() }
    };

    for (auto [from, to] : ranges)
    {
        const int value { rng.get(from, to) };
        EXPECT_GE(value, from);
        EXPECT_LT(value, to);
    }
}

TEST_F(RandomGetIntTest, GetIntegralDistribution)
{
    constexpr std::size_t iterations { 10000 };
    constexpr std::size_t min { 0 };
    constexpr std::size_t max { 10 };
    std::vector<std::size_t> counts(max - min, 0);

    for (std::size_t i { 0 }; i < iterations; ++i)
    {
        const std::size_t value = rng.get(min, max);
        EXPECT_GE(value, min);
        EXPECT_LE(value, max);
        ++counts[value - min];
    }

    for (const std::size_t count : counts)
        EXPECT_NEAR(count, iterations / counts.size(), 0.02 * iterations);
}

TEST_F(RandomGetIntTest, GetIntegralThrowsInvalidRange)
{
    EXPECT_DEBUG_DEATH(
        rng.get(0, 0),
        "Max value must be greater than min value"
    );

    EXPECT_DEBUG_DEATH(
        rng.get(-10, -15),
        "Max value must be greater than min value"
    );

    EXPECT_DEBUG_DEATH(
        rng.get<std::size_t>(10, 10),
        "Max value must be greater than min value"
    );

    EXPECT_DEBUG_DEATH(
        rng.get<std::size_t>(std::numeric_limits<std::size_t>::max(), 0),
        "Max value must be greater than min value"
    );
}
