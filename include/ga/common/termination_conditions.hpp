#ifndef GA_COMMON_TERMINATION_CONDITIONS_HPP
#define GA_COMMON_TERMINATION_CONDITIONS_HPP

#include <optional>

namespace ga
{

template <typename Fitness>
struct TerminationContext
{
    std::size_t generation {};
    Fitness bestFitness {};
};

template <typename Fitness>
struct FitnessThreshold
{
    Fitness threshold;

    bool operator()(const TerminationContext<Fitness>& context) const
    {
        return bestFitness >= threshold;
    }
};

template <typename Fitness>
class StagnationLimit
{
public:
    explicit StagnationLimit(std::size_t limit)
        : m_limit { limit }
    { }

    explicit StagnationLimit(std::size_t limit, Fitness margin)
        : m_limit { limit }
        , m_margin { margin }
    { }

    bool operator()(const TerminationContext<Fitness>& context)
    {
        if ((m_margin.has_value() && (context.bestFitness - m_previousBest) > m_margin.value()) || // Fitness improvement meets margin req
            (!m_margin.has_value() && context.bestFitness > m_previousBest)) // No margin req, fitness is improved
        {
            // New best, reset stagnation
            m_limit = 0;
            m_previousBest = context.bestFitness;
            return false;
        } 
        else
        {
            return ++m_stagnantGenerations > m_limit; 
        }
    }

private:
    std::size_t m_limit;
    std::optional<Fitness> m_margin;
    std::size_t m_stagnantGenerations {};
    Fitness m_previousBest {};
};


template <typename... Conditions>
class AnyOf
{
public:
    explicit AnyOf(Conditions... conditions) 
        : m_conditions { std::move(conditions)... }
    { }

    template <typename... Args>
    bool operator()(Args&&... args)
    {
        return std::apply(
            [&](auto&... condition)
            {
                return (condition(std::forward<Args>(args)...) || ...);
            },
            conditions
        );
    }

private:
    std::tuple<Conditions...> m_conditions;
};

}


#endif 
