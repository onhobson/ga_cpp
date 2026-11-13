#include "ga/ga.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <numeric>
#include <vector>

namespace
{
    using Sequence = int;

    int evalFunction(const Sequence& sequence)
    {
        return sequence;
    }

    ga::Operators<Sequence> operators {
        {},
        {
            [](const Sequence& parent, ga::Random&)
            {
                return Sequence { parent - 1 };
            }
        }
    };

    ga::Config config {
        .populationSize = 2,
        .eliteCount = 0,
        .tournamentSize = 0,
        .crossoverRate = 0.0
    };

    std::vector<Sequence> initFunc(const Sequence& base, std::size_t, ga::Random&)
    {
        return std::vector<Sequence> { base, base };
    }

    struct CountingCallback
    {
        std::size_t count { 0 };
        void operator()(const ga::context::CallbackContext<Sequence, int>&)
        {
            ++count;
        }
    };
    

    template <typename Term>
    void validateTerminatorInput(Term&& term, std::size_t expected = 0)
    {
        Sequence baseSequence { 100 };

        ga::GeneticAlgorithm<Sequence, int> ga {
            evalFunction,
            operators,
            config
        };

        ga::Result<Sequence, int> result;

        if constexpr (std::is_same_v<std::remove_reference_t<Term>, std::nullptr_t>)
        {
            ga::RunOptions options {
                .initializer = initFunc
            };
            result = ga.run(baseSequence, options);
        }
        else
        {
            CountingCallback cb {};

            ga::RunOptions options {
                .initializer = initFunc,
                .terminator = std::forward<Term>(term),
                .callback = cb
            };

            result = ga.run(baseSequence, options);

            EXPECT_EQ(expected, cb.count);
        }

        EXPECT_NE(result.best.sequence, baseSequence);
        EXPECT_LT(result.best.fitness, 100);
    }
}


TEST(EndToEndTest, DefaultTerminator)
{
    validateTerminatorInput(nullptr);
}

TEST(EndToEndTest, CustomTerminatorSimple)
{
    constexpr std::size_t generations { 10 };
    ga::terminate::TerminationPolicy policy { generations };
    validateTerminatorInput(policy, generations);
}

TEST(EndToEndTest, CustomTerminatorMultiLayered)
{
    ga::terminate::TerminationPolicy policy {
        10000,
        ga::terminate::AllOf {
            ga::terminate::FitnessThreshold<int> { 0 },
            ga::terminate::AnyOf {
                ga::terminate::GenerationLimit { 200 },
                ga::terminate::StagnationLimit<int> { 10 }
            }
        }
    };

    validateTerminatorInput(policy, 200);
}
