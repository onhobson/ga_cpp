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

    ga::Random rng { ga::Random::generateMtEngine() };

    ga::Crossover<Sequence> crossover;
    ga::Mutate<Sequence> mutate;

    Sequence crossoverFunc(const Sequence& seq1, const Sequence& seq2, ga::Random& rng)
    {
        ++crossoverCount;
        return Sequence { seq1.front(), seq2.front() };
    }
    
    Sequence mutateFunc(const Sequence& seq, ga::Random& rng)
    {
        return Sequence { 1 };
    }
};

TEST_F(PopulatorTest, CreateSequenceFollowsCrossoverRate)
{
    Sequence sequence1 { 1, 2, 3, 4, 5 };
    Sequence sequence2 { 6, 7, 8, 9, 10 };

    for (double rate : { 0.0, 0.5, 0.75, 1.0 } )
    {
        crossoverCount = 0;

        ga::detail::Populator<Sequence> populator {
            {[this](const Sequence& seq1, const Sequence& seq2, ga::Random& rng)
            {
                return crossoverFunc(seq1, seq2, rng);
            }},
            {[this](const Sequence& seq, ga::Random& rng)
            {
                return mutateFunc(seq, rng);
            }},
            rate
        };

        constexpr int sampleCount = 100000;
        for (int i = 0; i < sampleCount; ++i)
        {
            populator.createSequence(sequence1, sequence2, rng);
        }

        EXPECT_NEAR(crossoverCount, sampleCount * rate, sampleCount * 0.02);
    }
}
