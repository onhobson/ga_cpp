#ifndef GA_GENETIC_ALGORITHM_HPP
#define GA_GENETIC_ALGORITHM_HPP

#include "../common/random.hpp"

#include "chromosome.hpp"
#include "config.hpp"
#include "evaluator.hpp"
#include "populator.hpp"

#include "../operations/crossover.hpp"
#include "../operations/operators.hpp"
#include "../operations/mutate.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <vector>
#include <utility>
#include <functional>
#include <queue>
#include <random>

namespace ga
{

/**
 * @brief A class that implements a genetic algorithm to evolve a population of chromosomes towards an optimal solution.
 * 
 * @tparam Sequence The type that is used by the given crossover and mutation operators.
 * @tparam Fitness The type that is returned by the given evaluation function.
 * @tparam URBG The type of the random number generator engine to use. Defaults to `std::mt19937`.
 *         Must satisfy the requirements of a uniform random bit generator.
 * 
 * @throws `std::invalid_argument` If the configuration parameters or operators are invalid.
 */
template <typename Sequence, typename Fitness, std::uniform_random_bit_generator URBG = std::mt19937>
class GeneticAlgorithm
{
public:
    /// @brief Represents a member of the population, containing a `Sequence` and its associated `Fitness` value.
    using Chromosome = ga::Chromosome<Sequence, Fitness>;

    /**
     * @brief Constructs a GeneticAlgorithm object with the given base sequence, evaluation function, operators, configuration, and random number generator engine.
     * 
     * @param baseSequence The initial `Sequence` from which the population will be generated.
     * @param evaluationFunction A function that takes a `Sequence` and returns its `Fitness` value.
     * @param operators A `ga::Operators` struct containing the crossover and mutation operators to use.
     * @param config A `ga::Config` object specifying the parameters of the genetic algorithm.
     * @param randomEngine An optional random number generator engine to use. Defaults to a Mersenne Twister engine.
     */
    GeneticAlgorithm(
        Sequence baseSequence,
        std::function<Fitness(const Sequence&)> evaluationFunction,
        Operators<Sequence> operators,
        Config config,
        // void (*callback)(const std::vector<Chromosome>, Chromosome),
        URBG randomEngine = URBG ( std::move(Random::generateMtEngine()) )
    )
        : m_baseSequence { baseSequence }
        , m_evaluator { evaluationFunction }
        , m_config { config }
        , m_populator { detail::Populator<Sequence>(operators.crossover, operators.mutate, m_config) }
        , m_rng { Random(randomEngine) }
    { } 

    /**
     * @brief Runs the genetic algorithm for the number of generations specified in the `ga::Config` object, and returns the best `Chromosome` found.
     */
    Chromosome run()
    {
        initializePopulation();
        evaluateCurrentGeneration();
        
        for (int i {0}; i < m_config.generationCount(); ++i)
        {
            createNewGeneration();
            
            evaluateCurrentGeneration();
        }

        return *m_topElite;
    }

    void printCurrentGeneration() const
    {        
        for (const auto& chr : m_currentPopulation)
        {
            std::cout << "Fitness: " << chr.fitness << ", Seq: [ ";

            for (const auto& gene : chr.sequence)
                std::cout << gene << ' ';

            std::cout << "]\n";
        }
    }

private:
    const Sequence m_baseSequence {};
    std::function<Fitness(Sequence)> m_evaluationFunction {};

    std::vector<Chromosome> m_currentPopulation {};
    // Min-heap
    std::vector<Chromosome*> m_elites;
    Chromosome* m_topElite;

    const Config m_config;
    const detail::Populator<Sequence> m_populator;
    const detail::Evaluator<Sequence, Fitness> m_evaluator;
    Random m_rng {};


    void initializePopulation()
    {
        m_currentPopulation.reserve(m_config.populationSize());
        
        std::vector<Sequence> randomizedSequences { m_populator.createRandomizedPopulation(m_baseSequence, m_rng) };

        for (std::size_t i {0}; i < randomizedSequences.size(); ++i)
        {
            m_currentPopulation.push_back({
                std::move(randomizedSequences[i]), 0
            });
        }

        m_topElite = &m_currentPopulation[0];
        for (std::size_t i {0}; i < m_config.eliteCount(); ++i)
        {
            m_elites.push_back(&m_currentPopulation[i]);

            if (m_elites.back()->fitness > m_topElite->fitness)
                m_topElite = m_elites.back();
        }
        std::ranges::make_heap(m_elites, std::ranges::greater{}, &Chromosome::fitness);
    }

    void createNewGeneration()
    {
        std::vector<Chromosome> newGeneration {};
        newGeneration.reserve(m_config.populationSize());

        for (auto& elite : m_elites)
        {
            newGeneration.push_back(*elite);

            if (elite == m_topElite)
                m_topElite = &newGeneration.back();

            elite = &newGeneration.back();
        }

        // Fill rest of population
        while (newGeneration.size() < m_config.populationSize())
        {
            std::size_t firstIndex { getRandomPopulationIndex() };
            std::size_t secondIndex { getRandomPopulationIndex(firstIndex) };
            
            newGeneration.push_back({
                m_populator.createSequence(
                    m_currentPopulation[firstIndex].sequence,
                    m_currentPopulation[secondIndex].sequence,
                    m_rng
                ), 0
            });
        }    

        m_currentPopulation = std::move(newGeneration);
    }

    void evaluateCurrentGeneration()
    {
        m_evaluator.evaluate(m_currentPopulation);
        selectElites();
    }

    void selectElites()
    {
        for (std::size_t i {m_config.eliteCount()}; i < m_currentPopulation.size(); ++i)
        {
            if (m_elites.front()->fitness < m_currentPopulation[i].fitness)
            {
                std::ranges::pop_heap(m_elites, std::ranges::greater{}, &Chromosome::fitness);
                
                m_elites.back() = &m_currentPopulation[i];
                if (m_currentPopulation[i].fitness > m_topElite->fitness)
                    m_topElite = m_elites.back();

                std::ranges::push_heap(m_elites, std::ranges::greater{}, &Chromosome::fitness);
            }
        }
    }

    std::size_t getRandomPopulationIndex(std::size_t omit = -1)
    {
        return m_rng.getIntegerExclude<std::size_t>(0, m_config.populationSize() - 1, omit);
    }
};

}

#endif
