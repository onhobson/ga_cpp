#ifndef GA_POPULATOR_HPP
#define GA_POPULATOR_HPP

#include "ga/common/random.hpp"

#include "ga/operations/crossover.hpp"
#include "ga/operations/mutate.hpp"


namespace ga::detail
{

/**
 * @brief Provides access to and controls lifetimes of `Crossover` and `Mutate`.
 * 
 * @tparam Sequence The type that is used by the given crossover and mutation operators.
 */
template <typename Sequence>
class Populator
{
public:
    Populator(
        const Crossover<Sequence>& crossover,
        const Mutate<Sequence>& mutate,
        double crossoverRate
    )   
        : m_crossover { crossover }
        , m_mutate { mutate }
        , m_crossoverRate { crossoverRate }
    { }

    /**
     * @brief Creates a new sequence based on the given two parent sequences, using crossover and mutation operations.
     * 
     * Randomly selects either a crossover or mutation operation based on the crossover rate specified in the `ga::Config` object.
     * Forwards either the two parent sequences for crossover, or the first parent sequence for mutation, to the selected operation.
     */
    Sequence createSequence(const Sequence& seq1, const Sequence& seq2, Random& rng) const
    {
        Sequence newSequence;

        if (rng.getReal(0.0, 1.0) < m_crossoverRate)
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

    const double m_crossoverRate;
};

}

#endif
