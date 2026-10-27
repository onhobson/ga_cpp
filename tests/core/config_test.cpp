#include "ga/core/config.hpp"

#include <gtest/gtest.h>


class ConfigTest : public testing::Test
{
protected:
    ga::Config config { 10, 20, 5, 0.5 };
};


TEST_F(ConfigTest, GenerationCountCanBeSetAndRetrieved)
{
    config.generationCount(25);

    EXPECT_EQ(config.generationCount(), 25);
}

TEST_F(ConfigTest, GenerationCountRejectsZero)
{
    EXPECT_THROW(config.generationCount(0), std::invalid_argument);
}

TEST_F(ConfigTest, PopulationSizeCanBeSetAndRetrieved)
{
    config.populationSize(30);

    EXPECT_EQ(config.populationSize(), 30);
}

TEST_F(ConfigTest, PopulationSizeRejectsZero)
{
    EXPECT_THROW(config.populationSize(0), std::invalid_argument);
}

TEST_F(ConfigTest, PopulationSizeRejectsValuesBelowEliteCount)
{
    EXPECT_THROW(config.populationSize(4), std::invalid_argument);
}

TEST_F(ConfigTest, EliteCountCanBeSetAndRetrieved)
{
    config.eliteCount(10);

    EXPECT_EQ(config.eliteCount(), 10);
}

TEST_F(ConfigTest, EliteCountRejectsValuesAbovePopulationSize)
{
    EXPECT_THROW(config.eliteCount(21), std::invalid_argument);
}

TEST_F(ConfigTest, CrossoverRateCanBeSetAndRetrieved)
{
    config.crossoverRate(0.75);

    EXPECT_DOUBLE_EQ(config.crossoverRate(), 0.75);
}

TEST_F(ConfigTest, CrossoverRateAcceptsBoundaryValues)
{
    config.crossoverRate(0.0);
    EXPECT_DOUBLE_EQ(config.crossoverRate(), 0.0);

    config.crossoverRate(1.0);
    EXPECT_DOUBLE_EQ(config.crossoverRate(), 1.0);
}

TEST_F(ConfigTest, CrossoverRateRejectsValuesOutsideZeroToOne)
{
    EXPECT_THROW(config.crossoverRate(-0.01), std::invalid_argument);
    EXPECT_THROW(config.crossoverRate(1.01), std::invalid_argument);
}
