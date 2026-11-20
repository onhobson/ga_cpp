#ifndef GA_CORE_GENETIC_ALGORITHM_HPP
#define GA_CORE_GENETIC_ALGORITHM_HPP

#include "ga/builtins/initializers.hpp"
#include "ga/builtins/terminators.hpp"

#include "ga/core/chromosome.hpp"
#include "ga/core/config.hpp"
#include "ga/core/evaluator.hpp"
#include "ga/core/populator.hpp"
#include "ga/core/selector.hpp"
#include "ga/core/result.hpp"

#include "ga/operations/crossover.hpp"
#include "ga/operations/operators.hpp"
#include "ga/operations/mutate.hpp"

#include "ga/types/contexts.hpp"
#include "ga/types/run_options.hpp"

#include "ga/utility/constraints.hpp"
#include "ga/utility/random.hpp"

#include <format>
#include <functional>
#include <random>
#include <vector>


namespace ga
{

/**
 * @brief A class that implements a genetic algorithm to evolve a population of chromosomes towards an optimal solution.
 * 
 * @tparam Sequence The type that is used by the given crossover and mutation operators.
 * @tparam Fitness The type that is returned by the given evaluation function. Must satisfy `std::totally_ordered<Fitness>`
 * @tparam Compare A comparison function object that defines the ordering of fitness values. Must satisfy `std::strict_weak_order<Compare, Fitness, Fitness>`. Defaults to `std::ranges::greater`.
 * @tparam URBG The type of the random number generator engine to use. Defaults to `std::mt19937`.
 *         Must satisfy the requirements of a uniform random bit generator.
 * 
 * @throws `std::invalid_argument` If the configuration parameters or operators are invalid.
 */
template <
    typename Sequence, 
    typename Fitness, 
    typename Compare = std::ranges::less
>
requires (
    std::totally_ordered<Fitness> &&
    std::strict_weak_order<Compare, Fitness, Fitness>
)
class GeneticAlgorithm
{
public:
    /// @brief Represents a member of the population, containing a `Sequence` and its associated `Fitness` value.
    using Chromosome = ga::Chromosome<Sequence, Fitness>;

    /**
     * @brief Constructs a GeneticAlgorithm object with the given base sequence, evaluation function, operators, configuration, and random number generator engine.
     * 
     * @param evaluationFunction A function that takes a `Sequence` and returns its `Fitness` value.
     * @param operators A `ga::Operators` struct containing the crossover and mutation operators to use.
     * @param config A `ga::Config` object specifying the parameters of the genetic algorithm.
     * @param randomEngine An optional random number generator engine to use. Defaults to a Mersenne Twister engine.
     */
    GeneticAlgorithm(
        std::function<Fitness(const Sequence&)> evaluationFunction,
        Operators<Sequence> operators,
        Config config
    )
        : m_evaluator { 
            evaluationFunction, 
            config.eliteCount 
        }
        , m_config { config }
        , m_populator {
            operators.crossover, 
            operators.mutate,
            config.crossoverRate
        }
        , m_rng {}
        , m_selector { 
            m_config.tournamentSize, 
            m_rng 
        }
    { 
        m_config.validate();
    } 


    /**
     * @brief Creates an initial population using the given initializer and base sequence, then runs the genetic algorithm until the termination condition is met, and returns the best `Chromosome` found.
     * 
     * @param baseSequence The base sequence to use for initializing the population.
     * @param options A `ga::RunOptions` object containing the initializer, terminator, and callback functions to use during the run.
     * 
     * @return A `ga::Result` object containing the best `Chromosome` found and the number of generations processed.
     */
    template <typename Initializer, typename Terminator, typename Callback>
    requires (
        constraints::InitializationFunction<Initializer, Sequence> &&
        constraints::TerminationCondition<Terminator, Fitness> &&
        constraints::CallbackFunction<Callback, Sequence, Fitness>
    )
    Result<Sequence, Fitness> run(
        const Sequence& baseSequence,
        RunOptions<Initializer, Terminator, Callback> options
    ) { 
        return run(
            std::invoke(options.initializer, baseSequence, m_config.populationSize, m_rng),
            std::move(options)
        );
    }

