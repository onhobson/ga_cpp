#ifndef GA_GENETIC_ALGORITHM_HPP
#define GA_GENETIC_ALGORITHM_HPP

#include "ga/common/random.hpp"

#include "ga/core/chromosome.hpp"
#include "ga/core/config.hpp"
#include "ga/core/evaluator.hpp"
#include "ga/core/populator.hpp"

#include "ga/operations/crossover.hpp"
#include "ga/operations/operators.hpp"
#include "ga/operations/mutate.hpp"

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

    /// @brief A no-operation callback that does nothing when invoked.
    struct NoOpCallback
    {
        void operator()(const std::vector<Chromosome>&, const Chromosome&) const noexcept
        { }
    };

    /**
     * @brief Runs the genetic algorithm for the number of generations specified in the `ga::Config` object, and returns the best `Chromosome` found.
     * 
     * @param callback An optional callback function that is invoked at the end of each generation with the current population and the current best solution.
     *                 Must be invocable with `const std::vector<Chromosome>&` and `const Chromosome&`.
     * 
     * @return The best `Chromosome` found after running the genetic algorithm for the specified number of generations.
     */
    template <typename Callback = NoOpCallback>
        requires std::invocable<
            Callback&,                      // Fn
            const std::vector<Chromosome>&, // Population
            const Chromosome&               // Elite
        >
    Chromosome run(Callback&& callback = {})
    {
        initializePopulation();
        evaluateCurrentGeneration();
        
        for (int i {0}; i < m_config.generationCount(); ++i)
        {
            createNewGeneration();
            
            evaluateCurrentGeneration();

            // const ref to ensure callback does not modify members
            const std::vector<Chromosome>& currentPopulation = m_currentPopulation;
            const Chromosome& bestSolution = m_bestSolution;

            std::invoke(callback, currentPopulation, bestSolution);
        }

        
        return m_bestSolution;
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

    // Stores best solution found during evaluation.
    Chromosome m_bestSolution;

    const Config m_config;
    const detail::Populator<Sequence> m_populator;
    const detail::Evaluator<Sequence, Fitness> m_evaluator;
    Random m_rng {};

    // Initializes the population with randomized sequences and sets up the elite min-heap.
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

        // Initialize the elite min-heap with the front portion of the current population.
        for (std::size_t i {0}; i < m_config.eliteCount(); ++i)
        {
            m_elites.push_back(&m_currentPopulation[i]);
        }

        std::ranges::make_heap(m_elites, std::ranges::greater{}, &Chromosome::fitness);
    }

    // Creates a new generation of chromosomes by preserving elites and generating offspring from the current population.
    void createNewGeneration()
    {
        std::vector<Chromosome> newGeneration {};
        newGeneration.reserve(m_config.populationSize());

        for (auto& elite : m_elites)
        {
            newGeneration.push_back(*elite);

            // Ensure elite ptr remains valid in new generation
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

    // Evaluates the fitness of the current generation and updates the elite min-heap accordingly.
    void evaluateCurrentGeneration()
    {
        std::size_t bestIndex { m_evaluator.evaluate(m_currentPopulation) };

        if (m_currentPopulation[bestIndex].fitness > m_bestSolution.fitness)
            m_bestSolution = m_currentPopulation[bestIndex];

        if (m_config.eliteCount() > 0)
            selectElites();
    }

    // Selects the top-performing chromosomes from the current population and updates the elite min-heap.
    void selectElites()
    {
        // Start after elite carry-over section of population
        for (std::size_t i {m_config.eliteCount()}; i < m_currentPopulation.size(); ++i)
        {
            // If current fitness is better than worst in min-heap m_elites
            if (m_elites.front()->fitness < m_currentPopulation[i].fitness)
            {
                std::ranges::pop_heap(m_elites, std::ranges::greater{}, &Chromosome::fitness);
                
                // Push current pop into m_elites
                m_elites.back() = &m_currentPopulation[i];

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
