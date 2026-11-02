#include "ga/common/constraints.hpp"

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