#include "ga/ga.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <random>
#include <string>
#include <vector>


using Sequence = std::vector<int>;
using Chromosome = ga::Chromosome<Sequence, int>;

class ConfigEffectsTest : public testing::Test
{
protected:
    ga::Config config {
        .generationCount = 10,
        .populationSize = 10,
        .eliteCount = 2,
        .crossoverRate = 0.7
    };

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

    std::function<int(const Sequence&)> evalFunction = [](const Sequence& seq)
    {
        return seq.front();
    };

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


TEST_F(ConfigEffectsTest, GenerationCountChangesGenerationsRan)
{
    for (std::size_t count : { 5, 8 })
    {
        config.generationCount = count;

        ga::GeneticAlgorithm<Sequence, int> ga {
            evalFunction,
            operators,
            config
        };

        cb.callbackCount = 0;
        ga.run(baseSequence, {}, cb);

        EXPECT_EQ(cb.callbackCount, count);
    }
}

TEST_F(ConfigEffectsTest, PopulationSizeChangesSizeOfPopulation)
{
    for (std::size_t size : { 5, 8 })
    {
        SCOPED_TRACE("populationSize=" + std::to_string(size));

        config.populationSize = size;

        ga::GeneticAlgorithm<Sequence, int> ga {
            evalFunction,
            operators,
            config
        };

        ga.run(
            baseSequence, {},
            [size](const std::vector<Chromosome>& population, const Chromosome&)
            {
                EXPECT_EQ(population.size(), size);
            }
        );
    }
}

TEST_F(ConfigEffectsTest, CallbackIntervalChangesIntervalOfCallback)
{
    for (std::size_t interval : { 0, 1, 5 })
    {
        SCOPED_TRACE("callbackInterval=" + std::to_string(interval));

        config.callbackInterval = interval;

        ga::GeneticAlgorithm ga {
            evalFunction,
            operators,
            config
        };

        cb.callbackCount = 0;
        ga.run(baseSequence, {}, cb);

        if (config.callbackInterval == 0)
            EXPECT_EQ(cb.callbackCount, 0);
        else
            EXPECT_EQ(cb.callbackCount, static_cast<int>(config.generationCount / config.callbackInterval));
    }
}
