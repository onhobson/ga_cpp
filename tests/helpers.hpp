#ifndef GA_TESTS_HELPERS_HPP
#define GA_TESTS_HELPERS_HPP

#include "ga/common/random.hpp"

#include <gtest/gtest.h>

#include <algorithm>


inline ga::Random makeRandom(std::uint32_t seed)
{
    return ga::Random { std::mt19937 { seed } };
}


template <typename Sequence>
inline void expectPermutationOf(const Sequence& actual, Sequence expected)
{
    std::sort(expected.begin(), expected.end());
    Sequence sortedActual { actual };
    std::sort(sortedActual.begin(), sortedActual.end());

    EXPECT_EQ(sortedActual, expected);
}


#endif 
