#ifndef GA_EVALUATOR_HPP
#define GA_EVALUATOR_HPP

#include "ga/core/chromosome.hpp"

#include <concepts>
#include <functional>

namespace ga::detail
{

/**
 * @brief A class that evaluates the fitness of a population of chromosomes using the given evaluation function.
 * @tparam Sequence The type of the sequences in the chromosomes.
 * @tparam Fitness The type returned by the evaluation function.
 */
template <typename Sequence, typename Fitness>
class Evaluator
{
public:
    using Chromosome = ga::Chromosome<Sequence, Fitness>;
    
    Evaluator(std::function<Fitness(const Sequence&)> evaluate)
        : m_evaluate { evaluate }
    { }

    /**
     * @brief Calls the given evaluation function on each chromosome in the population and updates the `fitness` member of each chromosome with the result.
     * 
     * @returns `std::size_t` The index of the chromosome with the highest fitness
     */
    std::size_t evaluate(std::vector<Chromosome>& population) const 
    {
        std::size_t best = 0;

        for (std::size_t i = 0; i < population.size(); ++i)
        {
            population[i].fitness = m_evaluate(population[i].sequence);

            if (population[i].fitness > population[best].fitness)
                best = i;
        }

        return best;
    }

private:
    std::function<Fitness(const Sequence&)> m_evaluate;
};

}

#endif
