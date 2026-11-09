#ifndef GA_COMMON_TERMINATORS_HPP
#define GA_COMMON_TERMINATORS_HPP

#include "ga/common/constraints.hpp"
#include "ga/common/contexts.hpp"

#include <optional>

namespace ga::terminate
{

struct NoOpTerminator
{
    template <typename Fitness>
    bool operator()(const detail::TerminationContext<Fitness>& context) const
    {
        return false;
    }
};

#pragma region AGGREGATES

template <typename... Conditions>
class AnyOf
{
public:
    explicit AnyOf(Conditions... conditions) 
        : m_conditions { std::move(conditions)... }
    { }

    template <typename Fitness>
    requires (constraints::TerminationCondition<Conditions, Fitness> && ...)
    bool operator()(const detail::TerminationContext<Fitness>& context)
    {
        return std::apply(
            [&](auto&... condition)
            {
                return (condition(context) || ...);
            },
            m_conditions
        );
    }

private:
    std::tuple<Conditions...> m_conditions;
};

template <typename... Conditions>
class AllOf
{
public:
    explicit AllOf(Conditions... conditions)
        : m_conditions { std::move(conditions)... }
    { }

    template <typename Fitness>
    requires (constraints::TerminationCondition<Conditions, Fitness> && ...)
    bool operator()(const detail::TerminationContext<Fitness>& context)
    {
        return std::apply(
            [&](auto&... condition)
            {
                return (condition(context) && ...);
            },
            m_conditions
        );
    }

private:
    std::tuple<Conditions...> m_conditions;
};

#pragma endregion AGGREGATES


#pragma region COMPOSITIONS

struct GenerationLimit
{
    std::size_t count { 100 };

    template <typename Fitness>
    bool operator()(const detail::TerminationContext<Fitness>& context) const
    {
        return context.generation >= count;
    }
};

template <typename Fitness>
struct FitnessThreshold
{
    Fitness threshold;

    bool operator()(const detail::TerminationContext<Fitness>& context) const
    {
        return context.bestFitness >= threshold;
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

    bool operator()(const detail::TerminationContext<Fitness>& context)
    {
        if ((m_margin.has_value() && (context.bestFitness - m_previousBest) >= m_margin.value()) || // Fitness improvement meets margin req
            (!m_margin.has_value() && context.bestFitness > m_previousBest)) // No margin req, fitness is improved
        {
            // New best, reset stagnation
            m_stagnantGenerations = 0;
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

#pragma endregion COMPOSITIONS

#pragma region POLICY

// MAIN WRAPPER
template <typename Condition = NoOpTerminator>
class TerminationPolicy
{
public:
    TerminationPolicy(
        std::size_t generationLimit = 100,
        Condition condition = {}
    )
        : m_generationLimit { GenerationLimit { generationLimit } }
        , m_condition { std::move(condition) }
    { }

    template <typename Fitness>
    requires constraints::TerminationCondition<Condition, Fitness>
    bool operator()(const detail::TerminationContext<Fitness>& context)
    {
        return m_generationLimit(context) || m_condition(context);
        
    }

private:
    GenerationLimit m_generationLimit;
    Condition m_condition;
};

#pragma endregion POLICY

}


#endif 
