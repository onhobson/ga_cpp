#ifndef GA_MUTATION_FUNCTIONS_HPP
#define GA_MUTATION_FUNCTIONS_HPP

#include "../../common/random.hpp"

namespace ga::functions::Mutate
{
    // Swap the elements at two random indexes
    template <typename Sequence>
    Sequence swap(const Sequence& seq, Random& rng)
    {
        // Copy to new sequence
        Sequence newSequence { seq };

        std::size_t first { rng.getInteger<std::size_t>(0, seq.size() - 1) };

        std::size_t second { rng.getIntegerExclude<std::size_t>(0, seq.size() - 1, first) };

        std::swap(newSequence[first], newSequence[second]);

        return newSequence;
    }
}

#endif
