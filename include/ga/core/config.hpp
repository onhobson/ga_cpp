#ifndef GA_CONFIG_H
#define GA_CONFIG_H

#include <format>
#include <stdexcept>
#include <vector>


namespace ga
{

/**
 * @brief A class that holds the configuration parameters for the genetic algorithm.
 * 
 *  Configuration Parameters 
 * @anchor `ga::Config::generationCount`
 *  - The number of generations to run.
 * 
 * @anchor `ga::Config::populationSize`
 *  - The number of individuals in each generation.
 * 
 * @anchor `ga::Config::eliteCount`
 *  - The number of elite individuals to carry over to the next generation.
 * 
 * @anchor `ga::Config::crossoverRate`
 *  - The probability of crossover occurring between two parent sequences.
 * 
 * @throws `std::invalid_argument` If any of the configuration parameters are invalid.
 */
class Config
{
public:
    Config() = default;

    Config(
        std::size_t generation_count,
        std::size_t population_size,
        std::size_t elite_count,
        double crossover_rate
    ) {
        generationCount(generation_count);
        populationSize(population_size);
        eliteCount(elite_count);
        crossoverRate(crossover_rate);
    }

    std::size_t generationCount() const { return m_generationCount; }
    /**
     * @brief Sets the number of generations to run.
     * @throws `std::invalid_argument` If the generation count is 0.
     */
    void generationCount(std::size_t generationCount)
    {
        if (generationCount == 0)
            throw std::invalid_argument("Generation count can not be 0");

        m_generationCount = generationCount;
    }

    std::size_t populationSize() const { return m_populationSize; }
    /**
     * @brief Sets the number of individuals in each generation.
     * @throws `std::invalid_argument` If the population size is 0 or less than the elite count.
     */
    void populationSize(std::size_t populationSize)
    {
        if (populationSize == 0)
            throw std::invalid_argument("Population size can not be 0");
        if (populationSize < m_eliteCount)
            throw std::invalid_argument(std::format("Population size can not be less than elite count: {} < {}", populationSize, m_eliteCount));

        m_populationSize = populationSize;
    }

    std::size_t eliteCount() const { return m_eliteCount; }
    /**
     * @brief Sets the number of elite individuals to carry over to the next generation.
     * @throws `std::invalid_argument` If the elite count is greater than the population size.
     */
    void eliteCount(std::size_t eliteCount)
    {
        if (eliteCount > m_populationSize)
            throw std::invalid_argument(std::format("Elite count can not be greater than population size: {} > {}", eliteCount, m_populationSize));

        m_eliteCount = eliteCount;
    }

    double crossoverRate() const { return m_crossoverRate; }
    /**
     * @brief Sets the probability of crossover occurring between two parent sequences.
     * @throws `std::invalid_argument` If the crossover rate is not between 0 and 1.
     */
    void crossoverRate(double crossoverRate)
    {
        if (crossoverRate < 0 || crossoverRate > 1)
            throw std::invalid_argument("Crossover rate must be between 0 and 1");

        m_crossoverRate = crossoverRate;
    }

private:
    std::size_t m_generationCount {};
    std::size_t m_populationSize {};
    std::size_t m_eliteCount {};
 
    double m_crossoverRate {};
};

} // namespace ga


#endif
