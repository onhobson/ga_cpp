#ifndef GA_OPERATIONS_OPERATORS_HPP
#define GA_OPERATIONS_OPERATORS_HPP

#include "ga/operations/crossover.hpp"
#include "ga/operations/mutate.hpp"


namespace ga
{

/**
 * @brief A struct that bundles the crossover and mutation operators for the genetic algorithm.
 * 
 * @tparam Sequence The type that is used by the given crossover and mutation operators.
 */
template <typename Sequence>
struct Operators
{
    Crossover<Sequence> crossover;
    Mutate<Sequence> mutate;
};

}

#endif
