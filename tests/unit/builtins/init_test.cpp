#include "ga/builtins/initializers.hpp"

#include "helpers.hpp"

#include <gtest/gtest.h>

#include <array>
#include <cstddef>
#include <string>
#include <vector>

namespace
{
    template <typename Sequence>
    void expectRandomPermutationPopulation(const Sequence& baseSequence, std::size_t populationSize)
    {
        ga::Random rng {};
        const auto population {
            ga::init::RandomPermutation{}(baseSequence, populationSize, rng)
        };

        ASSERT_EQ(population.size(), populationSize);

        for (std::size_t i { 0 }; i < population.size(); ++i)
        {
            SCOPED_TRACE(i);
            expectPermutationOf(population[i], baseSequence);
        }
    }
}

TEST(RandomPermutationTest, ReturnsRequestedPopulationSizeOfValidPermutations)
{
    const std::vector<int> baseSequence { 4, 1, 7, 2, 9 };

    for (const std::size_t populationSize : { 1, 2, 8, 32 })
        expectRandomPermutationPopulation(baseSequence, populationSize);
}

TEST(RandomPermutationTest, SupportsDifferentPermutableSequenceTypes)
{
    const std::array<int, 5> intArray { 4, 1, 7, 2, 9 };
    const std::string text { "Permutation" };
    const std::vector<std::string> words { "genetic", "algorithm", "permutation" };

    for (const std::size_t populationSize : { 1, 3, 12 })
    {
        expectRandomPermutationPopulation(intArray, populationSize);
        expectRandomPermutationPopulation(text, populationSize);
        expectRandomPermutationPopulation(words, populationSize);
    }
}
