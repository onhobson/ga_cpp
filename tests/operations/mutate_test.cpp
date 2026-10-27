#include "ga/operations/mutate.hpp"

#include <gtest/gtest.h>

#include <functional>
#include <utility>
#include <vector>

using Sequence = std::vector<int>;
using Mutation = std::function<Sequence(const Sequence&, ga::Random&)>;

TEST(MutateTest, SingleOperationConstructorAppliesMutation)
{
    ga::Mutate<Sequence> mutate {
        [](const Sequence& sequence, ga::Random&) {
            return Sequence { sequence.front(), 24 };
        }
    };
    ga::Random rng { ga::Random::generateMtEngine() };

    EXPECT_EQ(mutate({ 1, 2, 3 }, rng), (Sequence { 1, 24 }));
}

TEST(MutateTest, OperationsAndWeightsConstructorAppliesSelectedMutation)
{
    std::vector<Mutation> operations {
        [](const Sequence&, ga::Random&) { return Sequence { 1 }; },
        [](const Sequence&, ga::Random&) { return Sequence { 2 }; }
    };
    ga::Mutate<Sequence> mutate { std::move(operations), { 0.0, 1.0 } };
    ga::Random rng { ga::Random::generateMtEngine() };

    EXPECT_EQ(mutate({}, rng), (Sequence { 2 }));
}
