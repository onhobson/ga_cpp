#ifndef GA_MUTATE_MISC_FUNCTIONS_HPP
#define GA_MUTATE_MISC_FUNCTIONS_HPP

#include "ga/common/random.hpp"

namespace ga::functions::mutate
{
    /// Performs a swap mutation on a sequence by randomly selecting two different indices and swapping their values.
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
