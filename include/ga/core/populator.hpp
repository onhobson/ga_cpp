#ifndef GA_POPULATOR_HPP
#define GA_POPULATOR_HPP

#include "../common/random.hpp"

#include "config.hpp"

#include "../operations/crossover.hpp"
#include "../operations/mutate.hpp"

#include <algorithm>
#include <vector>

#include <iostream>

namespace ga::detail
{
    
template <typename Sequence>
class Populator
{
public:
    Populator(
        const Crossover<Sequence>& crossover,
        const Mutate<Sequence>& mutate,
        const Config& config
    )   
        : m_crossover { crossover }
        , m_mutate { mutate }
        , m_config { config }
    { }

    std::vector<Sequence> createRandomizedPopulation(const Sequence& baseSequence, Random& rng) const
    {
        std::vector<Sequence> population {};
        population.reserve(m_config.populationSize());

        // Add given base sequence
        population.push_back( Sequence {baseSequence} );
        Sequence sequenceCopy { baseSequence };

        // Add random shuffles of sequence until population size is reached
        while (population.size() < m_config.populationSize())
        {
            std::shuffle(sequenceCopy.begin(), sequenceCopy.end(), rng.getEngine());

            for (const auto& e : sequenceCopy)
                std::cout << e << ", ";
            std::cout << '\n';

            population.push_back(sequenceCopy);
        }

        return population;
    }


    Sequence createSequence(const Sequence& seq1, const Sequence& seq2, Random& rng) const
    {
        Sequence newSequence;

        if (rng.getReal(0.0, 1.0) < m_config.crossoverRate())
        {
            newSequence = m_crossover(seq1, seq2, rng);
        }
        else 
        {
            newSequence = m_mutate(seq1, rng);
        }

        return newSequence;
    }

private:
    const Crossover<Sequence> m_crossover;
    const Mutate<Sequence> m_mutate;

    const Config& m_config;
};

}

#endif
