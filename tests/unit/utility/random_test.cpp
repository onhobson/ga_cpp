#include "ga/utility/random.hpp"

#include <gtest/gtest.h>

#include <cstddef>
#include <random>
#include <stdexcept>
#include <vector>

#pragma region Core_Functionality

class RandomTest : public testing::Test
{
protected:
    ga::Random rng { ga::Random::generateMtEngine() };
};


TEST_F(RandomTest, GetInteger)
{
    const int value = rng.getInteger(4, 4);
    EXPECT_EQ(value, 4);
}

TEST_F(RandomTest, GetIntegerWithDifferentTypes)
{
    const long longValue = rng.getInteger<long>(-10L, 10L);
    EXPECT_GE(longValue, -10L);
    EXPECT_LE(longValue, 10L);

    const unsigned short shortValue = rng.getInteger<unsigned short>(0, 10);
    EXPECT_GE(shortValue, 0);
    EXPECT_LE(shortValue, 10);

    const std::size_t sizeValue = rng.getInteger<std::size_t>(0, 10);
    EXPECT_GE(sizeValue, 0U);
    EXPECT_LE(sizeValue, 10U);
}

TEST_F(RandomTest, GetIntegerDistribution)
{
    constexpr std::size_t iterations { 10000 };
    constexpr std::size_t min { 0 };
    constexpr std::size_t max { 10 };
    std::vector<std::size_t> counts(max - min + 1, 0);

    for (std::size_t i { 0 }; i < iterations; ++i)
    {
        const std::size_t value = rng.getInteger(min, max);
        EXPECT_GE(value, min);
        EXPECT_LE(value, max);
        ++counts[value - min];
    }

    for (const std::size_t count : counts)
        EXPECT_NEAR(count, iterations / counts.size(), 0.02 * iterations);
}

TEST_F(RandomTest, GetIntegerExclude)
{
    const int value = rng.getIntegerExclude(1, 3, 2);

    EXPECT_GE(value, 1);
    EXPECT_LE(value, 3);
    EXPECT_NE(value, 2);
}

TEST_F(RandomTest, GetIntegerExcludeWithDifferentTypes)
{
    const long longValue = rng.getIntegerExclude(-1L, 1L, 0L);
    EXPECT_GE(longValue, -1L);
    EXPECT_LE(longValue, 1L);
    EXPECT_NE(longValue, 0L);

    const unsigned short unsignedValue = rng.getIntegerExclude(
        static_cast<unsigned short>(1),
        static_cast<unsigned short>(3),
        static_cast<unsigned short>(2)
    );
    EXPECT_GE(unsignedValue, 1);
    EXPECT_LE(unsignedValue, 3);
    EXPECT_NE(unsignedValue, 2);
}

TEST_F(RandomTest, GetIntegerExcludeWithMultipleValues)
{
    const std::vector<int> excluded { 2, 3 };

    for (int i = 0; i < 100; ++i)
    {
        const int value = rng.getIntegerExclude(1, 4, excluded);

        EXPECT_GE(value, 1);
        EXPECT_LE(value, 4);
        EXPECT_NE(value, 2);
        EXPECT_NE(value, 3);
    }
}

TEST_F(RandomTest, GetReal)
{
    const double value = rng.getReal(2.5, 3.5);
    EXPECT_GE(value, 2.5);
    EXPECT_LT(value, 3.5);
}

TEST_F(RandomTest, GetRealWithDifferentTypes)
{
    const float floatValue = rng.getReal(-1.0F, 1.0F);
    EXPECT_GE(floatValue, -1.0F);
    EXPECT_LT(floatValue, 1.0F);

    const long double longDoubleValue = rng.getReal(-1.0L, 1.0L);
    EXPECT_GE(longDoubleValue, -1.0L);
    EXPECT_LT(longDoubleValue, 1.0L);
}

TEST_F(RandomTest, GetIntegerOutputsWithinSuppliedBounds)
{
    for (int i = 0; i < 100; ++i)
    {
        const int value = rng.getInteger(0, 10);
        EXPECT_GE(value, 0);
        EXPECT_LE(value, 10);
    }
}

TEST_F(RandomTest, GetRealOutputsWithinSuppliedBounds)
{
    for (int i = 0; i < 100; ++i)
    {
        const double value = rng.getReal(0.0, 10.0);
        EXPECT_GE(value, 0.0);
        EXPECT_LT(value, 10.0);
    }
}

TEST_F(RandomTest, GetIntegerThrowsWhenMinimumExceedsMaximum)
{
    EXPECT_THROW(rng.getInteger(2, 1), std::invalid_argument);
}

TEST_F(RandomTest, GetIntegerExcludeThrowsForInvalidBoundsOrImpossibleExclusion)
{
    EXPECT_THROW(rng.getIntegerExclude(2, 1, 0), std::invalid_argument);
    EXPECT_THROW(rng.getIntegerExclude(2, 2, 2), std::invalid_argument);
}

TEST_F(RandomTest, GetIntegerExcludeWithMultipleValuesThrowsForInvalidBoundsOrImpossibleExclusion)
{
    EXPECT_THROW(rng.getIntegerExclude(2, 1, std::vector<int> { 0 }), std::invalid_argument);
    EXPECT_THROW(rng.getIntegerExclude(1, 3, std::vector<int> { 1, 2, 3 }), std::invalid_argument);
}

TEST_F(RandomTest, GetRealThrowsWhenMinimumExceedsMaximum)
{
    EXPECT_THROW(rng.getReal(2.0, 1.0), std::invalid_argument);
}

#pragma endregion Core_Functionality



#pragma region Different_Engines

void verifyGetInteger(ga::Random& rng)
{
    const int value = rng.getInteger(0, 10);
    EXPECT_GE(value, 0);
    EXPECT_LE(value, 10);
}

void verifyGetReal(ga::Random& rng)
{
    const double value = rng.getReal(0.0, 1.0);
    EXPECT_GE(value, 0.0);
    EXPECT_LT(value, 1.0);
}

TEST(EngineTest, MtEngine)
{
    ga::Random rng { std::mt19937 { 12345 } };
    verifyGetInteger(rng);
    verifyGetReal(rng);
}

TEST(EngineTest, MinstdRandEngine)
{
    ga::Random rng { std::minstd_rand { 12345 } };
    verifyGetInteger(rng);
    verifyGetReal(rng);
}

class BarebonesEngine
{
public:
    using result_type = unsigned int;

    static constexpr result_type min() { return 0; }
    static constexpr result_type max() { return 255; }

    result_type operator()()
    {
        const result_type result = m_value;
        m_value = (m_value == max()) ? min() : m_value + 1;
        return result;
    }

private:
    result_type m_value { min() };
};

TEST(EngineTest, CustomURBGEngine)
{
    ga::Random rng { BarebonesEngine {} };
    verifyGetInteger(rng);
    verifyGetReal(rng);
}

#pragma endregion Different_Engines
