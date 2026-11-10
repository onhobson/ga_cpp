#ifndef GA_CONSTRAINTS_HPP
#define GA_CONSTRAINTS_HPP

#include "ga/common/contexts.hpp"
#include "ga/common/random.hpp"
#include "ga/common/hash.hpp"

#include "ga/core/chromosome.hpp"

#include <concepts>
#include <ranges>
#include <type_traits>


namespace ga::constraints
{
    /**
     * @brief A concept that checks if a type is a permutable sequence.
     * 
     * @returns `true` if the type is copy constructible, a random access range, a sized range, and permutable; otherwise, returns `false`.
     */
    template <typename Sequence>
    concept Permutable =
        std::copy_constructible<Sequence> &&
        std::ranges::random_access_range<Sequence> &&
        std::ranges::sized_range<Sequence> &&
        std::permutable<std::ranges::iterator_t<Sequence>>;
        

    /**
     * @brief A concept that checks if a type is hashable.
     * 
     * @returns `true` if the type is equality comparable and can be hashed using `ga::Hash<T>`; otherwise, returns `false`.
     */
    template <typename T>
    concept Hashable = 
        std::equality_comparable<T> &&
        requires(const T& v)
        {
            { ga::Hash<T>{}(v) } -> std::convertible_to<std::size_t>;
        }; 

    /**
     * @brief A concept that checks if a type has a `value_type` member.
     * 
     * @returns `true` if the type has a `value_type` member; otherwise, returns `false`.
     */
    template <typename T>
    concept HasValueType =
        requires()
        {
            typename T::value_type;
        };

    /**
     * @brief A concept that checks if a type is a valid callback function for the genetic algorithm.
     * 
     * @returns `true` if the type is invocable with a `ga::context::CallbackContext` instance; otherwise, returns `false`.
     */
    template <typename Callback, typename Sequence, typename Fitness>
    concept CallbackFunction =
        std::invocable<
            Callback&,
            const ga::context::CallbackContext<Sequence, Fitness>&
        >;

    /**
     * @brief A concept that checks if a type is a valid initialization function for the genetic algorithm.
     * 
     * @returns `true` if the type is invocable with a `const Sequence&`, `std::size_t`, and `ga::Random&`, and returns a `std::vector<Sequence>`; otherwise, returns `false`.
     */
    template <typename InitFunction, typename Sequence>
    concept InitializationFunction =
        std::invocable<
            InitFunction&,
            const Sequence&,
            std::size_t,
            Random&
        > &&
        std::same_as<
            std::invoke_result_t<
                InitFunction&, 
                const Sequence&, 
                std::size_t, 
                Random&
            >, 
            std::vector<Sequence>
        >;

    /**
     * @brief A concept that checks if a type is a valid termination condition for the genetic algorithm.
     * 
     * @returns `true` if the type is invocable with a `ga::context::TerminationContext` instance and returns a `bool`; otherwise, returns `false`.
     */
    template <typename Terminator, typename Fitness>
    concept TerminationCondition =
        std::invocable<
            Terminator&,
            const context::TerminationContext<Fitness>&
        > &&
        std::convertible_to<
            std::invoke_result_t<
                Terminator&,
                const context::TerminationContext<Fitness>&
            >,
            bool
        >;

}

#endif
