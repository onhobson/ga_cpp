#include "ga/ga.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <random>
#include <string>
#include <vector>


using Sequence = std::vector<int>;
using Chromosome = ga::Chromosome<Sequence, int>;

class GeneticAlgorithmConfigTest : public testing::Test
{
protected:
    ga::Config config { 10, 10, 2, 0.0 };
    ga::Operators<Sequence> operators {
        ga::Crossover<Sequence> {
            [](const Sequence& first, const Sequence&, ga::Random&)
            {
                return first;
            }
        },
        ga::Mutate<Sequence> {
            [](const Sequence& sequence, ga::Random& rng)
            {
                return sequence;
            }
        }
    };

    std::vector<int> baseSequence { 1, 2, 3, 4, 5, 6, 7, 8 };

    int evalFunction(const Sequence& seq)
    {
        return seq.front();
    }

    struct CountingCallback
    {
        int callbackCount = 0;

        void operator()(const std::vector<Chromosome>&, const Chromosome&)
        {
            ++callbackCount;
        }
    };

    CountingCallback cb {};
};


TEST_F(GeneticAlgorithmConfigTest, GenerationCountChangesGenerationsRan)
{
    for (std::size_t generationCount : { 5, 8 })
    {
        config.generationCount(generationCount);

        ga::GeneticAlgorithm<Sequence, int> ga {
            baseSequence,
            [this](const Sequence& seq) { return evalFunction(seq); },
            operators,
            config
        };

        cb.callbackCount = 0;
        ga.run(cb);

        EXPECT_EQ(cb.callbackCount, generationCount);
    }
}

TEST_F(GeneticAlgorithmConfigTest, PopulationSizeChangesSizeOfPopulation)
{
    for (std::size_t populationSize : { 5, 8 })
    {
        SCOPED_TRACE("populationSize=" + std::to_string(populationSize));

        config.populationSize(populationSize);

        ga::GeneticAlgorithm<Sequence, int> ga {
            baseSequence,
            [this](const Sequence& seq) { return evalFunction(seq); },
            operators,
            config
        };

        ga.run(
            [populationSize](const std::vector<Chromosome>& population, const Chromosome&)
            {
                EXPECT_EQ(population.size(), populationSize);
            }
        );
    }
}
