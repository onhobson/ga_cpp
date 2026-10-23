#ifndef GA_CROSSOVER_HPP
#define GA_CROSSOVER_HPP

#include "../common/random.hpp"

#include "operator_selector.hpp"

#include <functional>
#include <vector>

namespace ga
{

template <typename Sequence>
class Crossover: public detail::OperatorSelector<std::function<Sequence(const Sequence&, const Sequence&, Random&)>>
{
    using CrossoverOperation = std::function<Sequence(const Sequence&, const Sequence&, Random&)>;

public:
    // Basic single op constructor
    Crossover(CrossoverOperation operation) 
        : Crossover { std::vector<CrossoverOperation> { operation }, std::vector<double> { 1.0 } }
    { }

    // Full op list + weight list constructor
    Crossover(std::vector<CrossoverOperation> operations, std::vector<double> weights)
        : detail::OperatorSelector<CrossoverOperation> { operations, weights }
    { }


    Sequence operator()(const Sequence& seq1, const Sequence& seq2, Random& rng) const 
    {
        CrossoverOperation op { this->getOperation(rng) };
        
        // Return Sequence result
        return op(seq1, seq2, rng);
    }

};

}

#endif
