#ifndef GA_COMMON_INITIALIZERS_HPP
#define GA_COMMON_INITIALIZERS_HPP

#include "ga/common/constraints.hpp"
#include "ga/common/random.hpp"

namespace ga::init
{


struct RandomPermutation
{
    /**
     * @brief Generates a population of random permutations of the given base sequence.
     * 
     * @tparam Sequence The type of the sequence to be permuted. Must satisfy `std::permutable<Sequence>`.
     * 
     * @param baseSequence The base sequence to be permuted.
     * @param size The number of permutations to generate.
     * @param rng A random number generator to use for shuffling the sequence.
     * 
     * @return A vector containing `size` random permutations of the `baseSequence`.
     */
    template <typename Sequence>
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
