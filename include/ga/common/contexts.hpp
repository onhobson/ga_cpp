#ifndef GA_COMMON_CONTEXTS_HPP
#define GA_COMMON_CONTEXTS_HPP

#include "ga/core/chromosome.hpp"

#include <vector>

namespace ga::context
{

/**
 * @brief A struct that holds the context information for the termination condition of the genetic algorithm.
 * 
 * @tparam Fitness The type of the fitness value used in the genetic algorithm.
 * 
 * @param generation The current generation number.
 * @param bestFitness The best fitness value found so far in the genetic algorithm.
 */
template <typename Fitness>
struct TerminationContext
{
    std::size_t generation {};
    Fitness bestFitness {};
};

/**
 * @brief A struct that holds the context information for the callback function of the genetic algorithm.
 * 
 * @tparam Sequence The type of the sequence used in the genetic algorithm.
 * @tparam Fitness The type of the fitness value used in the genetic algorithm.
 * 
 * @param population The current population of chromosomes.
 * @param best The best chromosome found so far in the genetic algorithm.
 * @param generation The current generation number.
 */
template <typename Sequence, typename Fitness>
struct CallbackContext
{
    const std::vector<Chromosome<Sequence, Fitness>>& population;
    const Chromosome<Sequence, Fitness> best;

    std::size_t generation;
};

}

#endif
