#ifndef GA_CONFIG_H
#define GA_CONFIG_H

#include <format>
#include <stdexcept>
#include <vector>


namespace ga
{

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
    void generationCount(std::size_t generationCount)
    {
        if (generationCount == 0)
            throw std::invalid_argument("Generation count can not be 0");

        m_generationCount = generationCount;
    }

    std::size_t populationSize() const { return m_populationSize; }
    void populationSize(std::size_t populationSize)
    {
        if (populationSize == 0)
            throw std::invalid_argument("Population size can not be 0");
        if (populationSize < m_eliteCount)
            throw std::invalid_argument(std::format("Population size can not be less than elite count: {} < {}", populationSize, m_eliteCount));

        m_populationSize = populationSize;
    }

    std::size_t eliteCount() const { return m_eliteCount; }
    void eliteCount(std::size_t eliteCount)
    {
        if (eliteCount > m_populationSize)
            throw std::invalid_argument(std::format("Elite count can not be greater than population size: {} > {}", eliteCount, m_populationSize));

        m_eliteCount = eliteCount;
    }

    double crossoverRate() const { return m_crossoverRate; }
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
