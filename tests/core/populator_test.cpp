#include "ga/core/populator.hpp"
#include "ga/operations/crossover.hpp"
#include "ga/operations/mutate.hpp"

#include <gtest/gtest.h>

#include <vector>


using Sequence = std::vector<int>;

class PopulatorTest : public testing::Test
{
protected:
    int crossoverCount = 0;
    ga::Config config { 10, 10, 2, 0.7 };
    ga::Random rng { ga::Random::generateMtEngine() };

    ga::Crossover<Sequence> crossover;
    ga::Mutate<Sequence> mutate;
    ga::detail::Populator<Sequence> populator;


    PopulatorTest()
        : crossover { [this](const Sequence& seq1 , const Sequence& seq2, ga::Random& rng)
            {
                ++crossoverCount;
                return Sequence { seq1.front(), seq2.front() };
            }}
        , mutate { [this](const Sequence& seq, ga::Random& rng)
            {
                return Sequence { 1 };
            }}
        , populator { crossover, mutate, config }
    { }
};


TEST_F(PopulatorTest, CreateRandomizedPopulationReturnsCorrectSize)
{
    Sequence baseSequence { 1, 2, 3, 4, 5 };

    std::vector<Sequence> population { populator.createRandomizedPopulation(baseSequence, rng) };

    EXPECT_EQ(baseSequence.size(), population.front().size());
}

TEST_F(PopulatorTest, CreateRandomizedPopulationReturnsCorrectAmount)
{
    Sequence baseSequence { 1, 2, 3, 4, 5 };

    std::vector<Sequence> population { populator.createRandomizedPopulation(baseSequence, rng) };

    EXPECT_EQ(population.size(), config.populationSize());
}

TEST_F(PopulatorTest, CreateRandomizedPopulationReturnsShuffledSequences)
{
    Sequence baseSequence { 1, 2, 3, 4, 5 };

    std::vector<Sequence> population { populator.createRandomizedPopulation(baseSequence, rng) };

    for (Sequence seq : population)
    {
        std::sort(seq.begin(), seq.end());

        EXPECT_EQ(seq, baseSequence);
    }
}

TEST_F(PopulatorTest, CreateSequenceFollowsCrossoverRate)
{
    Sequence sequence1 { 1, 2, 3, 4, 5 };
    Sequence sequence2 { 6, 7, 8, 9, 10 };

    // crossoverCount = 0;
    // mutateCount = 0;

    constexpr int sampleCount = 100000;
    for (int i = 0; i < sampleCount; ++i)
    {
        populator.createSequence(sequence1, sequence2, rng);
    }

    EXPECT_NEAR(crossoverCount, sampleCount * config.crossoverRate(), sampleCount * 0.05);
}
