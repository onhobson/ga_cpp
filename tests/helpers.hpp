#ifndef GA_TESTS_HELPERS_HPP
#define GA_TESTS_HELPERS_HPP

#include "ga/utility/random.hpp"

#include <gtest/gtest.h>

#include <algorithm>


template <typename Sequence>
inline void expectPermutationOf(Sequence actual, Sequence expected)
{
    std::sort(expected.begin(), expected.end());
    std::sort(actual.begin(), actual.end());

    EXPECT_EQ(actual, expected);
}

template <typename Sequence>
inline void outputsPermutation(Sequence& parent, std::function<Sequence(const Sequence&, ga::Random&)> operation, std::uint32_t count = 32)
{
    for (std::uint32_t seed { 0 }; seed < count; ++seed)
    {
        ga::Random rng {};
        std::ranges::shuffle(parent, rng.getEngine());

        const auto child { operation(parent, rng) };
        
        ASSERT_EQ(child.size(), parent.size());
        expectPermutationOf(parent, child);
    }
}

#endif 
