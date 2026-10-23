#ifndef GA_EVALUATOR_HPP
#define GA_EVALUATOR_HPP

#include "chromosome.hpp"

#include <functional>

namespace ga::detail
{

template <typename Sequence, typename Fitness>
class Evaluator
{
public:
    using Chromosome = ga::Chromosome<Sequence, Fitness>;
    
    Evaluator(std::function<Fitness(const Sequence&)> evaluate)
        : m_evaluate { evaluate }
    { }

    void evaluate(std::vector<Chromosome>& population) const 
    {
        for (auto& chromosome : population)
        {
            chromosome.fitness = m_evaluate(chromosome.sequence);
        }
    }

private:

    std::function<Fitness(const Sequence&)> m_evaluate;
};

}

#endif
