#ifndef GA_COMMON_TERMINATORS_HPP
#define GA_COMMON_TERMINATORS_HPP

#include "ga/common/constraints.hpp"
#include "ga/common/contexts.hpp"

#include <optional>

namespace ga::terminate
{

/**
 * @brief A no-operation terminator that always returns false.
 * 
 * This terminator can be used as a default or placeholder when no termination condition is desired.
 */
struct NoOpTerminator
{
    template <typename Fitness>
    bool operator()(const context::TerminationContext<Fitness>& context) const
    {
        return false;
    }
};

#pragma region AGGREGATES

/**
 * @brief A terminator that returns true if any of the given termination conditions return true.
 * 
 * @tparam Conditions The types of the termination conditions to combine. Must satisfy the `ga::constraints::TerminationCondition` concept.
 */
template <typename... Conditions>
class AnyOf
{
public:
    explicit AnyOf(Conditions... conditions) 
        : m_conditions { std::move(conditions)... }
    { }

    template <typename Fitness>
    requires (constraints::TerminationCondition<Conditions, Fitness> && ...)
    bool operator()(const context::TerminationContext<Fitness>& context)
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

/**
 * @brief A terminator that returns true only if all of the given termination conditions return true.
 * 
 * @tparam Conditions The types of the termination conditions to combine. Must satisfy the `ga::constraints::TerminationCondition` concept.
 */
template <typename... Conditions>
class AllOf
{
public:
    explicit AllOf(Conditions... conditions)
        : m_conditions { std::move(conditions)... }
    { }

    template <typename Fitness>
    requires (constraints::TerminationCondition<Conditions, Fitness> && ...)
    bool operator()(const context::TerminationContext<Fitness>& context)
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

/**
 * @brief A terminator that returns true if the current generation number is greater than or equal to a specified limit.
 */
struct GenerationLimit
{
    std::size_t count { 100 };

    template <typename Fitness>
    bool operator()(const context::TerminationContext<Fitness>& context) const
    {
        return context.generation >= count;
    }
};

/**
 * @brief A terminator that returns true if the best fitness value is greater than or equal to a specified threshold.
 * 
 * @tparam Fitness The type of the fitness value used in the genetic algorithm.
 */
template <typename Fitness>
struct FitnessThreshold
{
    Fitness threshold;

    bool operator()(const context::TerminationContext<Fitness>& context) const
    {
        return context.bestFitness >= threshold;
    }
};

/**
 * @brief A terminator that returns true if the best fitness value has not improved for a specified number of generations.
 * 
 * @tparam Fitness The type of the fitness value used in the genetic algorithm.
 * 
 * @param limit The number of generations to wait for an improvement before terminating.
 * @param margin An optional margin that defines the minimum improvement required to reset the stagnation counter. If not provided, any improvement will reset the counter.
 */
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

    bool operator()(const context::TerminationContext<Fitness>& context)
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

/**
 * @brief A termination policy that combines a generation limit with an optional custom termination condition.
 * 
 * @tparam Condition The type of the custom termination condition. Must satisfy the `ga::constraints::TerminationCondition` concept. Defaults to `NoOpTerminator`.
 */
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
    bool operator()(const context::TerminationContext<Fitness>& context)
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
