#ifndef GA_CORE_RESULT_HPP
#define GA_CORE_RESULT_HPP

#include "ga/core/chromosome.hpp"

#include <cstddef>


namespace ga
{

/**
 * @brief Holds the result of a genetic algorithm run, including the best chromosome found and the number of generations processed.
 * 
 * @tparam Sequence The type of the sequence in the Genetic Algorithm.
 * @tparam Fitness The type of the fitness value in the Genetic Algorithm.
 */
template <typename Sequence, typename Fitness>
struct Result
{
    Chromosome<Sequence, Fitness> best;

    std::size_t generation;
};

}

#endif
