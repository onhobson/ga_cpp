#include "ga/core/selector.hpp"
#include "ga/core/chromosome.hpp"

#include <gtest/gtest.h>
 

TEST(SelectorTest, SelectsTwoUniqueChromosomesFromPopulation)
{
    ga::Random rng { ga::Random::generateMtEngine() };

    for (std::size_t tournamentSize : { 0, 1, 2, 3 })
    {
        SCOPED_TRACE("tournamentSize=" + std::to_string(tournamentSize));

        ga::detail::Selector<int, int> selector { tournamentSize, rng };

        for (std::size_t populationSize : { 2, 3, 4, 5 })
        {
            SCOPED_TRACE("populationSize=" + std::to_string(populationSize));

            std::vector<ga::Chromosome<int, int>> population;

            for (std::size_t i = 0; i < populationSize; ++i)
                population.emplace_back(ga::Chromosome<int, int> { static_cast<int>(i), static_cast<int>(i) });

            auto [first, second] = selector.select(population);

            EXPECT_NE(&first, &second);
        }
    }
}

TEST(SelectorTest, SelectsTwoSameChromosomesFromSinglePopulation)
{
    ga::Random rng { ga::Random::generateMtEngine() };

    for (std::size_t tournamentSize : { 0, 1, 2, 3 })
    {
        SCOPED_TRACE("tournamentSize=" + std::to_string(tournamentSize));

        ga::detail::Selector<int, int> selector { tournamentSize, rng };

        std::vector<ga::Chromosome<int, int>> population;

        population.emplace_back(ga::Chromosome<int, int> { 0, 0 });

        auto [first, second] = selector.select(population);

        EXPECT_EQ(&first, &second);
    }
}

TEST(SelectorTest, SelectsTwoUniqueChromosomesFromLargePopulation)
{
    ga::Random rng { ga::Random::generateMtEngine() };

    for (std::size_t tournamentSize : { 0, 1, 2, 3 })
    {
        SCOPED_TRACE("tournamentSize=" + std::to_string(tournamentSize));

        ga::detail::Selector<int, int> selector { tournamentSize, rng };

        for (std::size_t populationSize : { 20, 40, 80, 100 })
        {
            SCOPED_TRACE("populationSize=" + std::to_string(populationSize));

            std::vector<ga::Chromosome<int, int>> population;

            for (std::size_t i = 0; i < populationSize; ++i)
                population.emplace_back(ga::Chromosome<int, int> { static_cast<int>(i), static_cast<int>(i) });

            auto [first, second] = selector.select(population);

            EXPECT_NE(&first, &second);
        }
    }
} 

TEST(SelectorTest, SelectSamplesEvenlyFromPopulation)
{
    ga::Random rng { ga::Random::generateMtEngine() };

    ga::detail::Selector<int, int> selector { 1, rng };

    constexpr std::size_t numSelections { 10'000 };
    constexpr std::size_t popSize { 30 };
    constexpr std::size_t expectedCount {( numSelections / popSize) * 2 };

    std::vector<ga::Chromosome<int, int>> population;

    for (std::size_t i { 0 }; i < popSize; ++i)
        population.emplace_back(ga::Chromosome<int, int> { static_cast<int>(i), static_cast<int>(i) });

    std::vector<std::size_t> selectionCounts(population.size(), 0);

    for (std::size_t i { 0 }; i < numSelections; ++i)
    {
        auto [first, second] = selector.select(population);
        selectionCounts[first.fitness]++;
        selectionCounts[second.fitness]++;
    }

    for (std::size_t count : selectionCounts)
    { 
        EXPECT_NEAR(count, expectedCount, 0.01 * numSelections); // Expect roughly uniform distribution
    }
}
