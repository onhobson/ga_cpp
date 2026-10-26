#ifndef GA_MUTATE_HPP
#define GA_MUTATE_HPP

#include "../common/random.hpp"

#include "operator_selector.hpp"

#include <functional>
#include <vector>

namespace ga
{

/**
 * @brief A class that selects and applies a mutation operation to a given sequence based on weighted probabilities.
 * 
 * @tparam Sequence The type that is mutated by the mutation operations.
 * 
 * @par Mutation Operation
 * Each mutation operation must be callable with a `const Sequence&` and `ga::Random&`, and return a `Sequence`.
 * Equivalently, mutation operations must be compatible with `std::function<Sequence(const Sequence&, ga::Random&)>`.
 * 
 * @throws `std::invalid_argument` If the operation configuration is invalid.
 */
template <typename Sequence>
class Mutate: public detail::OperatorSelector<std::function<Sequence(const Sequence&, Random&)>>
{
    using MutateOperation = std::function<Sequence(const Sequence&, Random&)>;

public:
    /**
     * @brief Construct a Mutate with one mutation operation, assigned a weight of 1.0.
     * 
     * @param operation The mutation operation to use.
     */
    Mutate(MutateOperation operation) 
        : detail::OperatorSelector<MutateOperation> { operation }
    { }

    /**
     * @brief Construct a new Mutate object with a vector of mutation operations and a vector of associated weights.
     * 
     * @param operations Candidate mutation operations, in the same order as `weights`.
     * @param weights Relative weights corresponding to `operations`.
     */
    Mutate(std::vector<MutateOperation> operations, std::vector<double> weights)
        : detail::OperatorSelector<MutateOperation> { operations, weights }
    { }

    /**
     * @brief Applies a randomly selected mutation operation using the given `Sequence`.
     * 
     * @param seq The parent `Sequence`.
     * @param rng The `ga::Random` random number generator to use for selecting the mutation operation.
     * @see ga::Random
     * 
     * @return The `Sequence` produced by the mutation operation. 
     */
    Sequence operator()(const Sequence& seq, Random& rng) const 
    {
        MutateOperation op { this->getOperation(rng) };
        
        // Return Sequence result
        return op(seq, rng);
    }

};

}

#endif
