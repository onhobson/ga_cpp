#ifndef GA_CORE_SELECTOR_HPP
#define GA_CORE_SELECTOR_HPP

#include "ga/utility/random.hpp"

#include "ga/core/chromosome.hpp"

#include <utility>


namespace ga::detail
{

/**
 * @brief Handles tournament selection logic for selecting chromosomes from a population.
 */
template <typename Sequence, typename Fitness, typename Compare>
class Selector
{
    using Chromosome = ga::Chromosome<Sequence, Fitness>;

public:
    Selector(
        std::size_t tournamentSize,
        Random& rng
    )
        : m_tournamentSize { (tournamentSize) ? tournamentSize : 1 } // Upgrade size = 0 to size = 1
        , m_rng { rng }
    { }
    
    /**
     * @brief Selects two chromosomes from the given population using tournament selection.
     * 
     * Applies Algorithm R (Reservoir Sampling) to select two unique chromosomes from the population based on their fitness values.
     * 
     * If population size is 1, returns references to the same chromosome.
     * 
     * @param population The population of chromosomes to select from.
     * @return A pair of references to the selected chromosome(s).
     */
    std::pair<const Chromosome&, const Chromosome&> select(const std::vector<Chromosome>& population)
    {
        if (population.size() == 1)
            return { population[0], population[0] };

        // Store each Chromosome index to be used in return pair
        std::vector<std::size_t> chosen(2, std::numeric_limits<std::size_t>::max());

        // Algorithm R
        for (std::size_t choose { 0 }; choose < chosen.size(); ++choose)
        {             
            std::vector<std::size_t> samples;
            samples.reserve(m_tournamentSize);

            // Separate counter to handle skipping first chosen index in second run
            std::size_t seen { 0 }; 

            // Randomly select subsequent items to override sample items
            for (std::size_t i { 0 }; i < population.size(); ++i)
            { 
                if (i == chosen[0])
                    continue;

                if (seen < m_tournamentSize)
                {
                    samples.push_back(i);
                }
                else
                {
                    // Uniformly select integer in [0, seen]
                    const std::size_t j {
                        m_rng.get<std::size_t>(0, seen + 1)
                    };

                    if (j < m_tournamentSize)
                        samples[j] = i;
                }

                ++seen;
            }

            // Select highest fitness sample
            chosen[choose] = std::ranges::min(
                samples, 
                m_compare, 
                [&population](const auto& index)
                    {
                        return population[index].fitness;
                    }
            );
        }

        return { population[chosen[0]], population[chosen[1]] };
    }

private:
    std::size_t m_tournamentSize {};
    Compare m_compare {};
    Random& m_rng;
};

}

#endif
