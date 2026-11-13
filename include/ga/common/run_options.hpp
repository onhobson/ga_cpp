#ifndef GA_COMMON_RUN_OPTIONS_HPP
#define GA_COMMON_RUN_OPTIONS_HPP

#include "ga/common/callbacks.hpp"
#include "ga/common/initializers.hpp"
#include "ga/common/terminators.hpp"

namespace ga
{
    
/**
 * @brief A struct that holds the options for running a genetic algorithm.
 * 
 * @param Initializer The initializer function to use for generating the initial population. Defaults to `ga::init::RandomPermutation`.
 * @param Terminator The terminator function to use for determining when to stop the algorithm. Defaults to `ga::terminate::TerminationPolicy<>`.
 * @param Callback The callback function to use for handling events during the algorithm execution. Defaults to `ga::callback::NoOpCallback`.
 */
template <
    typename Initializer = init::RandomPermutation, 
    typename Condition = terminate::NoOpTerminator, 
    typename Callback = callback::NoOpCallback
>
struct RunOptions
{
    Initializer initializer {};
    terminate::TerminationPolicy<Condition> terminator {};
    Callback& callback { callback::defaultCallback };
};

}

#endif
