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

template <typename Sequence, typename Fitness, std::uniform_random_bit_generator URBG = std::mt19937>
class GeneticAlgorithm
{
public:
    using Chromosome = ga::Chromosome<Sequence, Fitness>;

    GeneticAlgorithm(
        Sequence baseSequence,
        std::function<Fitness(const Sequence&)> evaluationFunction,
        Operators<Sequence> operators,
        Config config,
        URBG randomEngine = URBG ( std::move(Random::generateMtEngine()) )
    )
        : m_baseSequence { baseSequence }
        , m_evaluator { evaluationFunction }
        , m_config { config }
        , m_populator { detail::Populator<Sequence>(operators.crossover, operators.mutate, m_config) }
        , m_rng { Random(randomEngine) }
    { } 

    Chromosome run()
    {
        initializePopulation();
        evaluateCurrentGeneration();
        
        for (int i {0}; i < m_config.generationCount(); ++i)
        {
            createNewGeneration();
            
            evaluateCurrentGeneration();

            printCurrentGeneration();
            std::cout << "\n\n";
        }

        while (m_elites.size() > 1)
            m_elites.pop();

        return m_elites.top();
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
    struct CompareFitness
    {
        bool operator()(const Chromosome& c1, const Chromosome& c2)
        {
            return c1.fitness > c2.fitness; // For min-heap
        };
    };

    const Sequence m_baseSequence {};
    std::function<Fitness(Sequence)> m_evaluationFunction {};

    std::vector<Chromosome> m_currentPopulation {};
    std::priority_queue<Chromosome, std::vector<Chromosome>, CompareFitness> m_elites;

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
    }

    void createNewGeneration()
    {
        std::vector<Chromosome> newGeneration {};
        newGeneration.reserve(m_config.populationSize());

        // Carry over elites
        while (!m_elites.empty())
        {
            newGeneration.push_back(m_elites.top());
            m_elites.pop();
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

        m_currentPopulation = newGeneration;
    }

    void evaluateCurrentGeneration()
    {
        m_evaluator.evaluate(m_currentPopulation);
        selectElites();
    }

    void selectElites()
    {
        clearElites(m_elites);

        for (const auto& chromosome : m_currentPopulation)
        {
            if (m_elites.size() < m_config.eliteCount())
            {
                m_elites.push(chromosome);
            }
            else if (m_elites.top().fitness < chromosome.fitness)
            {
                m_elites.pop();
                m_elites.push(chromosome);
            }
        }
    }

    std::size_t getRandomPopulationIndex(std::size_t omit = -1)
    {
        return m_rng.getIntegerExclude<std::size_t>(0, m_config.populationSize() - 1, omit);
    }
    
    template <typename Q>
    void clearElites(Q& q)
    {
        q = Q();
    }
};

}

#endif
