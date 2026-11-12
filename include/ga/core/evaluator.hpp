#ifndef GA_EVALUATOR_HPP
#define GA_EVALUATOR_HPP

#include "ga/core/chromosome.hpp"

#include <concepts>
#include <functional>
#include <algorithm>
#include <utility>
#include <vector>

namespace ga::detail
{

/**
 * @brief A class that evaluates the fitness of a population of chromosomes using the given evaluation function.
 * 
 * @tparam Sequence The type of the sequences in the chromosomes.
 * @tparam Fitness The type returned by the evaluation function.
 * @tparam Compare A comparison function object that defines the ordering of fitness values.
 */
template <typename Sequence, typename Fitness, typename Compare>
class Evaluator
{
public:
    using Chromosome = ga::Chromosome<Sequence, Fitness>;
    
    Evaluator(
        std::function<Fitness(const Sequence&)> evaluate, 
        std::size_t eliteCount
    )
        : m_evaluate { evaluate }
        , m_eliteCount { (eliteCount) ? eliteCount : 1 } // Upgrade eliteCount of 0 to 1 to allow GA to track top fitness using .front()
    { }

    /**
     * @brief Calls the given evaluation function on each chromosome in the population and updates the `fitness` member of each chromosome with the result.
     * 
     * Moves the top `eliteCount` (constructor parameter) chromosomes to the front of the population, ordered by decreasing fitness.
     */
    void evaluate(std::vector<Chromosome>& population)
    {
        for (auto& chromosome : population)
        {
            chromosome.fitness = m_evaluate(chromosome.sequence);
        }

        const auto eliteEnd = population.begin() + std::min(m_eliteCount, population.size());
        std::ranges::partial_sort(population, eliteEnd, m_compare, &Chromosome::fitness);
    }

private:
    std::function<Fitness(const Sequence&)> m_evaluate;
    Compare m_compare {};

    std::size_t m_eliteCount;
};

}

#endif
