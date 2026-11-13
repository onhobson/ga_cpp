#include "ga/common/terminators.hpp"

#include <gtest/gtest.h>

namespace
{

template <typename Fitness>
using Context = ga::context::TerminationContext<Fitness>;

struct ComparableFitness
{
    int value {};

    auto operator<=>(const ComparableFitness&) const = default;
};

TEST(GenerationLimitTest, ReturnsTrueAtOrAboveSpecifiedGeneration)
{
    const ga::terminate::GenerationLimit terminator { 5 };

    EXPECT_FALSE(terminator(Context<int> { 4, 0 }));
    EXPECT_TRUE(terminator(Context<int> { 5, 0 }));
    EXPECT_TRUE(terminator(Context<int> { 6, 0 }));
}

TEST(FitnessThresholdTest, ReturnsTrueAtOrAboveSpecifiedFitness)
{
    const ga::terminate::FitnessThreshold<int> terminator { 10 };

    EXPECT_FALSE(terminator(Context<int> { 0, 9 }));
    EXPECT_TRUE(terminator(Context<int> { 0, 10 }));
    EXPECT_TRUE(terminator(Context<int> { 0, 11 }));
}

TEST(FitnessThresholdTest, SupportsCustomComparableFitnessTypes)
{
    const ga::terminate::FitnessThreshold<ComparableFitness> terminator {
        ComparableFitness { 10 }
    };

    EXPECT_FALSE(terminator(Context<ComparableFitness> { 0, { 9 } }));
    EXPECT_TRUE(terminator(Context<ComparableFitness> { 0, { 10 } }));
    EXPECT_TRUE(terminator(Context<ComparableFitness> { 0, { 11 } }));
}

TEST(StagnationLimitTest, ReturnsTrueAfterSpecifiedCountOfUnchangedFitness)
{
    ga::terminate::StagnationLimit<int> terminator { 2 };

    EXPECT_FALSE(terminator(Context<int> { 0, 10 }));
    EXPECT_FALSE(terminator(Context<int> { 1, 10 }));
    EXPECT_FALSE(terminator(Context<int> { 2, 10 }));
    EXPECT_TRUE(terminator(Context<int> { 3, 10 }));
}

TEST(StagnationLimitTest, CountsImprovementsBelowMarginAsStagnation)
{
    ga::terminate::StagnationLimit<int> terminator { 1, 2 };

    EXPECT_FALSE(terminator(Context<int> { 0, 10 }));
    EXPECT_FALSE(terminator(Context<int> { 1, 11 }));
    EXPECT_TRUE(terminator(Context<int> { 2, 11 }));
}

TEST(StagnationLimitTest, ResetsStagnationCountWhenFitnessImproves)
{
    ga::terminate::StagnationLimit<int> terminator { 2 };

    EXPECT_FALSE(terminator(Context<int> { 0, 10 }));
    EXPECT_FALSE(terminator(Context<int> { 1, 10 }));
    EXPECT_FALSE(terminator(Context<int> { 2, 10 }));
    EXPECT_FALSE(terminator(Context<int> { 3, 11 }));
    EXPECT_FALSE(terminator(Context<int> { 4, 11 }));
    EXPECT_FALSE(terminator(Context<int> { 5, 11 }));
    EXPECT_TRUE(terminator(Context<int> { 6, 11 }));
}

TEST(StagnationLimitTest, ResetsWhenFitnessImprovementMeetsMargin)
{
    ga::terminate::StagnationLimit<int> terminator { 2, 2 };

    EXPECT_FALSE(terminator(Context<int> { 0, 10 }));
    EXPECT_FALSE(terminator(Context<int> { 1, 11 }));
    EXPECT_FALSE(terminator(Context<int> { 2, 12 }));
    EXPECT_FALSE(terminator(Context<int> { 3, 12 }));
    EXPECT_FALSE(terminator(Context<int> { 4, 12 }));
    EXPECT_TRUE(terminator(Context<int> { 5, 12 }));
}

TEST(StagnationLimitTest, CompareLessThanCanBeUsedToTrackDecreasingFitness)
{
    ga::terminate::StagnationLimit<int, std::ranges::less> terminator { 2 };

    EXPECT_FALSE(terminator(Context<int> { 0, 10 }));
    EXPECT_FALSE(terminator(Context<int> { 1, 10 }));
    EXPECT_FALSE(terminator(Context<int> { 2, 10 }));
    EXPECT_TRUE(terminator(Context<int> { 3, 10 }));
}

TEST(StagnationLimitTest, CompareLessThanWithMarginCanBeUsedToTrackDecreasingFitness)
{
    ga::terminate::StagnationLimit<int, std::ranges::less> terminator { 2, 2 };

    EXPECT_FALSE(terminator(Context<int> { 0, 10 }));
    EXPECT_FALSE(terminator(Context<int> { 1, 9 }));
    EXPECT_FALSE(terminator(Context<int> { 2, 8 }));
    EXPECT_FALSE(terminator(Context<int> { 3, 8 }));
    EXPECT_FALSE(terminator(Context<int> { 4, 8 }));
    EXPECT_TRUE(terminator(Context<int> { 5, 8 }));
}

TEST(AnyOfTest, SupportsOneCondition)
{
    ga::terminate::AnyOf terminator {
        ga::terminate::GenerationLimit { 3 }
    };

    EXPECT_FALSE(terminator(Context<int> { 2, 0 }));
    EXPECT_TRUE(terminator(Context<int> { 3, 0 }));
}

TEST(AnyOfTest, SupportsTwoConditions)
{
    ga::terminate::AnyOf terminator {
        ga::terminate::GenerationLimit { 3 },
        ga::terminate::FitnessThreshold<int> { 10 }
    };

    EXPECT_FALSE(terminator(Context<int> { 2, 9 }));
    EXPECT_TRUE(terminator(Context<int> { 3, 9 }));
    EXPECT_TRUE(terminator(Context<int> { 2, 10 }));
    EXPECT_TRUE(terminator(Context<int> { 3, 10 }));
}

TEST(AnyOfTest, SupportsThreeConditions)
{
    ga::terminate::AnyOf terminator {
        ga::terminate::GenerationLimit { 3 },
        ga::terminate::FitnessThreshold<int> { 10 },
        ga::terminate::GenerationLimit { 5 }
    };

    EXPECT_FALSE(terminator(Context<int> { 2, 9 }));
    EXPECT_TRUE(terminator(Context<int> { 3, 9 }));
    EXPECT_TRUE(terminator(Context<int> { 2, 10 }));
    EXPECT_TRUE(terminator(Context<int> { 5, 0 }));
}

TEST(AllOfTest, SupportsOneCondition)
{
    ga::terminate::AllOf terminator {
        ga::terminate::GenerationLimit { 3 }
    };

    EXPECT_FALSE(terminator(Context<int> { 2, 0 }));
    EXPECT_TRUE(terminator(Context<int> { 3, 0 }));
}

TEST(AllOfTest, RequiresBothConditions)
{
    ga::terminate::AllOf terminator {
        ga::terminate::GenerationLimit { 3 },
        ga::terminate::FitnessThreshold<int> { 10 }
    };

    EXPECT_FALSE(terminator(Context<int> { 3, 9 }));
    EXPECT_FALSE(terminator(Context<int> { 2, 10 }));
    EXPECT_TRUE(terminator(Context<int> { 3, 10 }));
}

TEST(AllOfTest, SupportsThreeConditions)
{
    ga::terminate::AllOf terminator {
        ga::terminate::GenerationLimit { 3 },
        ga::terminate::FitnessThreshold<int> { 10 },
        ga::terminate::GenerationLimit { 5 }
    };

    EXPECT_FALSE(terminator(Context<int> { 5, 9 }));
    EXPECT_FALSE(terminator(Context<int> { 3, 10 }));
    EXPECT_TRUE(terminator(Context<int> { 5, 10 }));
}

TEST(TerminatorCompositionTest, AnyOfCanContainAllOfAndAnotherCondition)
{
    ga::terminate::AnyOf terminator {
        ga::terminate::AllOf {
            ga::terminate::GenerationLimit { 3 },
            ga::terminate::FitnessThreshold<int> { 10 }
        },
        ga::terminate::FitnessThreshold<int> { 20 }
    };

    EXPECT_FALSE(terminator(Context<int> { 3, 9 }));
    EXPECT_TRUE(terminator(Context<int> { 3, 10 }));
    EXPECT_TRUE(terminator(Context<int> { 0, 20 }));
}

TEST(TerminatorCompositionTest, AllOfCanContainAnyOf)
{
    ga::terminate::AllOf terminator {
        ga::terminate::AnyOf {
            ga::terminate::GenerationLimit { 3 },
            ga::terminate::FitnessThreshold<int> { 10 }
        },
        ga::terminate::FitnessThreshold<int> { 20 }
    };

    EXPECT_FALSE(terminator(Context<int> { 3, 15 }));
    EXPECT_FALSE(terminator(Context<int> { 2, 15 }));
    EXPECT_TRUE(terminator(Context<int> { 3, 20 }));
}

TEST(TerminationPolicyTest, UsesRequiredGenerationLimit)
{
    ga::terminate::TerminationPolicy policy { 3 };

    EXPECT_FALSE(policy(Context<int> { 2, 100 }));
    EXPECT_TRUE(policy(Context<int> { 3, 0 }));
}

TEST(TerminationPolicyTest, StopsWhenGenerationLimitOrSecondConditionIsMet)
{
    ga::terminate::TerminationPolicy<ga::terminate::FitnessThreshold<int>> policy {
        3,
        ga::terminate::FitnessThreshold<int> { 10 }
    };

    EXPECT_FALSE(policy(Context<int> { 2, 9 }));
    EXPECT_TRUE(policy(Context<int> { 3, 0 }));
    EXPECT_TRUE(policy(Context<int> { 2, 10 }));
    EXPECT_TRUE(policy(Context<int> { 3, 10 }));
}

}
