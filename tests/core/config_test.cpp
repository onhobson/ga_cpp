#include "ga/core/config.hpp"
#include "ga/core/genetic_algorithm.hpp"
#include "ga/operations/functions/bundles.hpp"

#include <gtest/gtest.h>

// Contains shared resources for constructing GeneticAlgorithm object
class ConfigValidationTest : public testing::Test
{
protected:
    static std::function<int(const std::vector<int>&)> evalFunc;
    static ga::Operators<std::vector<int>> operators;
};

std::vector<int> baseSequence { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
std::function<int(const std::vector<int>&)> evalFunc = [](const std::vector<int>& seq) 
{
    return seq.front();
};
ga::Operators<std::vector<int>> operators { ga::prebuilt::permutationOperators<std::vector<int>>() };


TEST(ConfigValidationTest, GenerationCountOfZeroThrows)
{
    ga::Config config {
        .generationCount { 0 }
    };

    EXPECT_THROW(
        (ga::GeneticAlgorithm { evalFunc, operators, config }),
        std::invalid_argument
    );
}

TEST(ConfigValidationTest, ValidGenerationCountDoesNotThrow)
{
    ga::Config config {
        .generationCount { 100 }
    };

    EXPECT_NO_THROW(
        (ga::GeneticAlgorithm { evalFunc, operators, config })
    );
}

TEST(ConfigValidationTest, PopulationSizeOfZeroThrows)
{
    ga::Config config {
        .populationSize { 0 }
    };

    EXPECT_THROW(
        (ga::GeneticAlgorithm { evalFunc, operators, config }),
        std::invalid_argument
    );
}

TEST(ConfigValidationTest, ValidPopulationSizeDoesNotThrow)
{
    ga::Config config {
        .populationSize { 10 },
        .eliteCount { 5 },
        .tournamentSize { 3 }
    };

    EXPECT_NO_THROW(
        (ga::GeneticAlgorithm { evalFunc, operators, config })
    );
    
    config.populationSize = 20;
    config.eliteCount = 20;
    config.tournamentSize = 20;

    EXPECT_NO_THROW(
        (ga::GeneticAlgorithm { evalFunc, operators, config })
    );
}

TEST(ConfigValidationTest, PopulationSizeLessThanEliteCountThrows)
{
    ga::Config config {
        .populationSize { 10 },
        .eliteCount { 11 }
    };

    EXPECT_THROW(
        (ga::GeneticAlgorithm { evalFunc, operators, config }),
        std::invalid_argument
    );
}

TEST(ConfigValidationTest, PopulationSizeLessThanTournamentSizeThrows)
{
    ga::Config config {
        .populationSize { 10 },
        .tournamentSize { 11 }
    };

    EXPECT_THROW(
        (ga::GeneticAlgorithm { evalFunc, operators, config }),
        std::invalid_argument
    );
}

TEST(ConfigValidationTest, TournamentSizeOfZeroThrows)
{
    ga::Config config {
        .tournamentSize { 0 }
    };

    EXPECT_THROW(
        (ga::GeneticAlgorithm { evalFunc, operators, config }),
        std::invalid_argument
    );
}

TEST(ConfigValidationTest, CrossoverRateOutsideOfBoundsThrows)
{
    ga::Config config {
        .crossoverRate { -1.0 }
    };

    EXPECT_THROW(
        (ga::GeneticAlgorithm { evalFunc, operators, config }),
        std::invalid_argument
    );

    config.crossoverRate = 1.01;
    
    EXPECT_THROW(
        (ga::GeneticAlgorithm { evalFunc, operators, config }),
        std::invalid_argument
    );
}

TEST(ConfigValidationTest, CrossoverRateInsideOfBoundsDoesNotThrow)
{
    ga::Config config {
        .crossoverRate { 0.0 }
    };

    EXPECT_NO_THROW(
        (ga::GeneticAlgorithm { evalFunc, operators, config })
    );
    
    config.crossoverRate = 1.0;

    EXPECT_NO_THROW(
        (ga::GeneticAlgorithm { evalFunc, operators, config })
    );
    
    config.crossoverRate = 0.5;

    EXPECT_NO_THROW(
        (ga::GeneticAlgorithm { evalFunc, operators, config })
    );
}
