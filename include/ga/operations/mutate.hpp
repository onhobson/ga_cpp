#ifndef GA_MUTATE_HPP
#define GA_MUTATE_HPP

#include "../common/random.hpp"

#include "operator_selector.hpp"

#include <functional>
#include <vector>

namespace ga
{

template <typename Sequence>
class Mutate: public detail::OperatorSelector<std::function<Sequence(const Sequence&, Random&)>>
{
    using MutateOperation = std::function<Sequence(const Sequence&, Random&)>;

public:
    // Basic single op constructor
    Mutate(MutateOperation operation) 
        : Mutate { std::vector<MutateOperation> { operation }, std::vector<double> { 1.0 } }
    { }

    // Full op list + weight list constructor
    Mutate(std::vector<MutateOperation> operations, std::vector<double> weights)
        : detail::OperatorSelector<MutateOperation> { operations, weights }
    { }

    
    Sequence operator()(const Sequence& seq, Random& rng) const 
    {
        MutateOperation op { this->getOperation(rng) };
        
        // Return Sequence result
        return op(seq, rng);
    }

};

}

#endif