    /**
     * @brief Runs the genetic algorithm until the termination condition is met, and returns the best `Chromosome` found.
     * 
     * @param population The initial population of Sequences to evolve.
     * @param options A `ga::RunOptions` object containing the initializer, terminator, and callback functions to use during the run.
     * 
     * @return A `ga::Result` object containing the best `Chromosome` found and the number of generations processed.
     * 
     * @throws `std::invalid_argument` if given `population` size does not match `config.populationSize` given to GeneticAlgorithm
     */
    template <typename Initializer, typename Terminator, typename Callback>
    requires (
        constraints::TerminationCondition<Terminator, Fitness> &&
        constraints::CallbackFunction<Callback, Sequence, Fitness>
    )
    Result<Sequence, Fitness> run(
        const std::vector<Sequence>& population, 
        RunOptions<Initializer, Terminator, Callback> options
    ) {
        if (population.size() != m_config.populationSize)
            throw std::invalid_argument(std::format(
                "Given population size was not equal to specified population size in Config object. {} vs {}", population.size(), m_config.populationSize
            ));

        // Default all tracking members
        m_bestSolution = {};
        m_sinceLastCallback = 0;
        m_generationCount = 0;

        initializePopulation(population);
        evaluateCurrentGeneration();
        m_bestSolution = m_currentPopulation.front();
        
        // Begin GA loop
        while (!options.terminator(createTerminationContext()))
        {
            assert(m_currentPopulation.size() == m_config.populationSize && "Current population size must stay equal to configured population size");

            ++m_generationCount;

            createNewGeneration();
            
            evaluateCurrentGeneration();

            // const ref to ensure callback does not modify members
            const std::vector<Chromosome>& currentPopulation = m_currentPopulation;
            const Chromosome& bestSolution = m_bestSolution;

            // Call callback if interval not set to 0 and interval reached
            if (m_config.callbackInterval && ++m_sinceLastCallback >= m_config.callbackInterval)
            {
                std::invoke(options.callback, createCallbackContext());
                m_sinceLastCallback = 0;
            }
        } 

        return Result {
            .best = m_bestSolution,
            .generation = m_generationCount
        };
    }

private:
    std::vector<Chromosome> m_currentPopulation {};

    // Stores best solution found during evaluation.
    Chromosome m_bestSolution {};

    std::size_t m_sinceLastCallback { 0 };
    std::size_t m_generationCount { 0 };

    const Config m_config;
    const detail::Populator<Sequence> m_populator;
    detail::Evaluator<Sequence, Fitness, Compare> m_evaluator;
    Compare m_compare {};
    detail::Selector<Sequence, Fitness, Compare> m_selector;
    Random m_rng {};

    // Constructs Chromosomes out of each member in population with default Fitness
    void initializePopulation(const std::vector<Sequence>& population)
    {
        m_currentPopulation.clear();
        m_currentPopulation.reserve(m_config.populationSize); 

        for (const auto& sequence : population)
            m_currentPopulation.emplace_back( Chromosome { .sequence = sequence } ); 
    }

    // Creates a new generation of chromosomes by preserving elites and generating offspring from the current population.
    void createNewGeneration()
    {
        std::vector<Chromosome> newGeneration {};
        newGeneration.reserve(m_config.populationSize);

        // Carry over elites
        for (std::size_t i { 0 }; i < m_config.eliteCount; ++i)
            newGeneration.push_back(m_currentPopulation[i]);

        // Fill rest of population
        while (newGeneration.size() < m_config.populationSize)
        {
            const auto [first, second] { m_selector.select(m_currentPopulation) };
            
            newGeneration.push_back( 
                Chromosome {
                    m_populator.createSequence(
                        first.sequence,
                        second.sequence,
                        m_rng
                    ), 0 
                }
            );
        }    

        m_currentPopulation = std::move(newGeneration);
    }

    // Evaluates the fitness of the current generation and updates the elite min-heap accordingly.
    void evaluateCurrentGeneration()
    {
        m_evaluator.evaluate(m_currentPopulation);

        if (m_compare(m_currentPopulation.front().fitness, m_bestSolution.fitness))
            m_bestSolution = m_currentPopulation.front();
    }
    
    context::TerminationContext<Fitness> createTerminationContext() const
    {
        return context::TerminationContext<Fitness> {
            .generation { m_generationCount },
            .bestFitness { m_bestSolution.fitness }
        };
    }

    context::CallbackContext<Sequence, Fitness> createCallbackContext() const
    {
        return context::CallbackContext<Sequence, Fitness> {
            .population { m_currentPopulation },
            .best { m_bestSolution },
            .generation { m_generationCount }
        };
    }
};

}

#endif
