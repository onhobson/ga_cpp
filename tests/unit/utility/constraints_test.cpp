#include "ga/utility/constraints.hpp"

#include <gtest/gtest.h>

#include <array>
#include <list>
#include <string>
#include <vector>

TEST(ConstraintsTest, PermutableAcceptsMutableRandomAccessSequences)
{
    EXPECT_TRUE((ga::constraints::Permutable<std::array<int, 3>>));
    EXPECT_TRUE(ga::constraints::Permutable<std::vector<int>>);
    EXPECT_TRUE(ga::constraints::Permutable<std::string>);
}

TEST(ConstraintsTest, PermutableRejectsSequencesThatDoNotMeetRequirements)
{
    EXPECT_FALSE(ga::constraints::Permutable<std::list<int>>);
    EXPECT_FALSE(ga::constraints::Permutable<const std::vector<int>>);
}

TEST(ConstraintsTest, HashableAcceptsEqualityComparableHashableTypes)
{
    EXPECT_TRUE(ga::constraints::Hashable<int>);
    EXPECT_TRUE(ga::constraints::Hashable<std::string>);
}

TEST(ConstraintsTest, HashableRejectsTypesWithoutAHashFunction)
{
    EXPECT_FALSE(ga::constraints::Hashable<std::vector<int>>);
}

TEST(ConstraintsTest, HasValueTypeAcceptsTypesWithNestedValueType)
{
    EXPECT_TRUE((ga::constraints::HasValueType<std::array<int, 3>>));
    EXPECT_TRUE(ga::constraints::HasValueType<std::vector<int>>);
}

TEST(ConstraintsTest, HasValueTypeRejectsTypesWithoutNestedValueType)
{
    EXPECT_FALSE(ga::constraints::HasValueType<int>);
}

TEST(ConstraintsTest, CallbackFunctionAcceptsInvocableCallbacks)
{
    using Sequence = std::vector<int>;
    using Chromosome = ga::Chromosome<Sequence, int>;

    auto validCallback = [](const ga::context::CallbackContext<Sequence, int>&) {};
    EXPECT_TRUE((ga::constraints::CallbackFunction<decltype(validCallback), Sequence, int>));

    auto invalidCallback = [](int) {};
    EXPECT_FALSE((ga::constraints::CallbackFunction<decltype(invalidCallback), Sequence, int>));
}

TEST(ConstraintsTest, InitializationFunctionAcceptsInvocableInitializers)
{
    using Sequence = std::vector<int>;

    auto validInitializer = [](const Sequence&, std::size_t, ga::Random&) { return std::vector<Sequence> { Sequence {} }; };
    EXPECT_TRUE((ga::constraints::InitializationFunction<decltype(validInitializer), Sequence>));

    auto invalidReturnInitializer = [](const Sequence&, std::size_t, ga::Random&) { return Sequence {}; };
    EXPECT_FALSE((ga::constraints::InitializationFunction<decltype(invalidReturnInitializer), Sequence>));

    auto invalidParamInitializer = [](const Sequence&, std::size_t) { return std::vector<Sequence> { Sequence {} }; };
    EXPECT_FALSE((ga::constraints::InitializationFunction<decltype(invalidParamInitializer), Sequence>));
}

TEST(ConstraintsTest, TerminationConditionAcceptsInvocableTerminators)
{
    auto validTerminator = [](const ga::context::TerminationContext<int>&) { return true; };
    EXPECT_TRUE((ga::constraints::TerminationCondition<decltype(validTerminator), int>));
    
    auto invalidReturnTerminator = [](const ga::context::TerminationContext<int>&) { return; };
    EXPECT_FALSE((ga::constraints::TerminationCondition<decltype(invalidReturnTerminator), int>));

    auto invalidParamTerminator = [](int) { return true; };
    EXPECT_FALSE((ga::constraints::TerminationCondition<decltype(invalidParamTerminator), int>));
}

