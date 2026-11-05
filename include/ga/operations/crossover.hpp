#ifndef GA_CROSSOVER_HPP
#define GA_CROSSOVER_HPP

#include "ga/common/random.hpp"

#include "ga/operations/operator_selector.hpp"

#include <functional>
#include <vector>

namespace ga
{

/**
 * @brief A class that selects and applies a crossover operation to a given sequence based on weighted probabilities.
 * 
 * @tparam Sequence The type that is crossed over by the crossover operations.
 * 
 * @par Crossover Operation
 * Each crossover operation must be callable with two `const Sequence&` and a `ga::Random&`, and return a `Sequence`.
 * Equivalently, crossover operations must be compatible with `std::function<Sequence(const Sequence&, const Sequence&, ga::Random&)>`.
 * 
 * @throws `std::invalid_argument` If the operation configuration is invalid.
 */
template <typename Sequence>
class Crossover: public detail::OperatorSelector<std::function<Sequence(const Sequence&, const Sequence&, Random&)>>
{
    using CrossoverOperation = std::function<Sequence(const Sequence&, const Sequence&, Random&)>;

public:
    Crossover() = default;

    /**
     * @brief Constructs a Crossover with one crossover operation, assigned a weight of 1.0.
     * 
     * @param operation The crossover operation to use.
     */
    Crossover(CrossoverOperation operation) 
        : Crossover { std::vector<CrossoverOperation> { operation }, std::vector<double> { 1.0 } }
    { }

    /**
     * @brief Construct a Crossover with a vector of crossover operations. Assigns equal weights to the operators.
     * 
     * @param operations Candidate crossover operations.
     */
    Crossover(std::vector<CrossoverOperation> operations)
        : detail::OperatorSelector<CrossoverOperation> { operations }
    { }

    /**
     * @brief Construct a Crossover with a vector of crossover operations and a vector of associated weights.
     * 
     * @param operations Candidate crossover operations, in the same order as `weights`.
     * @param weights Relative weights corresponding to `operations`.
     */
    Crossover(std::vector<CrossoverOperation> operations, std::vector<double> weights)
        : detail::OperatorSelector<CrossoverOperation> { operations, weights }
    { }

    /**
     * @brief Applies a randomly selected crossover operation using the 2 given `Sequence`s.
     * 
     * @param seq1 The first parent `Sequence`.
     * @param seq2 The second parent `Sequence`.
     * @param rng The `ga::Random` random number generator to use for selecting the crossover operation.
     * @see ga::Random
     * 
     * @return The `Sequence` produced by the selected crossover operation.
     */
    Sequence operator()(const Sequence& seq1, const Sequence& seq2, Random& rng) const 
    {
        return this->getOperation(rng)(seq1, seq2, rng);
    }

};

}

#endif
