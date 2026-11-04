#ifndef GA_CORE_CONFIG_H
#define GA_CORE_CONFIG_H

#include <format>
#include <stdexcept>
#include <vector>
#include <optional>

namespace ga
{

/**
 * @brief Configuration parameters for the genetic algorithm.
 *
 * The configuration controls population management, selection, and reproduction.
 *
 * Configuration parameters may be assigned in any order.
 * The complete configuration is validated when constructing
 * a GeneticAlgorithm instance.
 *
 * @note `eliteCount` must be less than `populationSize`.
 * @note `tournamentSize` must not exceed `populationSize`.
 */
// template <typename Fitness>
struct Config
{
    /**
     * @brief Maximum number of generations to execute.
     *
     * @details
     * Must be greater than zero.
     */
    std::size_t generationCount { 500 };

    /**
     * @brief Number of individuals in each generation.
     *
     * @details
     * Must be greater than zero.
     * Must be greater than eliteCount.
     */
    std::size_t populationSize { 30 };

    /**
     * @brief Number of elite individuals preserved between generations.
     *
     * @details
     * Must be less than populationSize.
     */
    std::size_t eliteCount { 3 };

    /**
     * @brief Number of individuals competing in tournament selection.
     *
     * @details
     * Must be greater than zero and no greater than populationSize.
     */
    std::size_t tournamentSize { 3 };

    /**
     * @brief Probability of applying crossover to a pair of parents.
     *
     * @details
     * Must be within the inclusive range [0.0, 1.0].
     */
    double crossoverRate { 0.7 };

    /**
     * @brief Interval at which the callback function is invoked.
     *
     * @details
     * A value of 0 disables callbacks
     */
    std::size_t callbackInterval { 1 };


    /**
     * @brief Validates the configuration parameters.
     *
     * @throws `std::invalid_argument` If any configuration parameter
     *         violates its requirements.
     */
    void validate() const 
    {
        if (generationCount == 0)
            throw std::invalid_argument("Generation count can not be 0");
            

        if (populationSize == 0)
            throw std::invalid_argument("Population size can not be 0");
        if (populationSize < eliteCount)
            throw std::invalid_argument(std::format("Population size can not be less than elite count: {} < {}", populationSize, eliteCount));

            
        if (eliteCount > populationSize)
            throw std::invalid_argument(std::format("Elite count can not be greater than population size: {} > {}", eliteCount, populationSize));

            
        if (crossoverRate < 0 || crossoverRate > 1)
            throw std::invalid_argument("Crossover rate must be between 0 and 1");


        if (tournamentSize == 0)
            throw std::invalid_argument("Tournament size can not be 0");
        if (tournamentSize > populationSize)
            throw std::invalid_argument(std::format("Tournament size can not be greater than population size: {} > {}", tournamentSize, populationSize));
    }
};

}

#endif
