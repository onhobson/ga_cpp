#ifndef GA_CROSSOVER_MISC_FUNCTIONS_HPP
#define GA_CROSSOVER_MISC_FUNCTIONS_HPP

#include "ga/common/constraints.hpp"
#include "ga/common/random.hpp"
#include "ga/common/hash.hpp"

#include <algorithm>
#include <vector>
#include <unordered_set>


namespace ga::functions::crossover
{
    /**
     * Performs a single-point crossover between two sequences, producing a new sequence that combines elements from both parents.
     * Not compatible with permutation sequences, as it may produce duplicates or missing elements.
     */
    template <typename Sequence>
    Sequence singlePoint(const Sequence& parent1, const Sequence& parent2, Random& rng)
    {
        std::size_t crossoverIndex { rng.template getInteger<std::size_t>(1, parent1.size() - 2) };

        // Initialize new chromosome
        Sequence child {};

        // Copy parent1 until crossoverIndex, then parent2
        std::copy(parent1.begin(), parent1.begin() + crossoverIndex, std::back_inserter(child));
        std::copy(parent2.begin() + crossoverIndex, parent2.end(), std::back_inserter(child));

        return child;
    }
}

#endif
