#ifndef GA_CROSSOVER_FUNCTIONS_HPP
#define GA_CROSSOVER_FUNCTIONS_HPP

#include "../../common/random.hpp"

#include <vector>

namespace ga::functions::Crossover
{
    template <typename Sequence>
    Sequence singlePoint(const Sequence& seq1, const Sequence& seq2, Random& rng)
    {
        std::size_t crossoverIndex { rng.template getInteger<std::size_t>(1, seq1.size() - 2) };

        // Initialize new chromosome
        Sequence newSequence {};
        newSequence.reserve(seq1.size());

        // Copy seq1 until crossoverIndex, then seq2
        std::copy(seq1.begin(), seq1.begin() + crossoverIndex, std::back_inserter(newSequence));
        std::copy(seq2.begin() + crossoverIndex, seq2.end(), std::back_inserter(newSequence));

        return newSequence;
    }
}

#endif
