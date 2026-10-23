#ifndef GA_OPERATORS_HPP
#define GA_OPERATORS_HPP

#include "crossover.hpp"
#include "mutate.hpp"

#include <vector>
#include <random>

namespace ga
{
    
template <typename Sequence>
struct Operators
{
    Crossover<Sequence> crossover;
    Mutate<Sequence> mutate;
};

}

#endif
