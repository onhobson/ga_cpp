#ifndef GA_OPERATOR_SELECTOR_HPP
#define GA_OPERATOR_SELECTOR_HPP

#include "../common/random.hpp"

#include <algorithm>
#include <format>
#include <vector>
#include <random>

namespace ga::detail
{
    
template <typename Operation>
class OperatorSelector
{
protected:
    // Basic single op constructor
    OperatorSelector(
        Operation operation
    ) : OperatorSelector(
        std::vector<Operation> { operation },
        std::vector<double> { 1.0 }
    ) { }

    // Full op list + weight list constructor
    OperatorSelector(
        std::vector<Operation> operations,
        std::vector<double> weights
    ) 
    {
        // Validate inputs that would fail the algorithm
        if (operations.size() != weights.size())
            throw std::invalid_argument("Mismatched operation and weight sizes");
        if (operations.empty())
            throw std::invalid_argument("At least one operation is required");

        double cumWeight { 0.0 };
        
        // Build WeightedOperator array by bundling operations and weights
        for (std::size_t i {0}; i < operations.size(); ++i)
        {
            if (weights[i] < 0)
                throw std::invalid_argument(std::format("Given weight, {}, cannot be negative", weights[i]));

            cumWeight += weights[i];

            m_weightedOperators.push_back(
                WeightedOperator { operations[i], cumWeight }
            );
        }
    }

    struct WeightedOperator
    {
        Operation op;
        double weight;
    };

    std::vector<WeightedOperator> m_weightedOperators {};


    Operation getOperation(Random& rng) const
    {
        // Select operator based on predefined weights
        double randomNum { rng.getReal(0.0, m_weightedOperators.back().weight) };

        auto it { std::upper_bound(
            m_weightedOperators.begin(), 
            m_weightedOperators.end(),
            randomNum, 
            [](double weight, const WeightedOperator& wOp) {
                return weight < wOp.weight;
            })
        };
        
        if (it == m_weightedOperators.end())
            throw std::invalid_argument("Random weight was not within given operator weight range");

        return it->op;
    }
    
};

}

#endif
