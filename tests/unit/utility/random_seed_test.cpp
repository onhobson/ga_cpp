#include "ga/utility/random.hpp"

#include <gtest/gtest.h>

#include <random>

TEST(RandomSeedTest, RandomAcceptsCustomSeeds)
{
    ga::Random rng { 0 };

    EXPECT_EQ(rng.get(0, 1), 0);
}

TEST(RandomSeedTest, RandomAcceptsCustomSeedSeqs)
{
    std::seed_seq ss { 123, 456 };
    ga::Random rng { ss };

    EXPECT_EQ(rng.get(0, 1), 0);
}

TEST(RandomSeedTest, SameRandomSeedOutputsSame)
{
    for (std::mt19937::result_type seed = 0; seed < 32; ++seed)
    {
        ga::Random rng1 { seed };
        ga::Random rng2 { seed };

        for (int i = 0; i < 10; ++i)
        {
            EXPECT_EQ(
                rng1.get(0, 10),
                rng2.get(0, 10)
            );
        }
    }
}

TEST(RandomSeedTest, SameSeedSeqOutputsSame)
{
    for (std::mt19937::result_type seed = 0; seed < 32; ++seed)
    {
        std::seed_seq ss1 { seed };
        std::seed_seq ss2 { seed };

        ga::Random rng1 { ss1 };
        ga::Random rng2 { ss2 };

        for (int i = 0; i < 10; ++i)
        {
            EXPECT_EQ(
                rng1.get(0, 10),
                rng2.get(0, 10)
            );
        }
    }
}

TEST(RandomSeedTest, DifferentSeedsOutputsDifferent)
{
    for (std::mt19937::result_type seed1 = 0; seed1 < 16; ++seed1)
    {
        for (std::mt19937::result_type seed2 = 16; seed2 < 32; ++seed2)
        {
            ga::Random rng1 { seed1 };
            ga::Random rng2 { seed2 };

            bool different { false };

            for (int i = 0; i < 10; ++i)
            {
                if (rng1.get(0, 10) != rng2.get(0, 10))
                {
                    different = true;
                    break;
                }
            }

            EXPECT_TRUE(different) << "Random outputs are the same for different seeds: " << seed1 << " and " << seed2;
        }
    }
}
