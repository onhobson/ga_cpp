#ifndef GA_COMMON_CONTEXTS_HPP
#define GA_COMMON_CONTEXTS_HPP

#include "ga/core/chromosome.hpp"

#include <vector>

namespace ga::detail
{

template <typename Fitness>
struct TerminationContext
{
    std::size_t generation {};
    Fitness bestFitness {};
};

template <typename Sequence, typename Fitness>
struct CallbackContext
{
    const std::vector<Chromosome<Sequence, Fitness>>& population;
    const Chromosome<Sequence, Fitness> best;

    std::size_t generation;
};

}

#endif
