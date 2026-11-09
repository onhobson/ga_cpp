#ifndef GA_COMMON_INITIALIZERS_HPP
#define GA_COMMON_INITIALIZERS_HPP

#include "ga/common/constraints.hpp"
#include "ga/common/random.hpp"

namespace ga::init
{


template <typename Sequence>
struct RandomPermutation
{
    std::vector<Sequence> operator()(const Sequence& baseSequence, std::size_t size, Random& rng) const
    {
        std::vector<Sequence> population {};
        population.reserve(size);

        // Add given base sequence
        population.push_back( Sequence {baseSequence} );
        
        // Add random shuffles of sequence until population size is reached
        Sequence sequenceCopy { baseSequence };

        while (population.size() < size)
        {
            std::ranges::shuffle(sequenceCopy, rng.getEngine());

            population.push_back(sequenceCopy);
        }
        
        return population;
    }
};

}

#endif
