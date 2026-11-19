#ifndef GA_OPERATIONS_OPERATOR_SELECTOR_HPP
#define GA_OPERATIONS_OPERATOR_SELECTOR_HPP

#include "ga/utility/random.hpp"

#include <algorithm>
#include <cassert>
#include <format>
#include <random>
#include <vector>


namespace ga::detail
{

/**
 * @brief Selects an operation based on the weighted distribution of the operations.
 * 
 * @tparam Operation The object that is stored and returned based on weights
 * 
 * @throws `std::invalid_argument` If the operation configuration is invalid.
 */
template <typename Operation>
class OperatorSelector
{
protected:
    OperatorSelector() = default;

    /**
     * @brief Construct a new OperatorSelector with given operations, assigned equal weights 
     * 
     * @par operations A vector of operations to be considered by the algorithm
     * 
     * @throws `std::invalid_argument` if `operations.size() != weights.size()`
     * @throws `std::invalid_argument` if `operations.empty()`
     * @throws `std::invalid_argument` if sum of `weights` is `0.0`
     */
    OperatorSelector(
        std::vector<Operation> operations
    ) 
        : OperatorSelector(
            operations,
            std::vector<double> (operations.size(), 1.0)
        )
    { }

    /**
     * @brief Construct a new OperatorSelector object with a vector of operations and a vector of associated weights. 
     * 
     * Operations and weights are paired based on index. 
     * Operations given a weight of 0.0 are omitted from selection
     * 
     * @par operations A vector of operations to be considered by the algorithm
     * @par weights A vector of associated weights
     * 
     * @throws `std::invalid_argument` if `operations.size() != weights.size()`
     * @throws `std::invalid_argument` if `operations.empty()`
     * @throws `std::invalid_argument` if sum of `weights` is `0.0`
     */
    OperatorSelector(
        std::vector<Operation> operations,
        std::vector<double> weights
    ) 
    {
        // Validate inputs that would fail the algorithm
        if (operations.size() != weights.size())
            throw std::invalid_argument("Mismatched operation and weight sizes");
        if (operations.empty())
            throw std::invalid_argument("At least one operation is required when instantiating with a vector");

        double cumWeight { 0.0 };
        
        // Build WeightedOperator array by bundling operations and weights
        for (std::size_t i {0}; i < operations.size(); ++i)
        {
            if (!std::isfinite(weights[i]))
                throw std::invalid_argument("Given weight must be finite");
            if (weights[i] < 0.0)
                throw std::invalid_argument(std::format("Given weight, {}, cannot be negative", weights[i]));

            cumWeight += weights[i];

            m_weightedOperators.push_back(
                WeightedOperator { operations[i], cumWeight }
            );
        }

        if (cumWeight == 0.0)
            throw std::invalid_argument("All supplied weights cannot be 0");
    }

    struct WeightedOperator
    {
        Operation op;
        double weight;
    };

    std::vector<WeightedOperator> m_weightedOperators {};

    /**
     * @brief Selects an operation based on the weighted distribution of the operations.
     * 
     * @param rng The random number generator to use for selection.
     * @return The selected operation.
     */
    Operation getOperation(Random& rng) const
    {
        assert(!m_weightedOperators.empty() && "Operations were not available for selection");
            
        // Select operator based on predefined weights
        double randomNum { rng.get<double>(0.0, m_weightedOperators.back().weight) };

        auto it { std::upper_bound(
            m_weightedOperators.begin(), 
            m_weightedOperators.end(),
            randomNum, 
            [](double weight, const WeightedOperator& wOp) {
                return weight < wOp.weight;
            })
        };
        
        assert(it != m_weightedOperators.end() && "Random weight was not within given operator weight range");

        return it->op;
    }
    
};

}

#endif
