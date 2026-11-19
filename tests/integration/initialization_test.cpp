#include "ga/ga.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <numeric>
#include <vector>

namespace
{
    using Sequence = std::vector<int>;

    int evalFunction(const Sequence& sequence)
    {
        int fitness {};
        
        for (std::size_t j {1}; j < sequence.size(); ++j)
            fitness += std::abs(sequence[j] - sequence[j - 1]);

        return fitness;
    }

    std::vector<Sequence> initFunction(const Sequence& baseSequence, std::size_t size, ga::Random& rng)
    {
        std::vector<Sequence> population;
        population.reserve(size);

        for (std::size_t i { 0 }; i < size; ++i)
        {
            Sequence newSeq { baseSequence };
            
            std::swap(
                newSeq[ rng.get<std::size_t>(0, newSeq.size()) ],
                newSeq[ rng.get<std::size_t>(0, newSeq.size()) ]
            );
            
            population.push_back(newSeq);
        }

        return population;
    }

    auto initLambda = [](const Sequence& baseSequence, std::size_t size, ga::Random& rng)
    {
        return initFunction(baseSequence, size, rng);
    };

    struct InitStruct
    {
        std::vector<Sequence> operator()(const Sequence& baseSequence, std::size_t size, ga::Random& rng)
        {
            return initFunction(baseSequence, size, rng);
        }
    };

    template <typename Init>
    void validateInitInput(Init&& init)
    {
        Sequence baseSequence(20);
        std::ranges::iota(baseSequence, 0);

        ga::GeneticAlgorithm<Sequence, int, std::greater<int>> ga {
            evalFunction,
            ga::op_set::permutationOperators<Sequence>(),
            ga::Config {}
        };

        ga::Result<Sequence, int> result;

        if constexpr (std::is_same_v<std::remove_reference_t<Init>, std::nullptr_t>)
        {
            ga::RunOptions options {};
            result = ga.run(baseSequence, options);
        }
        else
        {
            ga::RunOptions options {
                .initializer = std::forward<Init>(init)
            };
            result = ga.run(baseSequence, options);
        }

        EXPECT_NE(result.best.sequence, baseSequence);
        EXPECT_GT(result.best.fitness, 0);
    }
}


TEST(EndToEndTest, DefaultInitializer)
{
    validateInitInput(nullptr);
}

TEST(EndToEndTest, PredefinedInitializer)
{
    validateInitInput(ga::init::RandomPermutation{});
}

TEST(EndToEndTest, CustomInitializerFunction)
{
    validateInitInput(initFunction);
}

TEST(EndToEndTest, CustomInitializerLambda)
{
    validateInitInput(initLambda);
}

TEST(EndToEndTest, CustomInitializerStruct)
{
    validateInitInput(InitStruct{});
}
