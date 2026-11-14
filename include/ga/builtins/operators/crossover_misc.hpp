#ifndef GA_BUILTINS_OPERATORS_CROSSOVER_MISC_HPP
#define GA_BUILTINS_OPERATORS_CROSSOVER_MISC_HPP

#include "ga/utility/random.hpp"

#include <algorithm>
#include <unordered_set>
#include <vector>


namespace ga::crossover_op
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
