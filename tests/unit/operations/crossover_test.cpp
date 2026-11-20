#include "ga/operations/crossover.hpp"

#include <gtest/gtest.h>

#include <functional>
#include <utility>
#include <vector>

using Sequence = std::vector<int>;
using Crossover = std::function<Sequence(const Sequence&, const Sequence&, ga::Random&)>;

TEST(CrossoverTest, SingleOperationConstructorAppliesCrossover)
{
    ga::Crossover<Sequence> crossover {
        [](const Sequence& sequence1, const Sequence& sequence2, ga::Random&) {
            return Sequence { sequence1.front(), sequence2.front() };
        }
    };
    ga::Random rng {};

    EXPECT_EQ(
        crossover({ 1, 2, 3 }, { 4, 5, 6 }, rng), 
        (Sequence { 1, 4 })
    );
}

TEST(CrossoverTest, OperationsAndWeightsConstructorAppliesSelectedCrossover)
{
    std::vector<Crossover> operations {
        [](const Sequence&, const Sequence&, ga::Random&) { return Sequence { 1 }; },
        [](const Sequence&, const Sequence&, ga::Random&) { return Sequence { 2 }; }
    };
    ga::Crossover<Sequence> crossover { std::move(operations), { 0.0, 1.0 } };
    ga::Random rng {};

    EXPECT_EQ(crossover({}, {}, rng), (Sequence { 2 }));
}
